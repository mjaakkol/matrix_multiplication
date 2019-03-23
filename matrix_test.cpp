#include <iostream>
#include "matrix.hpp"

#include "test_vectors.hpp"

typedef Matrix<unsigned int> MatInt;

using namespace std;

int main()
{
    cout<<"\n============TESTING STARTS=============\n";
    MatInt a(matrix_1_data, matrix_1_rows, matrix_1_cols);
    MatInt b(matrix_1_data, matrix_1_rows, matrix_1_cols);

    assert(a == b);

    cout<<"Equal operator succeeded\n"<<endl;

    cout<<"Before transpose:"<<b;

    // We must copy matrix in place as otherwise it will modify
    // memory shared with matrix 1
    assert(b.copy_matrix_inplace());

    b.transpose();

    cout<<"\nAfter transpose:"<<b;

    // Creating transposed vector from external vectors
    MatInt transpose_test(matrix_1_transpose_data,
                          matrix_1_transpose_rows,
                          matrix_1_transpose_cols);

    assert(b == transpose_test);

    // Double-checking that the conversion works also otherway around
    b.transpose();

    assert(b == a);

    cout<<"\nTranspose succeeded\n"<<endl;

    // Testing multiplication with memory allocation

    // Transposing b back to transposed version as we use that in
    // multiplications and against pre-calculated vectors
    b.transpose();

    assert(b == transpose_test);

    MatInt product_result_test(matrix_1_product_data, 
                               matrix_1_product_rows,
                               matrix_1_product_cols);

    MatInt product_result;

    assert(product_result.initialize_matrix(matrix_1_rows, matrix_1_transpose_cols));

    cout<<"\nMatrices to be multiplied\n";
    cout<<a;
    cout<<b;

    // In this case matmul needs to both allocate the memory
    // and do the multiplication. Assert tests if the memory
    // allocation worked.
    a.matmul(product_result, b);

    cout<<"\nResult matrix\n";
    cout<<product_result;

    assert(product_result == product_result_test);

    // REPEAT THE SAME AS ABOVE WITH VECTORS

    // Another round of product testing corner case where
    // end result is matrix with 1x1 dimensions
    MatInt a2(matrix_2_data, matrix_2_rows, matrix_2_cols);

    MatInt a2_right(matrix_2_data, matrix_2_rows, matrix_2_cols);

    assert(a2_right.copy_matrix_inplace());

    a2_right.transpose();

    MatInt a2_transpose(matrix_2_transpose_data, matrix_2_transpose_rows, matrix_2_transpose_cols);

    assert(a2_right == a2_transpose);

    cout<<"\n\nTranspose succeeded with matrix 2\n";

    MatInt p2_result_test(matrix_2_product_data, matrix_2_product_rows, matrix_2_product_cols);

    MatInt p2_result;

    assert(p2_result.initialize_matrix(matrix_2_rows, matrix_2_transpose_cols));

    cout<<"\nMatrices to be multiplied:\n";
    cout<<a2;
    cout<<a2_right;    

    a2.matmul(p2_result, a2_right);

    assert(p2_result == p2_result_test);

    cout<<"\nResult matrix\n";
    cout<<p2_result;
    cout<<"\n============TESTS PASSED=============";
}