#include <stdio.h>
#include <random>
// #include <vector>

// #include <chrono>
using namespace std;

// #include <iostream>

#include <xcore/hwtimer.h>
#include <xcore/parallel.h>

#include "xmath/xmath.h"

float_s32_t x;

void mul(float_s32_t* A, float_s32_t* B, bool elementWise, float_s32_t* C, int row_a, int column_a, int column_b) {

	/*
	 * C = A*B if elementWise = false
	 * C = A.*B if elementWise = true
	 */

	// Data matrix
	float_s32_t* data_a = A;
	float_s32_t* data_b = B;
	//float* C = C; // No need

	if (elementWise == true) {

		// // Dot multiply all values
		// if (column_b > 1) { // If matrix b is a matrix
		// 	for (int i = 0; i < row_a; i++) {
		// 		for (int j = 0; j < column_a; j++) {
		// 			// Do element wise mutiplication. In MATLAB it is A.*A
		// 			*(C++) = *(data_a++) * *(data_b++);
		// 		}
		// 	}
		// } else {
		// 	// If matrix b is a vector
		// 	for (int i = 0; i < row_a; i++) {
		// 		for (int j = 0; j < column_a; j++) {
		// 			// Do element wise mutiplication. In MATLAB it is A.*b
		// 			*(C++) = *(data_a++) * *(data_b + i);
		// 		}
		// 	}
		// }

	} else {
		// Do regular mutiplication. In MATLAB it is A*A
		// Let's take our a matrix
		for (int i = 0; i < row_a; i++) {

			// Then we go through every column of b
			for (int j = 0; j < column_b; j++) {
				data_a = &A[i * column_a];
				data_b = &B[j];

				*C = f32_to_float_s32(0); // Reset
				// And we multiply rows from a with columns of b
				for (int k = 0; k < column_a; k++) {
					auto v = float_s32_mul(*data_a, *data_b);
					*C = float_s32_add(*C, v);
					// *C += *data_a * *data_b;
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

	const int dimA = 30;
	const int dimB = 1;
  float_s32_t D[dimA * dimA];
  float_s32_t E[dimA];
  float_s32_t F[dimA];

	for(size_t i =0; i < dimA * dimA; i++) {
		D[i] = f32_to_float_s32(unif(re));
	}
	for(size_t i =0; i < dimA; i++) {
		E[i] = f32_to_float_s32(unif(re));
		// F[i] = f32_to_float_s32(unif(re));
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

  // for(int i=0; i < 25; i++) {
  //   printf("%lf\t", F[i]);
  // }
  // printf("\n");

	hwtimer_free(timer);	
}

int main(void) {
  printf("Matmul Test!\n");

  float_s32_t A[4] = {f32_to_float_s32(1),f32_to_float_s32(2),f32_to_float_s32(3),f32_to_float_s32(4)};
  float_s32_t B[4] = {f32_to_float_s32(5),f32_to_float_s32(6),f32_to_float_s32(7),f32_to_float_s32(8)};
  float_s32_t C[4];
 
  mul(&A[0], &B[0], 0, &C[0], 2,2,2);

  for(int i=0; i < 4; i++) {
    printf("%f\t", float_s32_to_float(C[i]));
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

