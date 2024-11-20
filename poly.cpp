#include "poly.h"
#include <iostream>

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

void polynomial::print()
{
    std::vector<std::pair<power, coeff>> poly = this->canonical_form();
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

std::vector<std::pair<power, coeff>> polynomial::canonical_form() const{


    std::vector<std::pair<power, coeff>> ordVec;
    std::vector<std::pair<power, coeff>> canVec;
    
    if (polyVec.empty()){ //if empty say its 0,0
        ordVec.push_back(std::make_pair(0, 0));
        return ordVec;
    }

 
    for(std::pair<power, coeff> term : polyVec){
        bool found = false; //term  like power not found in ordVec yet
        for(std::pair<power, coeff>  &termO : ordVec){
            if(term.first == termO.first){ //found a like term
                termO.second += term.second;
                found = true;
                break; //only want to add once in case of multiple of same term pairs in vec     
            }
        }  

        if(!found){
            ordVec.push_back(term); //did not find a like term, add it as new pair
        }
    }
    //canVec should have all like terms combined, unordered by power
    //order by power now
    std::sort(ordVec.begin(), ordVec.end(), [](const std::pair<power, coeff> &a, const std::pair<power, coeff> &b) {
        return a.first > b.first;  // sort by power, lergest at index 0
    });
    //now get rid of all 0 coeff terms
    //just only add elem to canVec with no 0 coeff

    for (std::pair<power, coeff> termNZ : ordVec){
        if(termNZ.second != 0){
           canVec.push_back(termNZ); //found non zero coeff term, add it
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


    //should i copy one and add the other to it
    //or start with nothing and add both to it
    //which will be faster with threads? i think second could, but first is simpler
    //ill go with the first option for now


    std::vector<std::pair<power, coeff>> sumVec = lhs.getPolyVec();
    
    //if like powers, combine coefficients and update vector, if new power add to vector
    //combineing coeffs could be done in canonical form funct but it might be good to do here

    //vector is not ordered so need to iterate through and find (maybe sort could help for speed later)
    for(std::pair<power, coeff>  termR : rhs.getPolyVec()){//all pairs as term in this are copys            
    bool found = false; //termR  like power not found in sumVec yet
        for(std::pair<power, coeff>  &termS : sumVec){
            if(termR.first == termS.first){ //found a like term
                termS.second += termR.second;
                found = true;
                break; //only want to add once in case of multiple of same term pairs in vec     
            }
        }  

        if(!found){
            sumVec.push_back(termR); //did not find a like term, add it as new pair
        }
    }
    
    polynomial result(sumVec.begin(), sumVec.end());

    return result;

}

std::pair<power, coeff> multiply2terms(std::pair<power, coeff> t1, std::pair<power, coeff> t2)
{
    return {t1.first + t2.first, t1.second * t2.second};
}

polynomial operator*(const polynomial &lhs, const polynomial &rhs) 
{
    // size_t degree = lhs.find_degree_of() + rhs.find_degree_of();
    polynomial answer;
    std::vector<std::pair<power, coeff>> lhsPoly = lhs.getPolyVec();
    std::vector<std::pair<power, coeff>> rhsPoly = rhs.getPolyVec();

    //TODO: make this multithreaded
    //Multiply each term with each other term in the other polynomial and sum everything
    for(std::pair<power, coeff> pairL : lhsPoly)
    {
        for(std::pair<power, coeff> pairR : rhsPoly)
        {
            polynomial product = multiply2terms(pairL, pairR);
            answer = answer + product; //TODO: check if this causes errors IDK if default constructor works here
        }

    }

    return answer;

}


std::pair<power, coeff> operator/(std::pair<power, coeff>& numer, std::pair<power, coeff>& div)
{
    std::pair<power, coeff> answer = {numer.first - div.first, numer.second / div.second};
    return answer;
}

polynomial operator%(const polynomial &numer, const polynomial &denom)
{
    std::vector<std::pair<power, coeff>> v1 = numer.canonical_form();
    std::vector<std::pair<power, coeff>> v2 = denom.canonical_form();

    polynomial remainder = numer;

    while(remainder >= denom)
    {
        // Divide the first term of the remainder by the first term of the denominator
        std::pair<power, coeff> divide = remainder.canonical_form()[0] / v2[0];

        // Multiply the result by the denominator
        polynomial curr = divide * denom;

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
// }