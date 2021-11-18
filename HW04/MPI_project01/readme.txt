1. To setup MPI environment, use command: 
        module load mpich/3.3/intel/19.0

2. To compile the code, use command: mpicxx sorting.cpp -std=c++11

3. To run the program with 3 processes, use command:
       mpiexec -np 3 ./a.out

The goal of this project is to develop a MPI C++ code for sorting 
integers in parallel. It generalizes the basic sorting.cpp code we discussed 
in the class by combining it with idea explained in the parallel summation code.

The parallel algorithm of the code to be developed is as follows:

Let's say we run this program with "total" number of processes.
The size of the integer array is "N", which is defined as a macro in the code.

step 1. Process "0" uses random number generator to generate "N" integers and
        store them in int_to_sort[] array. 

step 2. Process "0" send subsets of ints from int_to_sort[] to 
            all other processes.        
   sub-step 2a. let "j" be the rank of a process who is to receive 
                 int from process 0. We use "startval = BLOCK_LOW(j,total,N);"
                 to compute the beginning index of the int in int_to_sort[] 
                 to send to process "j". The size of the ints to send to 
                 process "j" is computed by "BLOCK_SIZE(j,total,N)".
   sub-step 2b. Process "0" call MPI_Send() to send ints to processes "1"
                    to "total-1". Processes "1" to "total-1" call MPI_Recv()
                    to receive ints from process "0".

step 3. All processes use C++ std::set<int> sort_set to sort these 
        subsets of ints, respectively. And save the sorted 
        ints to dynamic array new_arr, 
        whose size is "toal_loc_value = sort_set.size()".

step 4. Process "0" combines these sorted "new_arr" as follows:
   step 4a. Since std::set<int> only saves unique values, the sizes 
            of the sorted ints in "new_arr" on the processes might be different. 
            Processes "1" to "total-1" call  MPI_Send() to send these sizes
            "toal_loc_value" to process "0". 
            Process "0" call MPI_Recv() correspondingly. 
   step 4b. Process "0" uses the received "toal_loc_value"s to allocate
           a set of arrays for saving sorted values to be received. 
   step 4c. Processes "1" to "total-1" call MPI_Send() to send "new_arr" 
           to process "0". Process "0" calls MPI_Recv() to receive.
   step 4d. Process "0" call  merge() function several times to combine
            all received ints from step 4c. 


You need to complete step 4 for the project. 

To develop and test the code, use "#define N 10" and 3 MPI processes. 
In this way, we can check the output files for correctness of the program.


Next, change N to 100, 1000, 10000 and number of MPI processes to 3, 4, 5 respectively and use the job script to the program. 
Timing computer times for each run. This is done by MPI_Wtime() function. 



In the project report, please explain in detail how you implement step 4. 
It should contain: comments of variables used for this step; how you implement these communications; and computer times for each of runs. 


Submission: please include source code and the project report. 
