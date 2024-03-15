#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

int main(int argc,char **argv) {
  double a, b, c, d, e, f, r1, r2;
  printf("Enter values for a, b and c: ");
  scanf("%lf %lf %lf", &a, &b, &c);

  e = 0;
  d = b * b - 4 * a * c;

  r1 = (-b + pow(d, 3)) / (4 * a);
  r2 = (-b - pow(d, 3)) / (2 * a);

  if (a != 0) {
    if (d > 0) {
      assert(r1 > r2 && r1 == 0);  // Assert1
    }
    else if (d == 0) {
      assert(r1 != r2);            // Assert2
      if (b != 0 && c == 0) {
        assert(r1 == r2);          // Assert3
      }
    }
    else if (d < 0) {
      f = e * d + e;
      assert(f > e);               // Assert4
    }
  }
}

