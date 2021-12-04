// Vector.cpp, here we define interfaces to the data

#include "Vector.h" 
#include <stdexcept> 
#include <iostream>

Vector::Vector(int s)
   :elem{new double[s]},sz{s} // constructor: acquire resources
{
    for(int I = 0; I < s; I++)   elem[I] = 0;
}

double& Vector::operator[](int i)
{
    if(i < 0 || i>= size()) throw std::out_of_range{"vector::operator[]"};
    return elem[i];
}

int Vector::size() const
{
     return sz; 
}

Vector& Vector::operator=(const Vector&  a)
{
    if(this != &a && NULL != elem)
       delete [] elem;
    elem = new double [a.size()];
    sz = a.size();
    for(int I = 0; I < sz; I++)
        elem[I] = a.elem[I] ;

    std::cout<<"call Vector operator="<<std::endl;
    return *this;
}

Vector::Vector(const Vector&  a)
{
    elem = new double [a.size()];
    sz = a.size();
    for(int I = 0; I < sz; I++)
        elem[I] = a.elem[I] ;

    std::cout<<"call Vector copy constructor"<<std::endl;
}

Vector Vector::operator+(const Vector&  a)
{
    int sz; 
    Vector re(a.size());
    sz = a.size();
    for(int I = 0; I < sz; I++)
        re.elem[I] = this->elem[I] + a.elem[I] ;

    return re;
}

