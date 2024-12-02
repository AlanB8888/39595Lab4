#include "poly.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include <queue>


polynomial::polynomial(){
    polyVec.push_back(std::make_pair(0, 0)); //construct with 0 coeff 0 power only

}

template <typename Iter>
polynomial::polynomial(Iter begin, Iter end) //constructor from iterators
{
    polyVec.insert(polyVec.end(), begin, end);
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

    
    //thread attempt...
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


        //polynomial printPoly8(ordVec.begin(),ordVec.end());
        //printPoly8.print();
    /*for (size_t i = 1; i < workingOnMerge.size(); ++i) {
        std::vector<std::pair<power, coeff>> temp;
        // Merge the current merged vector with the next chunk
        std::merge(ordVec.begin(), ordVec.end(),
                   workingOnMerge[i].begin(), workingOnMerge[i].end(),
                   std::back_inserter(temp));
        ordVec = temp;  // Update mergedVec to the newly merged result
    }*/

     end = std::chrono::steady_clock::now(); //end clock chunkc are all sorted

     //duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    //std::cout << "mergeing time: " << duration.count() << " ms\n";

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


    //just add them, by joinng vecs, canonize/simplify somewhere else
    //O(n) pretty sure

    std::vector<std::pair<power, coeff>> sumVec = lhs.getPolyVec();
    std::vector<std::pair<power, coeff>> addVec = rhs.getPolyVec();
    sumVec.insert(sumVec.end(), addVec.begin(), addVec.end());//gg

    
    polynomial result(sumVec.begin(), sumVec.end());

    return (result.canonical_form());

}

std::pair<power, coeff> multiply2terms(std::pair<power, coeff> t1, std::pair<power, coeff> t2)
{
    return {t1.first + t2.first, t1.second * t2.second};
}

void sectionMultiplier(const std::vector<std::pair<power, coeff>> &lhs, const std::vector<std::pair<power, coeff>> &rhs, std::vector<std::pair<power, coeff>> &threadAnswers){
    std::vector<std::pair<power, coeff>> answer;
    for(std::pair<power, coeff> pairL : lhs)
    {
        for(std::pair<power, coeff> pairR : rhs)
        {

            std::pair<power, coeff> product = multiply2terms(pairL, pairR);
            answer.push_back(product);
        }

    }
    threadAnswers = answer;
}

polynomial operator*(const std::pair<power, coeff>& pair, const polynomial& poly) {
    // This will invoke the constructor that converts the pair to a polynomial
    return polynomial(pair) * poly;
}


polynomial operator*(const polynomial &lhs, const polynomial &rhs) 
{
    // size_t degree = lhs.find_degree_of() + rhs.find_degree_of();
    std::vector<std::pair<power, coeff>> lhsPoly = lhs.getPolyVec();
    std::vector<std::pair<power, coeff>> rhsPoly = rhs.getPolyVec();

    //TODO: make this multithreaded
    /*polynomial printPoly1(lhsPoly.begin(),lhsPoly.end());
    printPoly1.print();
    polynomial printPoly2(rhsPoly.begin(),rhsPoly.end());
    printPoly2.print();*/

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

    for (int i = 0; i < num_threads; i++) {
        long start = i * range;               
        long end = (i == num_threads - 1) ? bigPolySize : (i + 1) * range;
        //std::cout << "i: " << i << "\n"; 
        //std::cout << "start: " << start << "\n"; 
        //std::cout << "end: " << end << "\n";  
        std::vector<std::pair<power, coeff>> divVec;
        if (threadLeft) {
            divVec.assign(lhsPoly.begin() + start, lhsPoly.begin() + end);  // Slice lhsPoly
        } else {
            divVec.assign(rhsPoly.begin() + start, rhsPoly.begin() + end);  // Slice rhsPoly
        }
        //polynomial printPoly3(divVec.begin(),divVec.end());
        //printPoly3.print();
        threads.push_back(std::thread(sectionMultiplier, divVec, othVec, std::ref(threadAnswers[i])));
    }


    for (auto &t : threads) {
        t.join();                        
    }//wait for thread answers to be full

    std::vector<std::pair<power, coeff>> totVec; //will hold the final product
    for(std::vector<std::pair<power, coeff>> ans : threadAnswers){
        totVec.insert(totVec.end(), ans.begin(), ans.end());
    }

    polynomial result(totVec.begin(), totVec.end());
    return result;

}


std::pair<power, coeff> operator/(const std::pair<power, coeff>& numer, const std::pair<power, coeff>& div)
{
    std::pair<power, coeff> answer = {numer.first - div.first, numer.second / div.second};
    return answer;
}

polynomial operator%(const polynomial &numer, const polynomial &denom)
{
    //std::vector<std::pair<power, coeff>> v1 = numer.canonical_form();
    //std::vector<std::pair<power, coeff>> v2 = denom.canonical_form();
    std::pair<power, coeff> leading = denom.canonical_form()[0];


    polynomial remainder = numer.canonical_form();


    while(remainder >= denom)
    {
        // Divide the first term of the remainder by the first term of the denominator
        std::pair<power, coeff> divide = remainder.canonical_form()[0] / leading;

        // if(divide.second == 0)
        // {
        //     break;
        // }

        // Multiply the result by the denominator
        polynomial curr = denom * divide;

        // Subtract the result from the remainder
        remainder = remainder + (-1 * curr);

    }

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

