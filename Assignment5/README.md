# Assignment 5

## Background

Fault injection testing is a software testing method where faults are purposely introduced to a system to understand its ability to respond and tolerate errors.
In this assignment, you will be implementing fault injection using LLVM on two different test programs: `PacketSender` and `Quicksort`.

Programs may respond differently under fault injection.
Generally, there are four possible failure outcomes:
- **Benign**: The injected program returns the same correct output, as the original program.
- **Crash**: The injected program throws an error, and fails to complete its execution.
- **Hang**: The injected program is stuck during execution (usually in a loop).
- **SDC**: (Silent Data Corruption), where the injected program completes its execution but returns a different, incorrect output compared to the original program.

Programs that yield more frequent benign outcomes under fault injection are said to be more resilient.
These programs may be intrinsicly resilient or purposely made resilient through the application of software fault tolerance design principles.


## Objective

Write a LLVM pass to conduct software fault injection testing of programs.

You will be injecting three types of faults:
1. Buffer overflow faults in `malloc` and `calloc`
2. Memory exhaustion faults in `malloc` and `calloc`
3. Logical fault (perturbing `sub` instructions to `add`)

These faults will be randomly injected into each test program.

Assume the following for this assignment:
1. `malloc()`, `calloc()` are the only heap memory allocator functions used in the program.

#### Buffer Overflow Bug

Buffers are memory storage regions that temporarily hold data while it is being transferred from one location to another. A buffer overflow (or buffer overrun) occurs when the volume of data exceeds the storage capacity of the memory buffer. As a result, the program attempting to write the data to the buffer overwrites adjacent memory locations.
For example, a buffer for log-in credentials may be designed to expect username and password inputs of 8 bytes, so if a transaction involves an input of 10 bytes (that is, 2 bytes more than expected), the program may write the excess data past the buffer boundary.

```c
int *a = malloc(10); // Allocates 10 bytes normally
int *b = malloc(10); // Allocates 7 bytes only
int *c = malloc(10); // Allocates 10 bytes normally
```

#### Memory Exhaustion Fault

When the Operating System (OS) runs out of memory to allocate, all calls to `malloc` and `calloc` will return `NULL`. This could occur as a result of too many accumulated memory leaks.

```c
int *a = malloc(5); // Returns normally
int *b = malloc(5); // Returns NULL
int *c = malloc(5); // Returns NULL
```

#### Logical Fault

A logical error can occur if a mistake in a program's source code results in incorrect or unexpected behaviour.
It is a type of runtime error that leads a program to produce a wrong output or cause a program to crash.

```c
int result = a - b;  ->  int result = a + b;
```

## Implementation

The following faults should be implemented such that exactly one single fault is injected during each program run.
For example, if buffer overflow is selected, it should only affect a single randomly selected `malloc` instruction, even if the program contains multiple `malloc` instructions.
Another example is logical fault - only a single, random subtraction instruction should be perturbed into a addition instruction.
For memory exhaustion, a random call to `malloc` / `calloc` should return `NULL`, and all subsequent calls to `malloc` / `calloc` should also return `NULL`.

1. Buffer Overflow - Underallocate the size of the memory returned by `malloc` and `calloc` by 30%.

2. Memory Exhaustion - Set the memory returned by `malloc` and `calloc` to `NULL`. All subsequent memory accesses should also return `NULL`.

3. Logical Fault - Perturb `sub` instructions into `add` instructions.

Your LLVM pass should accept an integer fault type as a command line argument.
This command line argument must be named `fault-type`.
For example, `opt ... -fault-type 2` means that the injected fault type is memory exhaustion.


## Tasks

We breakdown the steps for your fault injection analysis.
Please describe your findings in a written PDF document.
At the end of this assignment, you should have a total of **8 pie graphs**.

1. Describe a real-life scenario behind each of the three injected faults. What is a scenario in which you would encounter a buffer overflow? Memory Exhaustion? A logical fault? (5 marks)

2. For each test program, and each fault type, measure the number of percentage of Benigns, Crashes, Hangs, and SDCs, after performing fault injection for 1000 times. You will want to automate the fault injection process in a shell script. Report the results in a pie graph (the four above possible failure outcomes) for each fault type - you should have 6 separate pie graphs in total. (10 marks)

