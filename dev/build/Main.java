package com.company;

class Main {
  public static void main(String[] args) {
    var userClass = new TranspileTest();
    userClass.main();
  }
}

class TranspileTest {
  int sum(int a, int b) {
    int c = a + b;
    return c;
  }
  int factorial(int n) {
    if (n <= 0) return 1;
    else return n * factorial(n - 1);
  }
  public void main() {
    for (int i = 0; i <= 5; i = (i + 1)) {
      System.out.println("okay ${factorial(i)}");
    }
  }
}
