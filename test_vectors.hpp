#ifndef _TEST_VECTOR_
#define _TEST_VECTOR_

// Test-vector matrix_1
size_t matrix_1_rows = 3;
size_t matrix_1_cols = 4;
unsigned int matrix_1_data[] ={ 6, 8, 2, 8, 1, 1, 7, 0,11, 3, 5,11};

// Test-vector matrix_1_transpose
size_t matrix_1_transpose_rows = 4;
size_t matrix_1_transpose_cols = 3;
unsigned int matrix_1_transpose_data[] ={ 6, 1,11, 8, 1, 3, 2, 7, 5, 8, 0,11};

// Test-vector matrix_1_product
size_t matrix_1_product_rows = 3;
size_t matrix_1_product_cols = 3;
unsigned int matrix_1_product_data[] ={168, 28,188, 28, 51, 49,188, 49,276};

// Test-vector matrix_2
size_t matrix_2_rows = 1;
size_t matrix_2_cols = 8;
unsigned int matrix_2_data[] ={ 5, 4, 9, 5,11, 7,10, 2};

// Test-vector matrix_2_transpose
size_t matrix_2_transpose_rows = 8;
size_t matrix_2_transpose_cols = 1;
unsigned int matrix_2_transpose_data[] ={ 5, 4, 9, 5,11, 7,10, 2};

// Test-vector matrix_2_product
size_t matrix_2_product_rows = 1;
size_t matrix_2_product_cols = 1;
unsigned int matrix_2_product_data[] ={421};

#endif //_TEST_VECTOR_

