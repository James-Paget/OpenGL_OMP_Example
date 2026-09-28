#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main() {
    printf("Program start...\n");

    #pragma omp parallel
    {
        int n = omp_get_thread_num();
        printf("Thread %d\n", n);
    }

    printf("Program end\n");
    return 0;
}

/*
Tested using g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
-- COMPILE --
g++ -fopenmp -o omp_test_cpp openmp_test.cpp
-- RUN --
./omp_test_cpp
*/