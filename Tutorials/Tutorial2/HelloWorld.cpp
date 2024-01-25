// Sample C++ File for LLVM Tutorial

#include <iostream>

using namespace std;

int foo(int x, int y) {
  int z = x + y;
  return z;
}

int bar(int x) {
  if (x > 0)
    return x;
  return 0;
}

void func(int x) {
  int f = bar(x);
  cout << "Hello with non-negative numbers: " << f << endl;
}

int main() {
  cout << "Hello world!\n";
  int a = 1;
  int b = 2;
  
  func(foo(a, b));
  return 0;
}

