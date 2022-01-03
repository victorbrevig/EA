#include "pch.h"
#include "matrix.h"

namespace Utils
{
  Matrix::Matrix(size_t rows, size_t cols)
  {
    content.resize(rows * cols);
    firstDim = rows;
  }

  size_t Matrix::RowAndColToContentIndex(size_t i, size_t j)
  {
    return i * firstDim + j;
  }

  void Matrix::Set(size_t i, size_t j, double val)
  {
    content[RowAndColToContentIndex(i, j)] = val;
  }

  double Matrix::Get(size_t i, size_t j)
  {
    return content[RowAndColToContentIndex(i, j)];
  }
}
