package com.company;
class Matrix {int[][] value;

  Matrix(int[][] value) {
    this.value = value;
  }
  Matrix sum(int[][] other) {
    Matrix ret = (new Matrix(this.value));
    for (int i = 0; i < 2; i = (i + 1)) {
      for (int j = 0; j < 3; j = (j + 1)) {
        ret.value[i][j] = (ret.value[i][j] + other[i][j]);
      }
    }
    return ret;
  }
  void print() {
    for (int i = 0; i < 2; i = (i + 1)) {
      for (int j = 0; j < 3; j = (j + 1)) {
        System.out.print(("" + this.value[i][j]) + " ");
      }
      System.out.println();
    }
  }
}
class TranspileTest {
  public void main() {
    Matrix A = (new Matrix(new int[][]{ new int[]{ 1, 2, 3, }, new int[]{ 4, 5, 6, }, }));
    A.sum(new int[][]{ new int[]{ 1, 2, 3, }, new int[]{ 4, 5, 6, }, }).print();
  }
}
class Main {
  public static void main(String[] args) {
    var userClass = new Matrix();
    userClass.main();
  }
}


