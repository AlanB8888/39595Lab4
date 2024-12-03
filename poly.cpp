#include "poly.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include <queue>
#include <utility>
#include <vector>
#include <memory>
#include <mutex>

polynomial::polynomial(){
    polyVec.push_back(std::make_pair(0, 0)); //construct with 0 coeff 0 power only

}

template <typename Iter>
polynomial::polynomial(Iter begin, Iter end) //constructor from iterators
{
    // polyVec.insert(polyVec.end(), begin, end);

    while(begin != end)
    {
        if((*(begin)).second != 0) //dont add the zeros
        {
            polyVec.push_back(*(begin));
        }

        begin++;
    }
}

polynomial::polynomial(const polynomial &other):polyVec(other.polyVec) //copy constructor
{
}

polynomial::polynomial(int k) //copy from integer 
{
    std::vector<std::pair<power, coeff>> poly = {{0, k}};
    this->polyVec = poly;
}

polynomial::polynomial(std::pair<power, coeff> pair)
{
    std::vector<std::pair<power, coeff>> poly = {pair};
    this->polyVec = poly;
}

polynomial::polynomial(std::vector<std::pair<power, coeff>> vec)
{
    this->polyVec = vec;
}



void polynomial::print()
{
    //std::vector<std::pair<power, coeff>> poly = this->canonical_form();
    std::vector<std::pair<power, coeff>> poly = polyVec;
    std::vector<std::pair<power, coeff>>::iterator start = poly.begin();
    std::vector<std::pair<power, coeff>>::iterator end = poly.end();
    
    while(start != end)
    {
        std::cout << (*start).second << "x^" << (*start).first << " ";
        start++;
        if(start != end)
        {
            std::cout << "+ ";
        }
    }

    std::cout << "\n";
}

polynomial &polynomial::operator=(const polynomial &other) //assignmet for object
{
    if (this == &other)
    {
        return *this;
    }
    polyVec = other.polyVec; //vector assignment is a deep copy

    return *this;
}

size_t polynomial::find_degree_of() const{
    
    size_t curDeg = 0;
    if (polyVec.empty()){
        return curDeg;
    }

    for(std::pair<power, coeff>  term : polyVec){//all pairs as term in this are copys
        if(term.first > curDeg && term.second != 0){
            //vector is not ordered so need to iterate through and find highest power with nonzero coeff (maybe ordering could help for speed later)
            curDeg = term.first;
        }
    } 
    //done iterating, return highest degree found, if none found returns 0
    return curDeg;
}

std::vector<std::pair<power, coeff>> polynomial::getPolyVec() const{
    return polyVec;
}

//cannonical form function needs to take the polyVec and:
    //
    //1.combine like terms
    //2. remove 0 coeff pairs other than (0,0)
    //3. order from highest to lowest power
    //4 determine wether to have the (0,0) pair
/*std::vector<std::pair<power, coeff>> mergeSortedChunks(const std::vector<std::vector<std::pair<power, coeff>>> &threadSorts) {
    // Min-Heap for k-way merge
    using HeapElement = std::tuple<power, coeff, size_t, size_t>; // {value, coeff, vector_index, element_index}
    auto compare = [](const HeapElement &a, const HeapElement &b) {
        return std::get<0>(a) < std::get<0>(b); // Max-Heap for descending order
    };
    std::priority_queue<HeapElement, std::vector<HeapElement>, decltype(compare)> heap(compare);

    // Initialize the heap with the first element from each vector
    for (size_t i = 0; i < threadSorts.size(); ++i) {
        if (!threadSorts[i].empty()) {
            heap.emplace(threadSorts[i][0].first, threadSorts[i][0].second, i, 0);
        }
    }

    std::vector<std::pair<power, coeff>> result;

    // Perform the k-way merge
    while (!heap.empty()) {
        auto [value, coeff, vectorIndex, elementIndex] = heap.top();
        heap.pop();
        result.emplace_back(value, coeff);

        // Push the next element from the same vector into the heap
        if (elementIndex + 1 < threadSorts[vectorIndex].size()) {
            heap.emplace(threadSorts[vectorIndex][elementIndex + 1].first,
                         threadSorts[vectorIndex][elementIndex + 1].second,
                         vectorIndex,
                         elementIndex + 1);
        }
    }

    return result;
}*/
void mergeForThreads(const std::vector<std::pair<power, coeff>> &chunk0,const std::vector<std::pair<power, coeff>> &chunk1, std::vector<std::pair<power, coeff>> &threadSortsR){
    //split ordvec into even chunks to sort, then merge
    std::vector<std::pair<power, coeff>> result;
    std::merge(chunk0.begin(), chunk0.end(), chunk1.begin(), chunk1.end(),
               std::back_inserter(result),
               [](const std::pair<power, coeff>& a, const std::pair<power, coeff>& b) {
                   return a.first > b.first;  
               });
    threadSortsR = result;
}


