#if !defined(_VECTOR_H)
#define _VECTOR_H

#include <iostream>
#include <new>

using namespace std;

class Matrix; 
class Vector; 

Vector operator*(Matrix&, Vector&);

class Vector{
    private:
    	double* elem; // elem points to an array of sz doubles
	int     sz;
    public:
   	Vector(int s); // constructor: acquire resources
   	Vector()       // constructor: acquire resources
        {
            sz = 0; elem = NULL; 
        }
        Vector(const Vector&); // copy constructor. 
        Vector& operator=(const Vector&); // assignment: cleanup and copy

        double& operator[](int);      // retrieve elem[]
        int size() const;             //const indicates that this function does not modify data

        friend ostream& operator<<(std::ostream &output, const Vector& P ){
            int i; 
            output << "V=(";
            for(i = 0;  i < P.size(); i++)
                output<<P.elem[i]<<", ";
            output <<")"<<endl;
            return output;
        }
       
        Vector operator+(const Vector&);
        /* dot product of two vectors*/
        double operator*(const Vector& rhs)
        {
            int i;
            double ans = 0.0; 
            if(this->sz != rhs.sz)
            {
                cerr<<"ERROR, dot product of two vectors, length do not match"<<endl;
                return 0.0; 
            }
            for(i = 0; i < this->sz; i++)
                ans += this->elem[i]*rhs.elem[i]; 
            return ans; 
        }
        /*scalar multiplication of the vector*/
        Vector operator*(const double a)
        { 
            Vector re(this->sz); 
            for(int i = 0; i < this->sz; i++)
                 re.elem[i] = this->elem[i]*a; 
            return re; 
        }
  
	~Vector(){
            if(NULL != elem) delete[] elem;
            std::cout<<"vector destructor"<<std::endl; 
        }     //destructor : release resources

	friend Vector operator*(Matrix& A, Vector& v);

};

#endif /* !defined(_VECTOR_H) */

