# Assignment 3

## Objective

Write an LLVM pass to enable dynamic analysis of the input code for finding Use-after-free (UAF) and Double-free (DF) bugs. Upon finding the bug, your code should (1) print out the source code line number at which the bug occurred along with the type of bug (2) after printing the bug info, the code should immediately terminate with the `exit(1)` function call.
Unlike the previous assignments, which performed static analysis using LLVM, in this assignment, you will have to perform dynamic analysis by appropriately instrumenting the code.

Assume the following for this assignment:
1. Every test case (hidden or visible) will have only ONE UAF or DF bug.
2. The memory location (pointer) being freed was once a valid heap memory location.
3. The `Load`, `Store`, `GetElementPointer`, and `Call` are the only LLVM instructions that manipulate or use a pointer.
4. `malloc()`, `calloc()` are the only heap memory allocator functions used in the program, and `free()` is the only heap memory deallocator.
5. The heap memory location, once freed, might be reused upon the subsequent calls to a heap allocator. For example, if the program frees the heap memory at location `0xffffdead`, then a subsequent call to `malloc` or `calloc` functions might reallocate the memory at location `0xffffdead`.

## Background

The following bugs are heap vulnerabilities that arise due to the incorrect usage of heap during the program operation.

#### Use-after-free (UAF) bugs