std::vector<std::vector<std::pair<power, coeff>>> parMergeChunks(const std::vector<std::vector<std::pair<power, coeff>>> &threadSorts) {
    int numSortedVec = threadSorts.size();
    int numThreads = (numSortedVec+1) / 2; //if 128 sorted vec, make 64 threads to merge 2 in paralell
    //std::cout << "numthreads par canon: " << numThreads << "\n";

    std::vector<std::vector<std::pair<power, coeff>>> threadSortsR(numThreads); //vector of merged vectors
    std::vector<std::thread> threads; //init threds vec  
    //if(numThreads == 1){
    //        polynomial printPoly3(threadSorts[1].begin(),threadSorts[1].end());
    //        printPoly3.print();
    //}
    for (int i = 0; i < numThreads*2; i=i+2) {
        threads.push_back(std::thread(mergeForThreads, threadSorts[i],threadSorts[i+1], std::ref(threadSortsR[i/2])));
    }
    for (auto &t : threads) {
        t.join();                        
    }//wait for threads to sort their chunks
    return threadSortsR;
}

void sortForThreads(const std::vector<std::pair<power, coeff>> &chunk, std::vector<std::pair<power, coeff>> &threadSorts){
    //split ordvec into even chunks to sort, then merge
    std::vector<std::pair<power, coeff>> chunkSort = chunk;
    std::sort(chunkSort.begin(), chunkSort.end(), [](const std::pair<power, coeff> &a, const std::pair<power, coeff> &b) {
        return a.first > b.first;  // 
    });
    threadSorts = chunkSort;
}
std::vector<std::pair<power, coeff>> vectorCanon(std::vector<std::pair<power, coeff>> &makeCanon) {
 //way too slow!
    //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time the sorting of the divided vecs
    std::vector<std::pair<power, coeff>> canVec;
    
    if (makeCanon.empty()){ //if empty say its 0,0
        makeCanon.push_back(std::make_pair(0, 0));
        return makeCanon;
    }
    

    std::sort(makeCanon.begin(), makeCanon.end(), [](const std::pair<power, coeff> &a, const std::pair<power, coeff> &b) {
        return a.first > b.first;  // 
    });
    //following code is O(n), combines like terms
    std::vector<std::pair<power, coeff>>::iterator ordIt = makeCanon.begin();
    std::vector<std::pair<power, coeff>>::iterator ordItNext = ordIt;
    std::vector<std::pair<power, coeff>>::iterator ordEnd = makeCanon.end();
    int curCoef = 0;
    while(ordIt != ordEnd){
        ordItNext = ordIt+1;
        size_t itPower = ordIt->first;
        if(ordItNext != ordEnd){
            size_t nextItPower = ordItNext->first;
            //every element is accumulated
            curCoef = curCoef + ordIt->second;
            if(itPower != nextItPower){
                if(curCoef != 0){
                    canVec.push_back(std::make_pair(itPower, curCoef));
                    curCoef = 0;//reset coef accumulator for next power
                }
                ordIt++;
            }
            else{
                //ddont need to add any terms yet, not done accumulation, just go to next iterator
                ordIt++;
            }
        }else{
            //orditnext is poining to end, ordit is pointing to last existing term
            //can just accumulte and add
            curCoef = curCoef + ordIt->second;
            if(curCoef != 0){
                canVec.push_back(std::make_pair(itPower, curCoef));
                curCoef = 0;//reset coef accumulator for next power
            }
            ordIt++;
        }
    }

    if (canVec.empty()){ //if empty say its 0,0
        canVec.push_back(std::make_pair(0, 0));
    }
    return canVec;
}
unsigned int uIntPowTwo(unsigned int x) {
    if (x <= 0) return 0;
    x--;  
    x |= x >> 1;  
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;  
    return x + 1;  
}
std::vector<std::pair<power, coeff>> polynomial::canonical_form() const{
    //way too slow!
    //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();//start clock to time the sorting of the divided vecs
    std::vector<std::pair<power, coeff>> ordVec;
    std::vector<std::pair<power, coeff>> canVec;
    
    if (polyVec.empty()){ //if empty say its 0,0
        ordVec.push_back(std::make_pair(0, 0));
        return ordVec;
    }
    ordVec = polyVec;
    
    //thread attempt...
    /*
    unsigned int num_threadsOdd = std::min(static_cast<size_t>(std::thread::hardware_concurrency()), polyVec.size());//128 on my machine
    
    unsigned int num_threads = uIntPowTwo(num_threadsOdd);
    //std::cout << "num threads canon: " << num_threads << "\n";
    long range = polyVec.size()/num_threads;
    //std::cout << "polyVec size canon: " << polyVec.size() << "\n";
    //std::cout << "range canon: " << range << "\n";
    std::vector<std::vector<std::pair<power, coeff>>> threadSorts(num_threads); //vector of smaller sorted vector
    std::vector<std::thread> threads; //init threds vec  


    for (int i = 0; i < num_threads; i++) {
        long start = i * range;               
        long end = (i == num_threads - 1) ? polyVec.size() : (i + 1) * range;
        //std::cout << "i: " << i << "\n"; 
        //std::cout << "start: " << start << "\n"; 
        //std::cout << "end: " << end << "\n";
        std::vector<std::pair<power, coeff>> chunkVec((polyVec.begin() + start), (polyVec.begin() + end)); 
        //if(i == 127){
        //    polynomial printPoly3(chunkVec.begin(),chunkVec.end());
        //    printPoly3.print();
        //}
        threads.push_back(std::thread(sortForThreads, chunkVec, std::ref(threadSorts[i])));
    }

    for (auto &t : threads) {
        t.join();                        
    }//wait for threads to sort their chunks

           // polynomial printPoly3(threadSorts[0].begin(),threadSorts[0].end());
           // printPoly3.print();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock chunkc are all sorted

    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //std::cout << "sorting for divided time: " << duration.count() << " ms\n";




    std::vector<std::vector<std::pair<power, coeff>>> workingOnMerge;
    //begin = std::chrono::steady_clock::now();//start clock to time the merging proces

    workingOnMerge = threadSorts;
    //std::cout << "working on merge size canon: " << workingOnMerge.size() << "\n";
    int workingSize = threadSorts.size(); //always a power of 2
    //std::cout << "working size canon: " << workingSize << "\n";
    while(workingSize != 1){
        workingOnMerge = parMergeChunks(workingOnMerge);

        workingSize = workingOnMerge.size();
        //std::cout << "working size canon: " << workingSize << "\n";
    }



     ordVec = workingOnMerge[0];  // Start with the first sorted chunk

     end = std::chrono::steady_clock::now(); //end clock chunkc are all sorted

     //duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //std::cout << "mergeing time: " << duration.count() << " ms\n";
*/
    std::sort(ordVec.begin(), ordVec.end(), [](const std::pair<power, coeff> &a, const std::pair<power, coeff> &b) {
        return a.first > b.first;  // 
    });
    //following code is O(n), combines like terms
    std::vector<std::pair<power, coeff>>::iterator ordIt = ordVec.begin();
    std::vector<std::pair<power, coeff>>::iterator ordItNext = ordIt;
    std::vector<std::pair<power, coeff>>::iterator ordEnd = ordVec.end();
    int curCoef = 0;
    while(ordIt != ordEnd){
        ordItNext = ordIt+1;
        size_t itPower = ordIt->first;
        if(ordItNext != ordEnd){
            size_t nextItPower = ordItNext->first;
            //every element is accumulated
            curCoef = curCoef + ordIt->second;
            if(itPower != nextItPower){
                if(curCoef != 0){
                    canVec.push_back(std::make_pair(itPower, curCoef));
                    curCoef = 0;//reset coef accumulator for next power
                }
                ordIt++;
            }
            else{
                //ddont need to add any terms yet, not done accumulation, just go to next iterator
                ordIt++;
            }
        }else{
            //orditnext is poining to end, ordit is pointing to last existing term
            //can just accumulte and add
            curCoef = curCoef + ordIt->second;
            if(curCoef != 0){
                canVec.push_back(std::make_pair(itPower, curCoef));
                curCoef = 0;//reset coef accumulator for next power
            }
            ordIt++;
        }
    }

    if (canVec.empty()){ //if empty say its 0,0
        canVec.push_back(std::make_pair(0, 0));
    }
    return canVec;

}

