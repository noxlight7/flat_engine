#pragma once

#include <stdexcept>

template <typename T>
class Matrix {
private:
    T** data;
    int rows, cols;

    // Приватный метод для освобождения памяти
    void freeMemory();

public:
    // Конструктор с заданными размерами
    Matrix(int rows, int cols);
    Matrix(int rows, int cols, const T& initial);

    // Деструктор для освобождения памяти
    ~Matrix();

    // Перегрузка оператора () для доступа к элементу (с проверкой границ)
    T& operator()(int row, int col);
    const T& operator()(int row, int col) const;

    // Методы для получения размеров матрицы
    int rowCount() const;
    int colCount() const;

    // Метод для изменения размера матрицы
    void resize(int newRows, int newCols, const T& initial = T());
};

#include "common/matrix.tpp"