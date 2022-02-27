package com.company;
  class ImportTest {int value;

    ImportTest(int value) {
      this.value = value;
    }
    ImportTest() {
    }
    public static void println() {
      System.out.println("Okay!");
    }
  }
class TranspileTest {
  public void main() {
    ImportTest(5);
  }
}
class Main {
  public static void main(String[] args) {
    var userClass = new TranspileTest();
    userClass.main();
  }
}


