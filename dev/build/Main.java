package com.company;
class Person {
int age;
String name;
int yearOfBirth;


Person(int age, String name, int yearOfBirth) {
  this.age = age;
  this.name = name;
  this.yearOfBirth = yearOfBirth;
}
}
class PersonBuilder {
int age;
String name;
int yearOfBirth;


public PersonBuilder setAge(int age) {
  this.age = age;
  return this;
}
public PersonBuilder setName(String name) {
  this.name = name;
  return this;
}
public PersonBuilder setYearOfBirth(int yearOfBirth) {
  this.yearOfBirth = yearOfBirth;
  return this;
}
public Person build() {
  return (new Person(age, name, yearOfBirth));
}
}
class Entrypoint {


void main() {
  Person person = (new PersonBuilder()).setName("Soumya").setAge(23).setYearOfBirth(1998).build();
  System.out.println(((((("" + person.name) + " is ") + person.age) + " years old. He was born in ") + person.yearOfBirth) + ".");
}
}
class Main {
  public static void main(String[] args) {
    var userClass = new Entrypoint();
    userClass.main();
  }
}