bool polynomial::operator>=(const polynomial &other)
{
    std::vector<std::pair<power, coeff>> v1 = canonical_form();
    std::vector<std::pair<power, coeff>> v2 = other.canonical_form();

    if(v1[0].first >= v2[0].first) //Only check if powers are the same or greater
    {
        return true;
    }

   

    //If same polynomial, return false
    return false;
}

//FREE FUNCTIONS


polynomial operator+(const polynomial &lhs, const polynomial &rhs) {


    //O(n) pretty sure

    std::vector<std::pair<power, coeff>> sumVec = lhs.getPolyVec();
    std::vector<std::pair<power, coeff>> addVec = rhs.getPolyVec();
    sumVec.insert(sumVec.end(), addVec.begin(), addVec.end());//gg

    
    polynomial result(sumVec.begin(), sumVec.end());

    return (result.canonical_form());



    // //Add in parallel
    // std::vector<std::pair<power, coeff>> leftVec = lhs.getPolyVec();
    // std::vector<std::pair<power, coeff>> rightVec = rhs.getPolyVec();
    // int maxDegree = lhs.find_degree_of() >= rhs.find_degree_of() ? lhs.find_degree_of() : rhs.find_degree_of();
    // bool leftBigger = lhs.find_degree_of() >= rhs.find_degree_of();

    // std::vector<std::pair<power, coeff>> result;
    // result.reserve(maxDegree);

    // int numThreads = std::thread::hardware_concurrency();
    // int numChunks = 1;
    // if(leftBigger)
    // {
    //     if(numThreads < rhs.find_degree_of() + 1)
    //     {
    //         numChunks = (rhs.find_degree_of() + 1 + numThreads - 1) / numThreads;
    //     }
    // }
    // else
    // {
    //     if(numThreads < lhs.find_degree_of() + 1)
    //     {
    //         numChunks = (lhs.find_degree_of() + 1 + numThreads - 1) / numThreads;
    //     }
    // }
    

    // for(int i = 0; i < numChunks; i++)
    // {
    //     int start = numThreads * i;
    //     int end = ((start + numThreads) > numChunks) ? numChunks : (start + numThreads);

    //     //CALL ADDITION ON EACH CHUNK
    //     if(leftBigger)
    //     {
    //         addChunk(rightVec, leftVec, start, end);
    //     }
    //     else
    //     {
    //         addChunk(leftVec, rightVec, start, end);
    //     }
        
    // }

    // // Wait for all threads to complete
    // for (auto& t : threads) {
    //     t.join();
    // }

    // polynomial sum(result);
    // return sum;

}
int binarySearchPower(const std::vector<std::pair<power, coeff>>& arr, int target, bool* found) {
    //Assumes it is in canonical form and thus descending order
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2; // Find middle index

        // Check if the target is at the middle
        if (arr[mid].first == target) {
            *found = true;
            return mid; // Target found
        }
        // If target is greater, search in the left half
        else if (arr[mid].first < target) {
            right = mid - 1;
        }
        // If target is smaller, search in the right half
        else {
            left = mid + 1;
        }
    }
    *found = false;
    return left; // Target not found
}
std::pair<power, coeff> multiply2terms(std::pair<power, coeff> t1, std::pair<power, coeff> t2)
{
    return {t1.first + t2.first, t1.second * t2.second};
}

