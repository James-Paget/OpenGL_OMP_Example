/*Test code from https://www.open-mpi.org/papers/workshop-2006/hello.c*/

#include <mpi.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Hello, World.  I am %d of %d\n", rank, size);

    MPI_Finalize();
    return 0;
}

/*
--BUILD--
mpicc -o hello.exe mpi_test.c
--RUN--
mpirun -np 2 ./hello.exe

-> Check for output to ensure MPI is working
*/