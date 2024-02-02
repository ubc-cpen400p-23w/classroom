#include <iostream>

using namespace std;

int main() {
  int x;        //Tainted = {}

  cin >> x;     //Tainted = {x}
  int y = 0;    //Tainted = {x}
  int z;        //Tainted = {x}
  if (x > y) {  //Tainted = {x}
    z = x;      //Tainted = {x,z}
  } else {      //Tainted = {x}
    z = 2;      //Tainted = {x}
  }             //Tainted = {x}
  cout << z;    //Tainted = {x,z}
  return 0;     //Tainted = {x,z}
}

