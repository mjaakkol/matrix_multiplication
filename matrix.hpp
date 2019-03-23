#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

#include <string.h>
#include <iostream>
#include <assert.h>

/**
 * @file matrix.hpp
 *
 * High performance Matrix implementation.
 *
 * @author Mikko Jaakkola
 */

/**
  * Matrix class for performing multiplication and transpose.
  * This class is minimal matrix calculation module aimed for portability, high performance and maintainability.
  * There are few design rules placed to ensure higher-performance and portability.
  * No exception handling is used to stay true to the objective of high-performace and portability as
  * some environments can have significant performance impact of handling try-catch block despite being
  * C++11 compatible or they just don't support the feature. The current library will work both with
  * and without exception handling. Not using exception handling prevented some operator overloading
  * so both multiplication and transpose is using custom method as well as the construction of the class
  * is split into two phases: constructor that cannot fail and initialization function that have operations
  * that can fail.
  *
  * Templates are used to make this work across multiple types but one needs to remember that each instance is
  * increasing the size of the code.
  *
  * Also no defensive programming is practiced so the user needs to ensure the correct
  * usage or the implementation bill blow up into asserts like fireworks at 4th of July.
  *

*/
using namespace std;

template <typename T>
class Matrix
{
  private:
    T *matrix_;       ///< Pointer to flat marrix data
    size_t rows_;     ///< Number of rows in the matrix
    size_t cols_;     ///< Number of columns in the matrix
    bool copy_;       ///< Was memory allocated by the object
    bool transposed_; ///< Is matrix transposed from the original format

  public:
    Matrix(T *mat, size_t rows, size_t cols);
    //! Empty Matrix constructor that needs to be explicitly initialized.
    Matrix() : Matrix(nullptr, 0, 0) {}
    virtual ~Matrix();

    //! Deleted to prevent accidents
    Matrix(const Matrix &) = delete;
    //! Deleted to prevent accidents
    Matrix &operator=(const Matrix &) = delete;

    const T operator()(size_t row, size_t col) const;
    T &operator()(size_t row, size_t col);
    bool operator==(const Matrix<T> &mat) const;

    bool initialize_matrix(size_t rows, size_t cols);
    bool copy_matrix_inplace();
    void matmul(Matrix<T> &result, const Matrix<T> &right_matrix) const;
    void transpose();

    template <typename U>
    friend std::ostream &operator<<(std::ostream &os, const Matrix<U> &m);
};

/**
 * Constructor for creating Matrix with pre-reserved memory.
 * @param mat a flat matrix array with per row format. First row one and then continuing with row two,...
 * @param rows number of rows in the matrix.
 * @param cols number of columns in the matrix.
 */

template <typename T>
Matrix<T>::Matrix(T *mat, size_t rows, size_t cols) : matrix_(mat),
                                                      rows_(rows),
                                                      cols_(cols),
                                                      copy_(false),
                                                      transposed_(false)
{
}

//! Destructor releases all memory reserved by the object
template <typename T>
Matrix<T>::~Matrix()
{
    // Delete the memory only if it was allocated
    // by the object
    if (copy_)
    {
        // Matrices are always going to have memory array even if the size
        // would have a length of one so we want to use delete for arrays.
        delete[] matrix_;
    }
}

/**
 * Method for (re-)initializing matrix.
 *
 * It reserved internal memory per defined size and initialized the object for use.
 * If the object has previous reserved memory, it will be released or repurposed. This
 * method will not initialize the memory in any way (eg. zero). If you need to initialize
 * the memory with some values, it can be done through constructor and then using copy_matrix_inplace
 * @see copy_matrix_inplace()
 * @param rows number of rows in the matrix.
 * @param cols number of columns in the matrix.
 * @return true if the memory allocation succeeded, otherwise false.
 */

template <typename T>
bool Matrix<T>::initialize_matrix(size_t rows, size_t cols)
{
    // We compare if the size of the memory matches the new
    // one and it is allocated by the object to check reuse.
    // User is always adviced to initialize_matrix if they
    // reuse the object as the internal implementation might
    // change in the future.
    transposed_ = false;

    if (rows == rows_ && cols == cols_ && copy_)
    {
        return true;
    }

    // If there was previous memory allocated by the object
    // we need to release that prior re-initializing the class
    if (copy_)
    {
        delete[] matrix_;
    }

    matrix_ = new T[rows * cols];
    if (matrix_)
    {
        rows_ = rows;
        cols_ = cols;
        // Not true copy but flagging that we have reserved the memory and it is
        // our responsibility to clean it up when the time comes.
        copy_ = true;
        return true;
    }
    return false;
}

/**
 * Method for copying Matrix using external memory to change it into using internal one.
 *
 * Implementation both allocated the memory and copies the previous matrix data into new buffer.
 *
 * @see initialize_matrix()
 * @return  true if the memory allocation succeeded, otherwise false.
 */