void sectionMultiplier(const std::vector<std::pair<power, coeff>> &lhs, const std::vector<std::pair<power, coeff>> &rhs, std::vector<std::pair<power, coeff>> &threadAnswers){
    //std::vector<std::pair<power, coeff>> passPoly;
    //int i=0;
    bool termFound; 
    int termIdx;
    for(std::pair<power, coeff> pairL : lhs)
    {
        //i=0;
        for(std::pair<power, coeff> pairR : rhs)
        {
            //if (i<1006){
            //starts empty so should be able to be continoually sorted?
            termFound = false;
            termIdx = binarySearchPower(threadAnswers, multiply2terms(pairL, pairR).first, &termFound);
            if(termFound){
                threadAnswers[termIdx].second = threadAnswers[termIdx].second + multiply2terms(pairL, pairR).second;
            }else{
                threadAnswers.insert((threadAnswers.begin() + termIdx),multiply2terms(pairL, pairR));
            }
            //i++;
            //}

        }
        //std::cout << "i: " << i << "\n"; 
    }
    std::cout << "threadawnserssize: " << threadAnswers.size() << "\n"; 

}

polynomial operator*(const std::pair<power, coeff>& pair, const polynomial& poly) {
    // This will invoke the constructor that converts the pair to a polynomial
    return polynomial(pair) * poly;
}


