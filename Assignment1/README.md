
# Assignment 1

## Objective

Write a LLVM pass for intra-procedural analysis to find usages of undefined local variables. The pass should print the function name along with **all** the line numbers (corresponding to the source  code) at which an undefined variable(s) is(are) used.

For simplicity, you are not required to check for undefined variables that are both declared and used within a cycle.

## Example 1 (HelloWorld)

```cpp
1. #include <iostream>
2.
3. using namespace std;
4. 
5. int GetElement1(int arr[]) {
6.  int loc;
7.
8.  int temp = arr[loc + 1];   // Bug#1
9.  return temp + arr[loc];   // Bug#2
10. }
11.
12. int GetElement2(int arr[], int n) {
13.  int loc1;
14.  int loc2 = 1;
15.
16.  if (n > 0) {
17.   loc2 = loc1; // Bug#3
18.  }
19.  return arr[loc2];   // Bug#4
20. }
```

For this example, the pass should return the following:
```shell
GetElement1(int*) : 8
GetElement1(int*) : 9
GetElement2(int*, int) : 17
GetElement2(int*, int) : 19
```

### Explanation:

In the GetElement1() function, the undefined variable is loc. So, the bug is at lines 8 and 9 because they use an undefined variable.
In the GetElement2() function, the undefined variable is loc1. Line 17 uses an undefined variable so, it is a bug. Note that at line 17, the undefined variable is assigned to loc2 variable, so at line 19, the value of loc2 variable can be undefined. Therefore, we mark line 19 as a bug.

## Example 2 (HeapSort)

```cpp
6. void heapify(int arr[], int n, int i)
7. {
8. 	// int largest = i; // Initialize largest as root
9.  	int largest; // BUG!!!
10. 	int l = 2 * i + 1; // left = 2*i + 1
11. 	int r = 2 * i + 2; // right = 2*i + 2
12.
13. 	// If left child is larger than root
14. 	if (l < n && arr[l] > arr[largest])
15. 		largest = l;
16.
17. 	r = largest;
18.
19. 	// If right child is larger than largest so far
20. 	if (r < n && arr[r] > arr[largest])
21. 		largest = r;
22.
23. 	// If largest is not root
24. 	if (largest != i) {
25. 		swap(arr[i], arr[largest]);
26.
27. 		// Recursively heapify the affected sub-tree
28. 		heapify(arr, n, largest);
29. 	}
30. }
```

For this example, the pass should return the following:
```shell
heapify(int*, int, int) : 14
heapify(int*, int, int) : 17
heapify(int*, int, int) : 20
heapify(int*, int, int) : 21
heapify(int*, int, int) : 24
heapify(int*, int, int) : 25
heapify(int*, int, int) : 28
```
### Explanation:

The undefined variable in the heapify() function is `largest`. So, line 14, 17, 20, 24, 25, 28 might use an undefined variable, and therefore, they are buggy. Note that at line 17, the value of an undefined variable is assigned to the variable `r`, and therefore, the value of `r` at line 21 might be undefined, and thus, line 21 is also buggy.

## Running your pass on source code

1. Compile the C++ source code to readable IR (.ll) format.
You may use `build.sh` in the TestCases folder for reference.

```bash
clang++ -O0 -g -S -emit-llvm -o HelloWorld.ll -c HelloWorld.cpp
```

2. Run your LLVM pass on the IR file.
```bash
opt -enable-new-pm=0 -load $PATH_TO_PASS -undeclvar < HelloWorld.ll > /dev/null
```

where `$PATH_TO_PASS` points to the location of the .so file of your LLVM pass.
For example, `$PATH_TO_PASS` may point to `$LLVM_BUILD_DIR/lib/Assignment1.so`.

We pipe the output of the LLVM pass to /dev/null since this is an analysis pass - we are not making any modifications to the LLVM IR (i.e., a transformation / optimization pass).

## Template

For this assignment, we have provided you with a template with functions defined for demangling function names, mapping LLVM IR to the source code, and topologically sorting the Basic Blocks. Feel free to modify the template code as per your wish.

## Evaluation Rubric

Your submission will be evaluated on four test cases. Two test cases are visible (HelloWorld and HeapSort), and the other two test cases will be hidden.

```
25% - Visible test case #1
25% - Visible test case #2
20% - Hidden test case #1
20% - Hidden test case #2
10% - Quality of your code (Proper indentation, consistent and descriptive naming schemes, appropriate comments, and remove unnecessary code)
```

## Hint

```
For this assignment, here's what you have to do:

1. Iterate over all the basic blocks of a function in a topologically-sorted manner.

2. For every basic block, maintain EntrySet and ExitSet. These sets consist of undefined variables. 
   EntrySet is initialized as the union of ExitSets of the current basic block's predecessors. ExitSet is initialized as NULL.

3. Now iterate through all the instructions within the basic block:
  3a. For Alloca Instruction, add it into the Entry Set. By default, we are assuming that all the local variables are undefined.
  3b. For Store Instruction, there can be 2 cases that we care about:
    3ba. If you store any member of the Entry Set into any other variable, add that variable into the Entry Set as well. Mark that line as a bug.
    3bb. If you store a constant or any other value not in the Entry set into a variable in the Entry Set, remove that variable from the Entry Set.
  3c. For Load Instruction, just check if the instruction is loading from a variable that is in the Entry Set, then mark that line as a bug. 
      Also, add the loaded value to the Entry Set.
  3d. For now, Ignore all other instructions.

4. After iterating through all the instructions in the basic block, assign ExitSet = EntrySet
```

## Submission Instructions

#### Deliverable(s):

1. A single `.cpp` file containing the source code of your LLVM analysis pass. Please name this file as `Assignment1.cpp`. Also, please register the pass as `undeclvar` as done in the assignment template code.

```C++
static RegisterPass<Assignment1> X("undeclvar",
                                   "Pass to find undeclared variables");
```

#### Notes:
- Please push your deliverables in the **A1_Submit** folder in the **master** branch by the deadline.
- Do not rename the submission folder.
- Points will be deducted if you have more deliverables than requested.

## Deadline:

A snapshot of your submission folder will be automatically taken by the following deadline:

- Friday, Feb 2, 2024 23:59:00 PST

