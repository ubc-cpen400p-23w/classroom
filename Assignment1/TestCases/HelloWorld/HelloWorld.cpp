#include <iostream>

using namespace std;

int getElement1(int arr[]) {
  int loc;

  int temp = arr[loc + 1]; // Bug#1
  return temp + arr[loc];  // Bug#2
}

int getElement2(int arr[], int n) {
  int loc1;
  int loc2 = 1;

  if (n > 0) {
    loc2 = loc1; // Bug#3
  }
  return arr[loc2]; // Bug#4
}