polynomial operator*(const polynomial &lhs, const polynomial &rhs) 
{
    // size_t degree = lhs.find_degree_of() + rhs.find_degree_of();
    std::vector<std::pair<power, coeff>> lhsPoly = lhs.canonical_form();
    std::vector<std::pair<power, coeff>> rhsPoly = rhs.canonical_form();

    //Multiply each term with each other term in the other polynomial and sum everything
    //this is the O(n^2) method, may want to use O(nlogn) FFT method... only if necessary it is pretty wierd

    //with O(n^2) method, threads can be used to devide lhs by an amount of threads here

    //vector of all the vecotrs determined by the threads


    bool threadLeft = lhsPoly.size() >= rhsPoly.size();
    //std::cout << "bool threadLeft mul1: " << threadLeft << "\n";

    long bigPolySize = (threadLeft)? lhsPoly.size() : rhsPoly.size();

    unsigned int num_threads = std::min(static_cast<long>(std::thread::hardware_concurrency()), bigPolySize);//128 on my machine
    //std::cout << "num_threads mul1: " << num_threads << "\n";

    long range = bigPolySize/num_threads;
    //std::cout << "range mul1: " << range << "\n";                     
    std::vector<std::vector<std::pair<power, coeff>>> threadAnswers(num_threads); 
    std::vector<std::thread> threads; //init threds vec    
    std::vector<std::pair<power, coeff>> othVec = (threadLeft)? rhsPoly : lhsPoly;

    //polynomial printPoly(othVec.begin(),othVec.end());
    //printPoly.print();
    std::vector<std::pair<power, coeff>> divVec;
    for (int i = 0; i < num_threads; i++) {
        long start = i * range;               
        long end = (i == num_threads - 1) ? bigPolySize : (i + 1) * range;
        //std::cout << "i: " << i << "\n"; 
        //std::cout << "start: " << start << "\n"; 
        //std::cout << "end: " << end << "\n";  
        if (threadLeft) {
            divVec.assign(lhsPoly.begin() + start, lhsPoly.begin() + end);  // Slice lhsPoly
        } else {
            divVec.assign(rhsPoly.begin() + start, rhsPoly.begin() + end);  // Slice rhsPoly
        }
        //polynomial printPoly3(divVec.begin(),divVec.end());
        //printPoly3.print();
        threads.push_back(std::thread(sectionMultiplier, divVec, othVec, std::ref(threadAnswers[i])));
        divVec.clear();
        
    }


    for (auto &t : threads) {
        t.join();                        
    }//wait for thread answers to be full

    std::vector<std::pair<power, coeff>> totVec; //will hold the final product
    /*for(std::vector<std::pair<power, coeff>> &ans : threadAnswers){
        totVec.insert(totVec.end(), ans.begin(), ans.end());
        polynomial result(totVec.begin(), totVec.end());
        totVec = result.canonical_form();
        //break; //MAJOR MEMORY ALLOCKER
    }*/
    for(std::vector<std::pair<power, coeff>> &ans : threadAnswers){
        totVec.insert(totVec.end(), ans.begin(), ans.end());
        totVec = vectorCanon(totVec);
        //break; //MAJOR MEMORY ALLOCKER
    }
    polynomial result(totVec.begin(), totVec.end());
    return result;

}