template <typename T>
bool Matrix<T>::copy_matrix_inplace()
{
    const auto size = rows_ * cols_;
    T *new_buffer = new T[size];
    if (new_buffer)
    {
        memcpy(new_buffer, matrix_, sizeof(T) * size);
        matrix_ = new_buffer;
        copy_ = true;
        return true;
    }

    return false;
}

/** Reference returning method for accessing matrix internal data.
 *
 * @param row Specifies the target row
 * @param col Specifies the target column
 * @return data in the given target row and column
 */

template <typename T>
inline T &Matrix<T>::operator()(size_t row, size_t col)
{
    if (transposed_)
    {
        return matrix_[col * rows_ + row];
    }
    else
    {
        return matrix_[row * cols_ + col];
    }
}

/** Value returning method for accessing matrix internal data.
 *
 * @param row Specifies the target row
 * @param col Specifies the target column
 * @return data in the given target row and column
 * @note: Redundant code but I didn't manage to get this to call the method above.
 */

template <typename T>
inline const T Matrix<T>::operator()(size_t row, size_t col) const
{
    if (transposed_)
    {
        return matrix_[col * rows_ + row];
    }
    else
    {
        return matrix_[row * cols_ + col];
    }
}

/**
 * Method for performing Matrix multiplication.
 *
 * Matrix multiplication is the workhose method and optimized for predictable operations
 * using non-defensive programming. It is assumed that the user feeds correctly formulated
 * matrices for the method. There are asserts in place to validate the basic sanity but
 * they will stop runtime execution if the inputs are not correct.
 *
 * Multiplication function always succeed and it takes both right-matrix and the
 * result-matrix (that will be overwritten).
 * @param result Reference to matrix where the result is written.
 * @param right_matrix Reference to the right matrix.
 *
 */

template <typename T>
void Matrix<T>::matmul(Matrix<T> &result, const Matrix<T> &right_matrix) const
{
    // Checking that these two matrixes can be multiplied together
    assert(cols_ == right_matrix.rows_);
    // Checking that the destination matrix sizes is valid
    assert(rows_ == right_matrix.cols_ && cols_ == right_matrix.rows_);

    // Result matrix must have valid pointer for the buffer
    assert(result.matrix_);

    // Result matrix must have the right size of buffer and format
    assert(rows_ == result.rows_ && right_matrix.cols_ == result.cols_);

    // Algorithm works one row at the time through left matrix
    for (size_t left_row = 0; left_row < rows_; ++left_row)
    {
        // In flat matrix, we move between rows through multiples of columns
        for (size_t right_col = 0; right_col < right_matrix.cols_; ++right_col)
        {
            // Must be initialized to zero as we add multiple times on top of this
            result(left_row, right_col) = 0;

            for (size_t right_row = 0; right_row < right_matrix.rows_; ++right_row)
            {
                // This loop takes care of the dot-product per row & column pair
                // moving each cycle from inside the current left row per column at the time
                // while multiplying it with right matrix where column is kept the same while
                // the operation moves down a row at the time (thus multiplies of right columns)
                result(left_row, right_col) += this->operator()(left_row, right_row) * right_matrix(right_row, right_col);
            }
        }
    }
}

/**
 * Method for inplace matrix transpose operation.
 *
 * Inplace transpose method is marking that the matrix has been transposed without
 * changing the memory structures. This enables allocating matrix memories from ROM
 * and still performing matrix operations for them making this slightly more portable
 * than implementation shuffling the transpose in the memory.
 */

template <typename T>
void Matrix<T>::transpose()
{
    transposed_ = !transposed_;
    swap(cols_, rows_);
}

/**
 * Method for checking if the content of two matrices is the same.
 *
 * This function wasn't part of the specification but used for carrying out module-testing and thus
 * efficiency wasn't the top priority non-transposed matrices works nicely but transposed ones
 * are doing comparison one item at the time.
 *
 * @param mat Right matrix for checking equality.
 * @return true if the matrix contents are the same. Otherwise false.
 * Note: this does not validate if both matrix objects are different as long as the content matches.
 */
template <typename T>
bool Matrix<T>::operator==(const Matrix<T> &mat) const
{
    if (cols_ != mat.cols_ || rows_ != mat.rows_)
    {
        return false;
    }

    if (transposed_)
    {
        // Here we must
        for (size_t i = 0; i < rows_; ++i)
        {
            for (size_t j = 0; j < cols_; ++j)
            {
                if (this->operator()(i, j) != mat(i, j))
                {
                    return false;
                }
            }
        }
        return true;
    }
    else
    {
        return !memcmp(matrix_, mat.matrix_, sizeof(T) * cols_ * rows_);
    }
}

/**
 * Method for using standard std::ostream for outputting Matrix.
 *
 * Method created mainly for debugging and printing the matrix content.
 * @see std::cout
 */
template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &m)
{
    os << "Dim(" << m.rows_ << "," << m.cols_ << ")\n[\n";

    for (size_t i = 0; i < m.rows_; ++i)
    {
        for (size_t j = 0; j < m.cols_; ++j)
        {
            os << m(i, j) << ", ";
        }

        os << '\n';
    }
    os << "]\n";
    return os;
}

#endif //_MATRIX_HPP_