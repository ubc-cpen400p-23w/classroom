#include <iostream>

using namespace std;

int add_func(int a, int b) {
  return a + b;
}

int main() {
  int x;                  //Tainted = {}
  cin >> x;               //Tainted = {x}
  int y = 0;              //Tainted = {x}
  int z = add_func(x,y);  //Tainted = {x,z}
  cout << z;              //Tainted = {x,z}
  z = 3;                  //Tainted = {x}
  return 0;               //Tainted = {x}
}

