#ifndef MATRIX_H
#define MATRIX_H
#include <vector>

namespace Utils
{
  class Matrix
  {
    std::vector<double> content;
    size_t firstDim;
    size_t RowAndColToContentIndex(size_t i, size_t j);
  public:
    Matrix(size_t rows, size_t cols);
    void Set(size_t i, size_t j, double val);
    double Get(size_t i, size_t j);
  };
}
#endif