package com.company;
  class ImportTest {int value;

    ImportTest(int value) {
    }
    ImportTest() {
    }
    public void println(String n) {
      System.out.println(n);
    }
    public int sum(int a, int b) {
      return a + b;
    }
  }
class TranspileTest {
  public void main() {
    ImportTest test = (new ImportTest());
    test.println(Integer.toString(test.sum(4, 5)));
  }
}
class Main {
  public static void main(String[] args) {
    var userClass = new TranspileTest();
    userClass.main();
  }
}


