package com.company;
  class ImportTest {int value;

    ImportTest(int value) {
      this.value = value;
    }
    ImportTest() {
    }
    public static void println() {
    }
    public int sum(int a, int b) {
      return a + b;
    }
  }
class TranspileTest {
  public void main() {
    test = ImportTest();
  }
}
class Main {
  public static void main(String[] args) {
    var userClass = new TranspileTest();
    userClass.main();
  }
}


