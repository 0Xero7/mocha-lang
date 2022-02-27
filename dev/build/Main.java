package com.company;
  class ImportTest {
    public static void print() {
      System.out.println("Okay!");
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
    ImportTest.println((("" + (3 + 4)) + " okay ") + (5 + 6));
  }
}
class Utils {
  void println() {
    ImportTest.println("okay");
  }
}
class Main {
  public static void main(String[] args) {
    var userClass = new TranspileTest();
    userClass.main();
  }
}


