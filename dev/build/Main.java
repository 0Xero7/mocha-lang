package com.company;
class PersonBuilder {
int age;
String name;


public PersonBuilder setAge(int age) {
  this.age = age;
  return this;
}
public PersonBuilder setName(String name) {
  this.name = name;
  return this;
}
public Person build() {
  return (new Person(age, name));
}
}
class Person {
int age;
String name;


Person(int age, String name) {
  this.age = age;
  this.name = name;
}
}
class Entrypoint {


public void main() {
  Person person = (new PersonBuilder()).setAge(23).setName("Soumya").build();
  System.out.println(((("" + person.name) + " is ") + person.age) + " years old.");
}
}
class Main {
  public static void main(String[] args) {
    var userClass = new Entrypoint();
    userClass.main();
  }
}