UAF bugs arise when the program references the heap memory after it has been freed, which can cause the program to crash, use unexpected values, or execute malicious code. The following blog post describes how to exploit a recent UAF bug in Google Chrome [https://securitylab.github.com/research/CVE-2020-6449-exploit-chrome-uaf/](https://securitylab.github.com/research/CVE-2020-6449-exploit-chrome-uaf/).

#### Double free (DF) bugs:

DF bugs arise when the program tries to free the same heap memory twice. Doubly freeing memory may result in a write-what-where condition, allowing an attacker to execute arbitrary code.
The following blog post describes how to exploit a recent DF bug within the Linux kernel: [https://www.synacktiv.com/en/publications/exploitation-of-a-double-free-vulnerability-in-ubuntu-shiftfs-driver-cve-2021-3492.html](https://www.synacktiv.com/en/publications/exploitation-of-a-double-free-vulnerability-in-ubuntu-shiftfs-driver-cve-2021-3492.html)

## Example 1

```cpp
1. #include <stdio.h>
2. #include <unistd.h>
3. #include <stdlib.h>
4. #include <string.h>

5. #define BUFSIZER1   512
6. #define BUFSIZER2   ((BUFSIZER1/2) - 8)

7. int main(int argc, char **argv) {
8.  char *buf1R1;
9.  char *buf2R1;
10.  char *buf2R2;
11.  char *buf3R2;
12.  void* temp;

13.  buf1R1 = (char *) malloc(BUFSIZER1);
14.  buf2R1 = (char *) malloc(BUFSIZER1);

15.  temp = buf2R1 + 10;
16.  free(buf2R1);

17.  buf2R2 = (char *) malloc(BUFSIZER2);
18.  buf3R2 = (char *) malloc(BUFSIZER2);

19.  strncpy(temp, argv[1], BUFSIZER1-1); /* Use After Free Bug */
20.  free(buf1R1);
21.  free(buf2R2);
22.  free(buf3R2);
23.}
```

For this example, the pass should return the following:
```shell
Use after free bug at line: 19
```

### Explanation:

At line 16, the heap memory pointed by the variable `buf2R1` is freed, which also makes the memory pointed by the `temp` variable invalid. Therefore, at line 19, the use of `temp` variable with the `strncpy` function is a  use-after-free bug.

## Example 2

```cpp
1. #include <stdio.h>
2. #include <unistd.h>
3. #include <stdlib.h>
4. #include <string.h>

5. char *retptr(void* l) {
6.  char p ,*q ;
7.  q = l ;
8.  return q ; /* deallocation on the stack */
9.}

10. int main( ){
11.  char *a , *b;
12.  int i ;
13.  a = malloc(16) ;
14.  b = a + 5;
  
15.  b = retptr(b) ;
16.  free(a) ;

17.  *b = 'c' ; /* use after free */
18.}
```

For this example, the pass should return the following:
```shell
Use after free bug at line: 17
```

### Explanation:

The pointer returned by the function `retptr` is the same as the one given in the input to the function. Therefore, at line 15, the value of `b` will remain unchanged. Note that the pointer `b` is derived from the pointer `a`, so after freeing the memory pointed by `a, the pointer dereference at line 17 becomes invalid i.e., a use-after-free bug.

## Example 3

```cpp
1. #include <stdio.h>
2. #include <unistd.h>

3. #define BUFSIZE1    512
4. #define BUFSIZE2    ((BUFSIZE1/2) - 8)

5. int main(int argc, char **argv) {
6.  char *buf1R1;
7.  char *buf2R1;
8.  char *buf1R2;

9.   buf1R1 = (char *) malloc(BUFSIZE2);
10.  buf2R1 = (char *) malloc(BUFSIZE2);

11.  free(buf1R1);
12.  free(buf2R1);

13.  buf1R2 = (char *) malloc(BUFSIZE1);
14.  strncpy(buf1R2, "Hello", BUFSIZE1-1);

15.  free(buf2R1); /* Double free bug */
16.  free(buf1R2);
17.}
```

For this example, the pass should return the following:
```shell
Double free bug at line: 15
```

### Explanation:

`buf2R1` is getting freed twice at line 12 and 15.

### Approach:

Here's what you have to do:
1. Keep track of all the heap memory locations that are allocated, reallocated, and freed.
2. Instrument the instructions (`load`, `store`, `call`, `GetElementPointer`) that manipulates or uses a pointer and check if the pointer variable used in the instruction points to a heap memory or stack memory.
3. For pointers pointing to the heap memory, check if they are pointing to a valid heap memory location or not. If not, print the type of bug and immediately terminate the program through the `exit(1)` function call.

##### How to keep track of the heap memory locations that are getting allocated or deallocated during the program runtime?

Here's what you can do:
1. Make your custom heap memory allocators and deallocators. For example, `mymalloc`, `mycalloc`, and `myfree`.
2. Your LLVM pass should replace all the function calls of `malloc` with that of `mymalloc`. Similarly, `calloc` with `mycalloc`, and `free` with `myfree`.
3. Make a **shared library** containing the definition of `mymalloc`, `mycalloc`, and `myfree` functions, which will just be a wrapper over the original heap allocators. For example,

```cpp
interval_tree<int64_t> it; // interval tree to store all the valid heap memory locations

void* mymalloc (size_t size) {
    void* ret = malloc(size);
    if (ret != NULL)
        it.push_back({ret, ret + size}); // store [ret, ret + size] interval into the tree
    return ret;
}
```

4. Similarly, during `myfree` function, first use the interval tree to check if the received pointer points to a valid memory or not; if the access is valid, remove that interval from the interval tree.

Note that the **shared library** is responsible for tracking the heap memory used by the program. Through the LLVM pass, you will add function calls into the shared library to check if the pointer access is a valid heap memory access or not.

##### How to find out if a memory address belongs to the heap memory or the stack memory?
Well, that's pretty easy. Try to figure it out yourself!

##### How to instrument the instructions that use or manipulate a pointer?

The objective of instrumenting pointer manipulating instructions is to dynamically check if the pointer being used points to a valid heap memory location or not.

By instrumenting, we mean that you have to **inject** a function call, say `validatePointer(void*)` before the instruction, and pass the pointer as an argument to this function. The definition of `validatePointer()` function will be in the shared library that will check if the pointer points to valid heap memory.

#### Optimization:

You can optimize the above-mentioned approach by selectively instrumenting the instructions that manipulate or uses pointer(s). Here's how you can do that;
1. Through static analysis, find the pointers and their aliases that are (1) returned from the `malloc()` or `calloc()` functions OR (2) passed as an argument into the `free()` function.
2. Now, selectively instrument the instructions that use or manipulate the pointers obtained from the previous step.

This optimization will reduce the number of instrumentations and will thus, have a lower runtime performance overhead.

#### Deliverable(s):

1. A single `.cpp` file containing the source code of your LLVM analysis pass. Please name this file as `Assignment3.cpp`. Also, please register the pass as `heapbugs` as done below.

```C++
static RegisterPass<Assignment3> X("heapbugs",
                                   "Pass to find heap memory bugs");
```

2. Source code of your shared library that tracks and checks the usage of heap memory. Please name this file as `HeapChecker.cpp`.

3. Two test `.c` files named `Test1.c`, `Test2.c` that consist of two unique test cases (scenarios) not described in the examples that test your analysis pass. Include a brief comment at the top of each source file describing what scenario you are testing.

4. Any necessary header files (i.e., interval tree implementation) to run your solution. These will not be marked, however.

## Evaluation Rubric

Your submission will be evaluated on four test cases. Two test cases are visible, and the other two test cases will be hidden.

```
40% - Visible test cases
10% - Hidden test case #1
10% - Hidden test case #2
20% - Your unique test cases (How unique are your test cases from one another? Are they unique from the examples we've shown? Do they demonstrate that you have considered other scenarios?)
10% - Whether or not you are using the above-mentioned optimization for selectively instrumenting the instructions.
10% - Quality of your code (Proper indentation, consistent and descriptive naming schemes, appropriate comments, and remove unnecessary code)
```

## Working with the shared library
You can use the following command to compile a shared library:
```shell
$LLVM_BUILD_DIR/bin/clang++ --shared -fPIC HeapChecker.cpp -o HeapChecker.so
```
Notice the use of `--shared -fPIC` options.

You can make the shared library either in C or C++. If using C++, make sure to wrap the exported functions with the  `extern "C"` construct. This will make sure that your shared library can also be used with applications (Test cases) written in C. [https://stackoverflow.com/questions/2587613/what-is-the-effect-of-declaring-extern-c-in-the-header-to-a-c-shared-libra](https://stackoverflow.com/questions/2587613/what-is-the-effect-of-declaring-extern-c-in-the-header-to-a-c-shared-libra)


## Hint

You may reuse any template code or helper functions provided in the previous assignments.
As per the course policy, please add a comment to cite any code you use, including code from your previous assignments.

You may find it useful to reuse the alias analysis pass in LLVM for the selective instruction instrumentation  optimization.
To call another LLVM pass from your own pass, add the following method under your pass
```cpp
void getAnalysisUsage(AnalysisUsage &AU) const override {
  AU.addRequired<AAResultsWrapperPass>();
  AU.setPreservesAll();
}
```
and add the following include statements to your header.
```cpp
 #include "llvm/Analysis/AliasAnalysis.h"
```

## Running your pass on source code

1. Compile the C++ source code to readable IR (.ll) format. You will want to compile it such that it preserves the original variable names.
```bash
clang -O0 -g -emit-llvm -o HelloWorld.ll -c HelloWorld.c
```

2. Run your LLVM pass using `opt` on the IR file. Name the instrumented IR file differently for easier debugging.
```bash
opt -S -enable-new-pm=0 -load $PATH_TO_PASS -heapbugs < HelloWorld.ll > HelloWorld_Instr.ll
```

3. Build the shared library. We recommend that you build the shared library in a separate folder (e.g., `$SHARED_LIB_PATH`).
```bash
cd $SHARED_LIB_PATH
clang++ -shared -fPIC HeapChecker.cpp -o HeapChecker.so
```

4. Return to your test program directory. Link the shared library file with your instrumented IR file to produce an executable.
```bash
clang HelloWorld_Instr.ll $SHARED_LIB_PATH/HeapChecker.so -o HelloWorld
```

## Submission Instructions

#### Notes:
- Please push your deliverables in the **A3_Submit** folder in the **master** branch by the deadline.
- Do not rename the submission folder.
- Points will be deducted if you have more deliverables than requested.

## Deadline:

A snapshot of your submission folder will be automatically taken by the following deadline:

- Friday, March 15, 2024 23:59:00 PST

