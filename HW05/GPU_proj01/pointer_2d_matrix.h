#if !defined(_POINTER_2D_MATRIX_H)
#define _POINTER_2D_MATRIX_H

#include <iostream>
#include <new>
#include <cstddef>
#include <fstream>
#include "Vector.h"

using namespace std;

#define  off_set            0
#define  Mesh_Size          10
#define  FILTER_SIZE        3

#define  A(i,j)             A[(i)+off_set][(j)+off_set]
#define  Mat(i,j,A)        (A)[(i)+off_set][(j)+off_set]

class Matrix{
    private:
    double  **pptr; 
    int     rows, cols; 
    
    public:
    Matrix(int _rows, int _cols)
    {
        rows = _rows;
        cols = _cols;

        pptr = new double*[rows+2*off_set];
        pptr[0] = new double[(rows+2*off_set)*(cols+2*off_set)];  

        for(int i = 1; i < rows+2*off_set; i++)
            // pptr[i] = new double[cols+2*off_set];
            pptr[i] = pptr[i-1]+ (cols+2*off_set);
    }

    Matrix()
    {
        rows = cols = 0;
        pptr = nullptr;  // use -std=c++11 option in compilation
    }

    Matrix(const Matrix&);
    Matrix& operator=(const Matrix&);
    void resize(int n_rows, int n_cols);

    ~Matrix()
    {
        int   i;
        cout<<"Entered matrix destructor\n"; 
        if(nullptr != pptr)
        {
            // for(i = 0; i < rows+2*off_set; i++)
            //     delete[] pptr[i]; 
            cout<<"matrix destructor, before delete\n"; 
            delete[] pptr[0];
            delete[] pptr; 
        }
        rows = cols = 0;
    }

    /* Note: operator[] always takes exactly one parameter,
    * but operator() can take any number of parameters
    * (in the case of a rectangular matrix, two parameters are needed). */
    double& operator()(int i, int j)
    {
        return pptr[(i)+off_set][(j)+off_set]; 
    }

    friend ostream& operator<<(std::ostream &output, Matrix &mat)
    {
        int i, j;
        for(i = 0; i < mat.rows; i++)
        {
            for(j = 0; j < mat.cols; j++)
                output<<mat.pptr[(i)+off_set][(j)+off_set]<<" "<<endl;
        }
        return output;
    }

    // Cumulative addition of this matrix and another
    Matrix& operator+=(const Matrix& rhs)
    {
        int rows = rhs.get_rows();
        int cols = rhs.get_cols();

        for (int i=0; i<rows; i++) {
          for (int j=0; j<cols; j++) {
            pptr[i+off_set][j+off_set] += rhs.pptr[i+off_set][j+off_set];
          }
        }
        return *this;
    }

    Matrix& operator+(const Matrix& rhs);  // Addition of two matrices

    friend Vector operator*(Matrix&, Vector&);
    unsigned get_rows() const
    {
        return rows;
    }
    unsigned get_cols() const
    {
        return cols;
    }

}; 

double&  mat_access(int, int,double**);
void     function_using_matrix(Matrix& rhs);

#endif 
