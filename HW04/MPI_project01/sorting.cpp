#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <bits/stdc++.h>
#include <set>


#include "mpi.h" // needed for using mpi functions

/*
 * The following 4 macros can be used to compute 
 * the subset of the array data  owned by a process whose rank is "id".
 * Given an integer array, let's call it A[n].  
 * BLOCK_LOW: returns the beginning index of the element in A, which is to be computed by process "id".
 * BLOCK_HIGH: returns the end index of the element in A, which is to be computed by process "id".
 */
#define  BLOCK_LOW(id, p, n)     ((id)*(n)/(p)) //id: rank of process; 
                                                //p: total number of processes
                                                //n: number of elements in the dataset 
#define  BLOCK_HIGH(id, p, n)    (BLOCK_LOW((id)+1,p,n)-1)
#define  BLOCK_SIZE(id, p, n)    (BLOCK_HIGH(id,p,n) - BLOCK_LOW(id,p,n)+1)
#define  BLOCK_OWNER(j, p, n)    (((p)*((j)+1)-1)/(n))

#define  N 10 // The size of the whole array of int to be sorted. 
/*
 * merge two arrays into the third array: Arrays are assumed to be 
 * sorted in ascending order
 */
void merge(int*, int, int*, int, int*); 

using namespace std; 

/*
 * Compile: mpicxx sorting.cpp -std=c++11
 */
int main (
	int    argc,
	char*  argv[])
{
	int         my_rank, total, i, j, k, toal_loc_value, receive_toal_loc_value;
        
        // implement step D
        // change int_to_sort to be a pointer and use dynamic memory
        int         *int_to_sort;
        int_to_sort = (int*)malloc(N * sizeof(int));
        
        int         v_receive[N], sorted[N], temp[N];
        int         startval; 
        MPI_Status  status;
        int          *new_arr;

        std::set<int> sort_set; 
        std::set <int>::iterator itr;
        string outfilename  = "output.txt";

        /* Timing variables */
        double starttime = 0.0;
        double endtime = 0.0;

        MPI_Init(&argc, &argv); 
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &total);

        outfilename += std::to_string(my_rank);
      
        ofstream ost;

        starttime = MPI_Wtime();

        if(0 == my_rank){
            /*Use random number generator to generate int. values to be sorted*/
            srand(time(0)); 
            for(i = 0; i < N; i++)
            {
                int_to_sort[i] = rand();
                //cout << i << "  " << int_to_sort[i] << endl;  
            }
            // process 0 send int to all other processes. 
            for(j = 1; j < total; j++){
                startval = BLOCK_LOW(j,total,N); 
                MPI_Send(&int_to_sort[startval], BLOCK_SIZE(j,total,N), 
                        MPI_INT, j, 1234, MPI_COMM_WORLD);
            }

            for(i = 0; i < BLOCK_SIZE(0,total,N); i++){
                sort_set.insert(int_to_sort[i]); 
            }
        }
        else{
            MPI_Recv(v_receive, BLOCK_SIZE(my_rank,total,N), MPI_INT,
                     0, 1234, MPI_COMM_WORLD, &status);

            for(i = 0; i < BLOCK_SIZE(my_rank,total,N); i++){
                sort_set.insert(v_receive[i]); 
                //cout<<"proc "<<my_rank<<" insert "<<v_receive[i]<<endl; 
            }
        }

        /*following code is common to all processes */
        new_arr = new int [sort_set.size()];    
        for (j = 0, itr = sort_set.begin(); itr != sort_set.end(); ++itr){
            new_arr[j] = *itr;
            j++;
        }
        toal_loc_value = sort_set.size();

        /*each process opens a file for writting*/
        ost.open(outfilename.c_str());
        if (!ost){
            cerr<<"unable to open file, "<<outfilename<<endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for(i = 0; i < toal_loc_value; i++){
            ost <<new_arr[i]<<endl; 
        }

        /* Now processes "1" to "total-1" send partial results to process 0*/
        /* This is done in two steps:
         * step 1: processes "1" to "total-1" send toal_loc_value to process 0
         *         so that process 0 knows the size of the data set to receive from
         *         other processes.
         * step 2: processes "1" to "total-1" send sorted values 
         *         in new_arr to process 0
         */ 
        // complete this part of the code
        if (0 != my_rank)
        {
                // step 1 send toal_loc_value
                MPI_Send(&toal_loc_value,1, MPI_INT, 0, 12345, MPI_COMM_WORLD);
                
                // step 2 send sorted values in new_arr
                MPI_Send(new_arr, toal_loc_value,
                        MPI_INT, 0, 123456, MPI_COMM_WORLD);
        }

        /*
         * In case there are duplicated values, std::set only saves
         * sorted unique values. 
         * Being said, this means lengths of array "new_arr" maybe different
         * on different processes. 
         *
         * How to handle this?
         *
         * To send the sorted values back to process 0, the communication
         * takes the following steps.  
         * 1. Other processes (rank value not 0) must first send the size 
         *    of "new_arr" (toal_loc_value) to process 0 
         *    so that process 0 knows number of 
         *    int values to receive from each of the other processes.  
         * 2. Process 0 uses the received "toal_loc_value"s to allocate 
         *    a set of arrays for storing
         *    sorted values to be received.
         * 3. Processes whose rank value are not 0 send the sorted 
         *    int values to process 0.
         */

        // Now process 0 computes the final result
        // This can be done by calling merge() multiple times. 
        if(0 == my_rank){
           
           
           for (j = 0; j<toal_loc_value; j++){
              temp[j] = new_arr[j];
           }

           for (j =1; j < total; j++)
           {  
                //receive the toal_loc_value
                MPI_Recv(&receive_toal_loc_value,1, MPI_INT, j, 12345, MPI_COMM_WORLD,  &status);

                //received the sorted array in v_receive
                MPI_Recv(v_receive, receive_toal_loc_value, MPI_INT, j, 123456, MPI_COMM_WORLD,  &status);
                
                // merge the temp with the v_receive
                merge(temp, toal_loc_value , v_receive, receive_toal_loc_value, sorted);
                
                //update the length of the combined array
                toal_loc_value = toal_loc_value + receive_toal_loc_value;
                //update the combined array
                for (i = 0; i<toal_loc_value; i++){
                     temp[i] = sorted[i];
                }

           }
           // save answer in file
           ost <<"Whole sorted array: "<<endl;
           for(i = 0; i < N; i++){
                ost <<sorted[i]<<endl; 
           }
        }

        delete [] new_arr; 
        ost.close();
        
        endtime = MPI_Wtime();

        if(0 == my_rank)
            printf("Proc %d tooks %f seconds.\n", my_rank, endtime-starttime);
  
        MPI_Finalize();  /* Shut down MPI */
        return 0;
}

/*
 * merge two arrays into the third array: Arrays are assumed to be 
 * sorted in ascending order.
 * inputs: 
 * *a, *b : two arrays to be merged.
 * m: size of array *a;
 * n: size of array *b;
 * sorted: combined results.
 */
void merge(int *a, int m, int *b, int n, int *sorted) {
  int i, j, k;
 
  j = k = 0;
 
  for (i = 0; i < m + n;) {
    if (j < m && k < n) {
      if (a[j] < b[k]) {
        sorted[i] = a[j];
        j++;
      }
      else {
        sorted[i] = b[k];
        k++;
      }
      i++;
    }
    else if (j == m) {
      for (; i < m + n;) {
        sorted[i] = b[k];
        k++;
        i++;
      }
    }
    else {
      for (; i < m + n;) {
        sorted[i] = a[j];
        j++;
        i++;
      }
    }
  }
}
