#include "poly.h"

polynomial::polynomial(){
    polyVec.push_back(std::make_pair(0, 0)); //construct with 0 coeff 0 power only

}

template <typename Iter>
polynomial::polynomial(Iter begin, Iter end)
{
    polyVec.insert(polyVec.end(), begin, end);
}

polynomial::polynomial(const polynomial &other):polyVec(other.polyVec) 
{
}

polynomial &polynomial::operator=(const polynomial &other)
{
    if (this == &other)
    {
        return *this;
    }
    polyVec = other.polyVec; //vector assignment is a deep copy

    return *this;
}