std::pair<power, coeff> operator/(const std::pair<power, coeff>& numer, const std::pair<power, coeff>& div)
{
    std::pair<power, coeff> answer = {numer.first - div.first, numer.second / div.second};
    return answer;
}

class Base
{
    public:

    virtual int getIter() = 0;
    virtual std::pair<power, coeff> getPair() = 0;
};

class Del : public Base
{
    public:
    int iter;

    int getIter()
    {
        return iter;
    }

    std::pair<power, coeff> getPair()
    {
        return {-1,-1};
    }

    Del(int i) : iter(i){}
};

class Insert : public Base
{
    public:
    std::pair<int, std::pair<power, coeff>> pair;

    int getIter()
    {
        return pair.first;
    }

    std::pair<power, coeff> getPair()
    {
        return pair.second;
    }

    Insert(std::pair<int, std::pair<power, coeff>> pair) : pair(pair){}
};

std::mutex changesMutex;
std::mutex insertsMutex;
std::mutex deletesMutex;

//moved binarySearchPower to earlier declaration

void findAndUpdate(const std::vector<std::pair<power, coeff>> small, const std::vector<std::pair<power, coeff>> big, int start, int end,
              std::vector<std::pair<int, int>>& changes, std::vector<std::pair<int, std::pair<power, coeff>>>& inserts, std::vector<int>& deletes, int i)
{

    bool found = true;
    int index = binarySearchPower(big, small[i].first, &found); //look for index of portion with same power

    std::cout << "Power: " << small[i].first << '\n';
    //If not there, insert into vector at right place
    if(!found)
    {
        std::lock_guard<std::mutex> lock(insertsMutex);
        inserts.push_back({index, {small[i].first, -small[i].second}});
    }

    //If there
    else
    {
        //Subtract coefficients
        int difference = big[index].second - small[i].second;

        if(difference == 0) //account for removal to keep in canonical form
        {
            std::lock_guard<std::mutex> lock(deletesMutex);
            deletes.push_back(index); //Set to be removed
        }

        else
        {
            //Change the coefficient
            std::lock_guard<std::mutex> lock(changesMutex);
            changes.push_back({index, difference});
        }
    }
    
}
//Subtract by finding common power first
void subChunk(const std::vector<std::pair<power, coeff>>& small, const std::vector<std::pair<power, coeff>>& big, int start, int end,
              std::vector<std::pair<int, int>>& changes, std::vector<std::pair<int, std::pair<power, coeff>>>& inserts, std::vector<int>& deletes)
{
    std::vector<std::thread> threads;

    std::vector<std::pair<int, int>> changesDeref = changes;
    std::vector<std::pair<int, std::pair<power, coeff>>> insertsDeref = inserts;
    std::vector<int> deletesDeref = deletes;

    for(int i = start; i < end; i++)
    {
        

        threads.push_back(std::thread(findAndUpdate, small, big, start, end, std::ref(changesDeref), std::ref(insertsDeref), std::ref(deletesDeref), i));
    }

    for(int i = start; i < end; i++)
    {
        threads[i].join();
    }

    changes = changesDeref;
    inserts = insertsDeref;
    deletes = deletesDeref;
}

bool compareInsertsAndDels(std::shared_ptr<Base> a, std::shared_ptr<Base> b)
{
    return a->getIter() > b->getIter(); //sort descending order then wont need to worry about accounting for previous inserts
}

polynomial changeBig(const polynomial &big, std::vector<std::pair<int, int>>& changes, std::vector<std::pair<int, std::pair<power, coeff>>>& inserts, std::vector<int>& deletes)
{
    std::vector<std::pair<power, coeff>> result = big.getPolyVec();

    std::vector<std::shared_ptr<Base>> insertAndDel;

    for(auto insert : inserts)
    {
        insertAndDel.push_back(std::make_shared<Insert>(insert));
    }

    for(auto del : deletes)
    {
        insertAndDel.push_back(std::make_shared<Del>(del));
    }
    

    std::sort(insertAndDel.begin(), insertAndDel.end(), compareInsertsAndDels);

    for(auto pair : changes)
    {
        result[pair.first].second = pair.second; //Change coefficients
    }

    for(auto item : insertAndDel) //insert and del from the back
    {
        if(typeid(*item) == typeid(Del)) //delete
        {
            result.erase(result.begin() + item->getIter());
        }

        else if(typeid(*item) == typeid(Insert)) //insert
        {
            result.insert(result.begin() + item->getIter(), item->getPair());
        }
    }

    return polynomial(result);

}

