#include <stdio.h>
#include <random>
// #include <vector>

// #include <chrono>
using namespace std;

// #include <iostream>

#include <xcore/hwtimer.h>
#include <xcore/parallel.h>

// float_s32_t x = 0.0;

void mul(float* A, float* B, bool elementWise, float* C, int row_a, int column_a, int column_b) {

	/*
	 * C = A*B if elementWise = false
	 * C = A.*B if elementWise = true
	 */

	// Data matrix
	float* data_a = A;
	float* data_b = B;
	//float* C = C; // No need

	if (elementWise == true) {

		// Dot multiply all values
		if (column_b > 1) { // If matrix b is a matrix
			for (int i = 0; i < row_a; i++) {
				for (int j = 0; j < column_a; j++) {
					// Do element wise mutiplication. In MATLAB it is A.*A
					*(C++) = *(data_a++) * *(data_b++);
				}
			}
		} else {
			// If matrix b is a vector
			for (int i = 0; i < row_a; i++) {
				for (int j = 0; j < column_a; j++) {
					// Do element wise mutiplication. In MATLAB it is A.*b
					*(C++) = *(data_a++) * *(data_b + i);
				}
			}
		}

	} else {
		// Do regular mutiplication. In MATLAB it is A*A
		// Let's take our a matrix
		for (int i = 0; i < row_a; i++) {

			// Then we go through every column of b
			for (int j = 0; j < column_b; j++) {
				data_a = &A[i * column_a];
				data_b = &B[j];

				*C = 0; // Reset
				// And we multiply rows from a with columns of b
				for (int k = 0; k < column_a; k++) {
					*C += *data_a * *data_b;
					data_a++;
					data_b += column_b;
				}
				C++;
			}
		}
	}
}

DECLARE_JOB(runtest, (int))

void runtest(int id) {

	std::uniform_real_distribution<float> unif(-1.0, 1.0);
  std::default_random_engine re;

	const int dimA = 100;
	const int dimB = 1;
  float D[dimA * dimA];
  float E[dimA];
  float F[dimA];

	for(size_t i =0; i < dimA * dimA; i++) {
		D[i] = unif(re);
	}
	for(size_t i =0; i < dimA; i++) {
		E[i] = unif(re);
		F[i] = unif(re);
	}

	hwtimer_t timer = hwtimer_alloc();
	auto now = hwtimer_get_time(timer);
	int iterations = 10000;
	for(size_t i=0; i < iterations; i++) {
  	mul(&D[0], &E[0], 0, &F[0], dimA, dimA, dimB);
	}
	auto t2 = hwtimer_get_time(timer);
	printf("%d start time: %lf\n", id, now * 1e-8);
	printf("%d end time: %lf\n", id, t2 * 1e-8);
	printf("%d total time: %lf\n", id, (t2-now) * 1e-8);
	printf("%d unit time: %lf\n", id, (t2-now) / iterations * 1e-8);




	// auto duration = duration_cast<microseconds>(stop - start);
	// printf("%lld\n", duration.count());
	// cout << "Malmul: " << duration.count() << " us" << endl;

  for(int i=0; i < 25; i++) {
    printf("%lf\t", F[i]);
  }
  printf("\n");

	hwtimer_free(timer);	
}

int main(void) {
  printf("Matmul Test!\n");

  float A[4] = {1,2,3,4};
  float B[4] = {5,6,7,8};
  float C[4] = {0,0,0,0};

  mul(&A[0], &B[0], 0, &C[0], 2,2,2);

  for(int i=0; i < 4; i++) {
    printf("%lf\t", C[i]);
  }
  printf("\n");


	// runtest(0);
  PAR_JOBS(
    PJOB(runtest, (0))
    // PJOB(runtest, (1)),
    // PJOB(runtest, (2)),
    // PJOB(runtest, (3)),
    // PJOB(runtest, (4))
    // PJOB(runtest, (5)),
    // PJOB(runtest, (6)),
    // PJOB(runtest, (7))
);

  return 0;
}

