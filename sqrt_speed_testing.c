/*
* Writen by: Taylor B. Sage
* Speed testing of floating point functions using math.h, immintrin.h ( SSE, AVX2 )
* The will let you bench mark various mathmatical operations against a dataset of random double's 
* and check the various functions for accuracy.
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <immintrin.h>

#define TEST_DATA_SIZE (1 << 10) //size of the test data
#define MACRO_TEST_SIZE (1 << 14) //number of test itterations

// a standard test loop for the dataset size
#define TEST_LOOP for(int n=0;n<TEST_DATA_SIZE;n++)


// the test dataset structure
typedef struct test_data
{
    double* data;
    double* results;
    double* check;
} test_data;

/*
*  The benchmarking function.
*/
void timeFunction(void* F(test_data*), test_data* td)
{
    clock_t start_t, end_t;
    double total_t;
    start_t = clock();

    for (int i = 0; i < MACRO_TEST_SIZE; i++)
    {
        F(td);
    }
    end_t = clock();

    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    printf("Total time taken by CPU: %f\n", total_t);
}


//alocate the testing data
test_data *create_test_data(void)
{
    test_data* td;
    td = malloc(sizeof(struct test_data));
    td->data = malloc(sizeof(double) * TEST_DATA_SIZE);
    td->results = malloc(sizeof(double) * TEST_DATA_SIZE);
    td->check = malloc(sizeof(double) * TEST_DATA_SIZE);
    return td;
}

// generate the test data from random doubles and zero out the results and check values;
void gen_test_data(test_data *td)
{
    TEST_LOOP
    {
        td->data[n] = (double)rand() / (double)(RAND_MAX / (RAND_MAX / 2.0));
        td->results[n] = 0;
        td->check[n] = 0;
    }
}

//dealocate the testing data memory
void release_test_data( test_data* td)
{
    free(td->check);
    free(td->results);
    free(td->data);
    free(td);
}

/*
* Set the check values based on the last results calculated.
*/
void setcheck(test_data* td)
{
    TEST_LOOP
    {
        td->check[n] = td->results[n];
    }
}

/*
*  Check results, and sum the the absolute error
*/
double check_results(test_data *td)
{
    double error = 0.0;
    TEST_LOOP
    {
        error += fabs(td->results[n] - td->check[n]);
    }
    printf("Tottal Error: %.17f\n", error);
    return error;
}

/*
*  Reset the results
*/
void zero_results(test_data* td)
{
    TEST_LOOP
    {
        td->results[n] = 0.0;
    }
}

/*
*  Functions to be testd
*/
void sqrt_test(test_data* td)
{
    for (int n = 0; n < TEST_DATA_SIZE; n++)
    {
        td->results[n] = sqrt(td->data[n]);
    }
}

void avx_sqrt_test(test_data* td)
{
    for (int n = 0; n < TEST_DATA_SIZE; n += 4)
    {
        //using AVX intrinsic instructions, run sqrt against 4 double's at the same time
        _mm256_store_pd(td->results + n, _mm256_sqrt_pd(_mm256_setr_pd(td->data[n], td->data[n + 1], td->data[n + 2], td->data[n + 3])));
    }
}

void sse_sqrt_test(test_data* td)
{
    for (int n = 0; n < TEST_DATA_SIZE; n++)
    {
        //using SSE intrinsic instructions, run sqrt against 1 double at the same time
        _mm_storel_pd(td->results + n, _mm_sqrt_pd(_mm_setr_pd(td->data[n], td->data[n])));
    }
}

/* An intentionaly pore implimentation of sqrt using 10 rounds of newton-raphson after a really silly aproximation*/
inline double bad_sqrt(double x)
{
    double y = x;
    // Approximation
    //  The worst aproximation
    x = x / (x * 0.25);
    // Newton-Raphson
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;
    x = (x + y / x) / 2;

    return x;
}

void bad_sqrt_test(test_data* td)
{
    for (int n = 0; n < TEST_DATA_SIZE; n++)
    {
        td->results[n] = bad_sqrt(td->data[n]);
    }
}





int main(int argc, char* argv)
{
    test_data* td = create_test_data();
    // create the random test data
    gen_test_data(td);
    
    // time sqrt_test function for total run time
    timeFunction(sqrt_test,td);
    // set the check values to the correct values since this function is the standard
    setcheck(td);

    // time avx_sqrt_test function for total run time
    timeFunction(avx_sqrt_test, td);

    // check the results of f2 against the correct values
    check_results(td);

    //zero out the results
    zero_results(td);

    //run the sse test
    timeFunction(sse_sqrt_test, td);

    //check the results
    check_results(td);

    //zero out the results
    zero_results(td);

    //run the sse test
    timeFunction(bad_sqrt_test, td);

    //check the results
    check_results(td);


    // release the test data alocations
    release_test_data(td);

    return(0);
}