//Subtracting only when both is already in canonical form. big - small
polynomial specialSub(const polynomial &big, const polynomial &small)
{
    auto smallVec = small.getPolyVec();
    int maxDegree = big.find_degree_of();

    //std::vector<std::pair<power, coeff>> result = big.getPolyVec();

    int numThreads = std::thread::hardware_concurrency();
    int numChunks = 1;
    if(numThreads < smallVec.size() + 1)
    {
        numChunks = (smallVec.size() + 1 + numThreads - 1) / numThreads;
    }
    
    //Position to change, new integer to change coefficient to
    std::vector<std::pair<int, int>> changes;

    //Position to put it in (before the other inserts), new polynomial term
    std::vector<std::pair<int, std::pair<power, coeff>>> inserts;

    //Position to delete in (before the inserts and the other deletes)
    std::vector<int> deletes;

    for(int i = 0; i < numChunks; i++)
    {
        int start = numThreads * i;
        int end = ((start + numThreads) > smallVec.size()) ? smallVec.size() : (start + numThreads);
        
        //std::cout << "Start and end BIG ONE: " << start << " " << end << '\n';
        std::cout << "NUM THREADS: " << numThreads << "\n";
        //CALL SUBTRACTION ON EACH CHUNK
        subChunk(small.getPolyVec(), big.getPolyVec(), start, end, changes, inserts, deletes);
        
    }

    polynomial sum = changeBig(big, changes, inserts, deletes);

    return sum;

}

polynomial operator%(const polynomial &numer, const polynomial &denom)
{
    //std::vector<std::pair<power, coeff>> v1 = numer.canonical_form();
    //std::vector<std::pair<power, coeff>> v2 = denom.canonical_form();
    std::pair<power, coeff> leading = denom.canonical_form()[0];
    polynomial canonDenom = denom.canonical_form();


    polynomial remainder = numer.canonical_form();

    int i = 0;

    while(remainder >= denom)
    {
        //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        // Divide the first term of the remainder by the first term of the denominator
        std::pair<power, coeff> divide = remainder.getPolyVec()[0] / leading;

        if(divide.second == 0)
        {
            std::cout << "GOT HERE" << '\n';
            break;
        }

        // Multiply the result by the denominator
        polynomial curr = canonDenom * divide;

        // Subtract the result from the remainder
        //remainder = remainder + (-1 * curr);
        remainder = specialSub(remainder, curr);

        // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); //end clock
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        // std::cout << "timing inside modulo..." << '\n';
        // std::cout << duration.count() << '\n';
        i++;

    }

    std::cout << "Went through modulus " << i << " times\n";

    return remainder;
}


// polynomial operator+(const polynomial &lhs, int rhs) {

//     std::vector<std::pair<power, coeff>> sumVec = lhs.getPolyVec();
//     //check if 0 power term exists, if it does update it
//     bool found = false;
//     for(std::pair<power, coeff>  &termS : sumVec){
//         if(0 == termS.first){ //found a 0 power term
//             termS.second += rhs;
//             found = true;
//             break;     
//         }
//     }  

//     if(!found){
//         sumVec.push_back(std::make_pair(0,rhs)); //did not find a like term, add it as new pair
//     }
//     polynomial result(sumVec.begin(), sumVec.end());

//     return result;
// }

// polynomial operator+(int lhs, const polynomial &rhs) {

//     std::vector<std::pair<power, coeff>> sumVec = rhs.getPolyVec();
//     //check if 0 power term exists, if it does update it
//     bool found = false;
//     for(std::pair<power, coeff>  &termS : sumVec){
//         if(0 == termS.first){ //found a 0 power term
//             termS.second += lhs;
//             found = true;
//             break;     
//         }
//     }  

//     if(!found){
//         sumVec.push_back(std::make_pair(0,lhs)); //did not find a like term, add it as new pair
//     }
//     polynomial result(sumVec.begin(), sumVec.end());

//     return result;
// } test