3. Measure and report the average percentage runtime overhead of the fault injected programs over the original programs. Do this for two test programs, and for each fault type, separately. (5 marks)

4. Modify the `PacketSender.c` file to reduce error cases caused by buffer overflow injection. (Hint: The modifications should be minor. Can you find a one-line workaround?) Submit this modified source code file. Explain what you added/modified/removed. Rerun the fault injection experiments for buffer overflow only, and report the new results in a new pie graph (by failure outcome). Do you observe any differences in failure outcomes after fixing the bug(s)? What is the runtime overhead of your (more fault tolerant) modified program compared to the original program? (10 marks)

5. Modify the `Quicksort.cpp` file to reduce error cases caused by logical faults. Submit this modified source code file. Explain what you added/modified/removed. Rerun the fault injection experiments for logical faults only, and report the new results in a new pie graph (by failure outcome). Do you observe any differences in failure outcomes after fixing the bug(s)? What is the runtime overhead of your (more fault tolerant) modified program compared to the original program? (10 marks)


#### Deliverable(s):

1. A single `.cpp` file containing the source code of your LLVM analysis pass. Please name this file as `Assignment5.cpp`. Also, please register the pass as `faultinjection` as done below.

```C++
static RegisterPass<Assignment5> X("faultinjection",
                                   "Pass to inject faults");
```

2. A single `.cpp` file containing the source code of your shared library that performs dynamic fault injection. Please name this file as `RuntimeFI.cpp`.

3. A written report `report.pdf` file containing your responses to the tasks above.

4. Modified source code files for `PacketSender.c` and `Quicksort.cpp`.


## Evaluation Rubric

```
25% - Fault Injection Pass
25% - Runtime Shared Library
40% - Written Report (containing your responses to each task)
10% - Quality of your code (Code quality will not be evaluated on the two modified source code files)
```


## Running your pass on source code

The following assumes that your test source file is named `Quicksort.cpp`.

1. Compile the C++ source code to readable IR (.ll) format. You will want to compile it such that it preserves the original variable names and includes debugging information.
```bash
clang++ -O0 -g -S -emit-llvm -fno-discard-value-names -o Quicksort.ll -c Quicksort.cpp
```

2. Run your LLVM pass using `opt` on the IR file.
```bash
opt -S -enable-new-pm=0 -load $PATH_TO_PASS -faultinjection -fault-type $N < Quicksort.ll > QuicksortFI.ll
```

You can use the following command to compile a shared library:
```bash
clang++ --shared -fPIC RuntimeFI.cpp -o RuntimeFI.so
```
Notice the use of `--shared -fPIC` options.

Please make the shared library in C++, and make sure to wrap the exported functions with the  `extern "C"` construct. This will make sure that your shared library can also be used with applications (Test cases) written in C. [https://stackoverflow.com/questions/2587613/what-is-the-effect-of-declaring-extern-c-in-the-header-to-a-c-shared-libra](https://stackoverflow.com/questions/2587613/what-is-the-effect-of-declaring-extern-c-in-the-header-to-a-c-shared-libra)

3. Link the shared library to the IR file, resulting in an fault injected executable.
``` bash
clang++ QuicksortFI.ll RuntimeFI.so -o QuicksortFI
```

4. Run the fault injected program.
``` bash
./QuicksortFI
```

## Hint

You may reuse any template code or helper functions provided in the previous assignments.
As per the course policy, please add a comment to cite any code you use, including code from your previous assignments.

To guide your runtime injection, you may want to print some statistics out to a file in your LLVM pass.
Then, read from this statistics file in your runtime injection library.


## Submission Instructions

#### Notes:
- Please push your deliverables in the **A5_Submit** folder in the **master** branch by the deadline.
- Do not rename the submission folder.
- Points will be deducted if you have more deliverables than requested.

## Deadline:

A snapshot of your submission folder will be automatically taken by the following deadline:

- Friday, Apr 12, 2024 23:59:00 PST

