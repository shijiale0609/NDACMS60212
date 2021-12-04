#include <iostream>
#include <new>
#include <cstddef>
#include <fstream>
#include "rw_jpg.h"
#include "pointer_2d_matrix.h"
//argv[0] is name of this program
//argv[1] is input .jpg image
//eg:  ./a.out smallimage.jpg

using namespace std;

double& mat_access(int i, int j,double  **a)
{
    return a[i+off_set][j+off_set];
}



void function_using_matrix(Matrix& rhs)
{
    int i, j; 
    for(i = 0; i < Mesh_Size; i++)
    {
        for(j = 0; j < Mesh_Size; j++)
            rhs(i, j) = rhs(i,j) + 2.0; 
    }
}

//**OVERLOADING * OPERATOR**//

Vector operator*(Matrix& A, Vector& v)
{
                int rows=A.get_rows();
                int cols=A.get_cols();
//		printf("1-1=%d, 1-2=%d, 2-1=%d, 2-2=%d",matTest(0,0),matTest(0,1),matTest(1,0),matTest(1,1)); 
                if(v.size()==rows)
                {
		    Vector newVec(cols);

                    for(int i=0;i<cols;i++)
                    {
                        double count=0;
                        for(int j=0;j<rows;j++)
                        {
                            count=count + A.pptr[(i)+off_set][(j)+off_set]*v[j];    
//			    count+=A[(i)+off_set][(j)+off_set]*v[j];
                        }
                        newVec[i]=count;
                    }
                    return newVec;
                }
                else
                {
                    printf("Matrix and Vector dimensions are not compatible for multiplication \n");
                }
                /*return a vector with no storage*/
		Vector newVec;
                return newVec;
}

//**MATRIX COPY CONSTRUCTOR**//

Matrix::Matrix(const Matrix& A2)
{
                cout<<"Entered matrix copy constructor\n";
                cols=A2.get_cols();
                rows=A2.get_rows();

                if(A2.pptr != nullptr){
                    pptr= new double* [rows+2*off_set]; //creates a new array of pointers with length corresponding to the number of rows in the input Matrix plus the offset
                    pptr[0] = new double[(rows+2*off_set)*(cols+2*off_set)];
                    for(int i=1;i<rows+2*off_set;i++)
                    {
                        // pptr[i]=new double[cols+2*off_set];//adds and extra layer to the array of pointers, essantially creating a matrix of pointers
                        pptr[i]=pptr[i-1] + (cols+2*off_set);//adds and extra layer to the array of pointers, essantially creating a matrix of pointers
                    }
                }
                else{
                    rows = cols = 0; 
                    pptr = nullptr;
                    return;
                }

                for(int i=0;i<rows+2*off_set;i++)
                {
                        for(int j=0;j<cols+2*off_set;j++)
                        {
                                pptr[i][j]=A2.pptr[i][j];//creates the new matrix containing identical values to those of the input matrix A2
                        }
                }
}

//**OVERLOADING = OPERATOR - "copy and cleanup"**//

Matrix& Matrix:: operator=(const Matrix &rhs)
{
	rows=rhs.get_rows();
	cols=rhs.get_cols();
//deallocate memory if pptr has already been assigned
	if(pptr !=nullptr)
	{
		// for(int i=0;i<rows;i++)
		// delete[] pptr[i];
		delete[] pptr[0];
		delete[] pptr;
	}
//allocate memory for rows of double pointer
	pptr= new double* [rows+2*off_set]; 
        pptr[0] = new double[(rows+2*off_set)*(cols+2*off_set)];

//allocates memory for the columns of pointer
	for(int i=1;i<cols;i++)
	    // pptr[i]=new double[cols+2*off_set];
	    pptr[i]=pptr[i-1] +(cols+2*off_set);


//assigns matrix values to the value on the right side of the equals sign 
	for(int i=0;i<rows+2*off_set;i++)
        {	
        	for(int j=0;j<cols+2*off_set;j++)
                {
                	pptr[i][j]=rhs.pptr[i][j];
                }
        }
	
        return *this;
}

//**OVERLOADING + OPERATOR**//

Matrix&  Matrix:: operator+(const Matrix &rhs)
{
	

	int rows=rhs.get_rows();
        int cols=rhs.get_cols();

	if((this->rows!=rows)||(this->cols!=cols))
	std::cout<<"The two added matrices have incompatible dimensions"<<endl;
	else
	{
		for(int i=0;i<rows;i++)
		{
			for(int j=0;j<cols;j++)
			{
			this->pptr[i+off_set][j+off_set]=this->pptr[i+off_set][j+off_set]
                                      +rhs.pptr[i+off_set][j+off_set]; 
			}
		}
	}
	return *this;
}
	
//**RESIZING FUNCTION**//

void Matrix::resize(int rows2, int cols2)
{
//create a new pointer array with its number of rows specified by the input
       
//make sure the inputs make sense
	if ((rows2<=0)||(cols2<=0))
	{ 
		std::cout<<"Matrix::resize(). Invalid matrix dimensions"<<endl;
                exit(-1);
	} 
	else
	{	
	        double **pptr2 = new double* [rows2+2*off_set];
                pptr2[0] = new double[(rows2+2*off_set)*(cols2+2*off_set)];
		for(int i=1;i<rows2+2*off_set;i++)
		{
			pptr2[i]= pptr2[i-1] +(cols2+2*off_set);
		}
//fill in values of matrix with new dimensions
		for(int i=0;i<rows+2*off_set;i++)
		{	for(int j=0;j<cols+2*off_set;j++)
				pptr2[i][j]=pptr[i][j];
		}		
//free up unused memory
	        delete[] pptr[0];
	        delete[] pptr;
//reassign values of original pointer, rows and columns to the new values
		pptr=pptr2;rows=rows2;cols=cols2;	
	}
}

