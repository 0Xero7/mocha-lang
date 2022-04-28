package com.company;
class IntList {
private int[] values;
private int length = 0;
private int capacity = 2;

IntList() {
  values = int[capacity];
}
public int length() {
  return length;
}
public int at(int index) {
  return values[index];
}
public void add(int n) {
  if (length == capacity) increaseCapacity();
  values[length] = n;
  length = (length + 1);
}
private void increaseCapacity() {
  capacity = (capacity * 2);
  int[] newValues = int[capacity];
  for (int i = 0; i < length; i = (i + 1)) {
    newValues[i] = values[i];
  }
  values = newValues;
}
}
class Entrypoint {

void print(IntList list) {
  for (int i = 0; i < list.length(); i = (i + 1)) {
    System.out.print(("" + list.at(i)) + " ");
  }
  System.out.println();
}
public void main() {
  IntList list = IntList();
  for (int i = 1; i < 10; i = (i + 1)) {
    list.add(i);
    print(list);
  }
}
}
class Main {
  public static void main(String[] args) {
    var userClass = new Entrypoint();
    userClass.main();
  }
}


