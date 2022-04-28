package com.company;
class A {

class B {


}

}
class Entrypoint {


public void main() {
  B.test = A.B();
}
}
class Main {
  public static void main(String[] args) {
    var userClass = new Entrypoint();
    userClass.main();
  }
}


