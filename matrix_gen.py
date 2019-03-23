#! /usr/bin/env python
# Generate test-vectors
# The script is meant to be used in format "python3 matrix_gen.py > test_vectors.hpp"

import numpy as np

def generate_matrix(rows, cols):
    # Generate random matrix. The range is limited to 12 to keep
    # number more managable.
    np_matrix = np.random.randint(12, size=(rows, cols))
    return np_matrix


def matrix2struct(postfix: str, mat: np.array):
    np_flatten_matrix = mat.flatten()
    # prints valid vector in C++ format to be used creating test vectors.
    print("// Test-vector", postfix)
    matrix = """size_t {0}_rows = {1};\nsize_t {0}_cols = {2};\nunsigned int {0}_data[] ={{{3}}};\n"""\
    .format(postfix, *mat.shape, np.array2string(np_flatten_matrix, prefix='{', separator=',')[1:-1])

    return matrix

if __name__ == "__main__":
    print("#ifndef _TEST_VECTOR_\n#define _TEST_VECTOR_\n")

    matrix_1 = generate_matrix(3, 4)

    matrix_1_transpose = matrix_1.T

    matrix_1_product = np.matmul(matrix_1, matrix_1_transpose)

    matrix_2 = generate_matrix(1,8)

    matrix_2_transpose = matrix_2.T

    matrix_2_product = np.matmul(matrix_2, matrix_2_transpose)

    # To make things simple, the vectors are
    # created from the local variables names
    data = locals().copy()

    for i in data:
        if i.startswith('matrix_'):
            print(matrix2struct(i, data[i]))

    print("#endif //_TEST_VECTOR_\n")

