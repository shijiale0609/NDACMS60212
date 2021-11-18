
mpicxx sorting.cpp -std=c++11 -o a.out

mpicxx sorting_100.cpp -std=c++11 -o a100.out

mpicxx sorting_1000.cpp -std=c++11 -o a1000.out

mpicxx sorting_10000.cpp -std=c++11 -o a10000.out


echo "N=10, np =3"
mpiexec -np 3 ./a.out 
echo "N=10, np =4"
mpiexec -np 4 ./a.out 
echo "N=10, np =5"
mpiexec -np 5 ./a.out 

echo "N=100, np =3"
mpiexec -np 3 ./a100.out 
echo "N=100, np =4"
mpiexec -np 4 ./a100.out 
echo "N=100, np =5"
mpiexec -np 5 ./a100.out 

echo "N=1000, np =3"
mpiexec -np 3 ./a1000.out 
echo "N=1000, np =4"
mpiexec -np 4 ./a1000.out 
echo "N=1000, np =5"
mpiexec -np 5 ./a1000.out 

echo "N=10000, np =3"
mpiexec -np 3 ./a10000.out 
echo "N=10000, np =4"
mpiexec -np 4 ./a10000.out 
echo "N=10000, np =5"
mpiexec -np 5 ./a10000.out 

