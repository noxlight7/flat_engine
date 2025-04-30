#pragma once

template <typename T>
Matrix<T>::Matrix(int rows, int cols) : rows(rows), cols(cols) {
    data = new T * [rows];
    for (int i = 0; i < rows; ++i) {
        data[i] = new T[cols];
    }
}

template <typename T>
Matrix<T>::Matrix(int rows, int cols, const T& initial) : rows(rows), cols(cols) {
    data = new T * [rows];
    for (int i = 0; i < rows; ++i) {
        data[i] = new T[cols];
        for (int j = 0; j < cols; ++j) {
            data[i][j] = initial;
        }
    }
}

template <typename T>
Matrix<T>::~Matrix() {
    freeMemory();
}

template <typename T>
void Matrix<T>::freeMemory() {
    for (int i = 0; i < rows; ++i) {
        delete[] data[i];
    }
    delete[] data;
}

template <typename T>
T& Matrix<T>::operator()(int row, int col) {
    if (row >= rows || col >= cols || row < 0 || col < 0) {
        throw std::out_of_range("Индекс вне границ матрицы");
    }
    return data[row][col];
}

// template <typename T>
// T Matrix<T>::get(int row, int col) const {
//     return (*this)(row, col);
// }

template <typename T>
const T& Matrix<T>::operator()(int row, int col) const {
    if (row >= rows || col >= cols || row < 0 || col < 0) {
        throw std::out_of_range("Индекс вне границ матрицы");
    }
    return data[row][col];
}

template <typename T>
int Matrix<T>::rowCount() const {
    return rows;
}

template <typename T>
int Matrix<T>::colCount() const {
    return cols;
}

template <typename T>
void Matrix<T>::resize(int newRows, int newCols, const T& initial) {
    T** newData = new T * [newRows];
    for (int i = 0; i < newRows; ++i) {
        newData[i] = new T[newCols];
    }

    // Копирование старых значений в новую матрицу
    for (int i = 0; i < std::min(rows, newRows); ++i) {
        for (int j = 0; j < std::min(cols, newCols); ++j) {
            newData[i][j] = data[i][j];
        }
    }

    // Заполнение новых элементов значением initial
    for (int i = 0; i < newRows; ++i) {
        for (int j = (i < rows ? cols : 0); j < newCols; ++j) {
            newData[i][j] = initial;
        }
    }

    // Освобождение старой памяти
    freeMemory();

    // Присвоение новых значений
    data = newData;
    rows = newRows;
    cols = newCols;
}
