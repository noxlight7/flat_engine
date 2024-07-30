#pragma once

#include <stdexcept>

template <typename T>
class Matrix {
private:
    T** data;
    int rows, cols;

    // ��������� ����� ��� ������������ ������
    void freeMemory();

public:
    // ����������� � ��������� ���������
    Matrix(int rows, int cols);
    Matrix(int rows, int cols, const T& initial);

    // ���������� ��� ������������ ������
    ~Matrix();

    // ���������� ��������� () ��� ������� � �������� (� ��������� ������)
    T& operator()(int row, int col);
    const T& operator()(int row, int col) const;

    // ������ ��� ��������� �������� �������
    int rowCount() const;
    int colCount() const;

    // ����� ��� ��������� ������� �������
    void resize(int newRows, int newCols, const T& initial = T());
};

#include "common/matrix.tpp"