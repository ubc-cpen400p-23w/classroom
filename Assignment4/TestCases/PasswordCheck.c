#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 12

int verify_password(char* password, int offsetA) {
	int correct = 1;
	int offsetB = 2;
	char passwordBuffer[SIZE];
	const char actualPwd[] = "realpassword";
	
	for (int i=0; i < SIZE; i++) {
	  passwordBuffer[i] = password[i];
	}
	
	for (int i=0; i < SIZE; i++) {
	  char pwdChar = passwordBuffer[i] - offsetA + offsetB;
	  assert(pwdChar >= 'a' && pwdChar <= 'z');  
	  if (pwdChar != actualPwd[i]) {
	    correct = 0;
	  }
	}

	return correct;
}

int main(int argc,char **argv) {
  int offsetA = 2;

  int check = verify_password(argv[1], offsetA);
  if (check) {
    printf("Password matched!\n");
  } else {
    printf("Password did not match\n");
  }
}

