# Assignment 2

## Background

Taint analysis or [taint checking](https://en.wikipedia.org/wiki/Taint_checking) is often used in the security analysis of software.
Tainted data is any data that comes from outside the source code, including stdin and environment variables.
Since they are outside the control of the program, no assumptions should be made about their impact on the program - they can lead to buffer overflows and SQL injection attacks.
A tainted variable is any variable that is affected by tainted data, whether by control flow or data flow.
We define an explicitly tainted variable as one that is impacted by data flow only.

More information about taint analysis can be found here: [Lecture Slides from CMU](https://www.cs.cmu.edu/~ckaestne/15313/2018/20181023-taint-analysis.pdf).

## Objective

Write a LLVM pass to conduct taint analysis on programs.
The pass should print out all the explicitly tainted variables that are in scope at the end of the program.
Additionally, for each tainted variable in scope, print out the line number (corresponding to the source code) where it first becomes tainted.
The line numbers should be printed out in order.
Unlike the last assignment, you will be responsible for inter-procedural analysis and loops / cycles as well.

Assume the following for this assignment:
1. `cin` is the only source of tainted data, and will only appear in the `main()` function.
2. The end of the program is always located at the last instruction of the `main()` function.
3. Only explicit tainting needs to be considered (where variables are impacted by data flow).
4. Tainted variables can become untainted again if they are reassigned untainted values while in straight-line code (not in any conditional branch). For simplicity, once tainted variables become untainted, they will never be used again (or reassigned another value) in another instruction.
5. You only have to consider primitive types - aggregates (i.e., arrays) and pointers do not need to be handled.
6. You only have to consider local variables.
7. All function arguments are passed by value. Also, a function call always returns a tainted value if at least one argument is tainted. Otherwise, a function call always returns an untainted value.
8. Conditionals are neither tautologies (always true) or contradictions (always false).
9. All control flow is structured.
10. You are only responsible for determining variable scope created through control flow statements. Programs will not contain lexical scope blocks created by plain braces ie., `{`, `}`.


## Example 1 (Conditional)

```cpp
1.  #include <iostream>
2.
3.  using namespace std;
4. 
5.  int main() {
6.    int x;        //Tainted = {}
7.
8.    cin >> x;     //Tainted = {x}
9.    int y = 0;    //Tainted = {x}
10.   int z;        //Tainted = {x}
11.   if (x > y) {  //Tainted = {x}
12.     z = x;      //Tainted = {x,z}
13.   } else {      //Tainted = {x}
14.     z = 2;      //Tainted = {x}
15.   }             //Tainted = {x}
16.   cout << z;    //Tainted = {x,z}
17.   return 0;     //Tainted = {x,z}
18. }
```

For this example, the pass should return the following:
```shell
Line 8: x is tainted
Line 12: z is tainted
Tainted: {x,z}
```

### Explanation:

`x` is tainted by `cin` as `cin` can accept any user input. A comparison operation between `x` and `y` in Line 11 does not taint `y`. `z` is tainted by `x` in Line 12, since the value `x`, an unknown user input, is assigned to `z`. At the end of the program, both x and z are tainted.
Note that on Line 14, `z` is also implicitly tainted (via control flow) by `x` due to the conditional in Line 11, but we are only interested in **explicit tainting** (via data flow) in this assignment.

## Example 2 (FunctionCall)

```cpp
1.  #include <iostream>
2.
3.  using namespace std;
4.  
5.  int add_func(int a, int b) {
6.    return a + b;
7.  }
8.
9.  int main() {
10.   int x;                  //Tainted = {}
11.   cin >> x;               //Tainted = {x}
12.   int y = 0;              //Tainted = {x}
13.   int z = add_func(x,y);  //Tainted = {x,z}
14.   cout << z;              //Tainted = {x,z}
15.   z = 3;                  //Tainted = {x}
16.   return 0;               //Tainted = {x}
17. }
```

For this example, the pass should return the following:
```shell
Line 11: x is tainted
Line 13: z is tainted
Line 15: z is now untainted
Tainted: {x}
```

### Explanation:

`x` is tainted by `cin` as `cin` can accept any user input. `z` is tainted by `x` in Line 13 since the value `x`, a tainted value, is added to `y` in the function `add_func`, and then assigned to z. However, at Line 15, `z` is assigned to an untainted constant, so `z` is no longer tainted. At the end of the program, only `x` is tainted.

## Running your pass on source code

1. Compile the C++ source code to readable IR (.ll) format. You will want to compile it such that it preserves the original variable names.
```bash
clang++ -O0 -g -S -emit-llvm -fno-discard-value-names -o Conditional.ll -c Conditional.cpp
```

2. Run your LLVM pass using `opt` on the IR file.
```bash
opt -enable-new-pm=0 -load $PATH_TO_PASS -taintanalysis < Conditional.ll > /dev/null
```

## Evaluation Rubric

Your submission will be evaluated on four test cases. Two test cases are visible, and the other two test cases will be hidden.

```
25% - Visible test case #1
25% - Visible test case #2
10% - Hidden test case #1
10% - Hidden test case #2
20% - Your unique test cases (How unique are your test cases from one another? Are they unique from the examples we've shown? Do they demonstrate that you have considered other scenarios?)
10% - Quality of your code (Proper indentation, consistent and descriptive naming schemes, appropriate comments, and remove unnecessary code)
```

## Hint

You may reuse any template code or helper functions provided in the previous assignment.
As per the course policy, please add a comment to cite any code you use, including code from your previous assignments.

You may find it useful to check the Dominator Tree and the Post Dominator Tree.
To get the Dominator tree, call the `DominatorTreeWrapperPass`, and then use the `getDomTree()` method to get a `DominatorTree` object.
Similarly, you can call the `PostDominatorTreeWrapperPass`, and then use the `getPostDomTree()` method to get a `PostDominatorTree` object.

To call another LLVM pass from your own pass, add the following method under your pass
```cpp
void getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<DominatorTreeWrapperPass>();
  AU.addRequired<PostDominatorTreeWrapperPass>();
  AU.setPreservesAll();
}
```
and add the following include statements to your header.
```cpp
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
```

## Submission Instructions

#### Deliverable(s):

1. A single `.cpp` file containing the source code of your LLVM analysis pass. Please name this file as `Assignment2.cpp`. Also, please register the pass as `taintanalysis` as done below.

```C++
static RegisterPass<Assignment2> X("taintanalysis",
                                   "Pass to find tainted variables");
```

2. Three test `.cpp` files named `Test1.cpp`, `Test2.cpp`, `Test3.cpp` that consist of three unique test cases (scenarios) not described in the examples that test your analysis pass. Include a brief comment at the top of each source file describing what scenario you are testing.

#### Notes:
- Please push your deliverables in the **A2_Submit** folder in the **master** branch by the deadline.
- Do not rename the submission folder.
- Points will be deducted if you have more deliverables than requested.

## Deadline:

A snapshot of your submission folder will be automatically taken by the following deadline:

- Friday, Feb 16, 2024 23:59:00 PST

