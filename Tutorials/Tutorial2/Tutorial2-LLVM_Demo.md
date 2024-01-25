
# Tutorial 2

## Objectives:

1. Get familiar with the LLVM programming model
2. Write a basic LLVM [analysis] pass


## Prerequisites

1. LLVM should be built and setup.


## Compiling C/C++ Source Code to LLVM IR

```shell
clang++ -O0 -S -emit-llvm -o HelloWorld.ll -c HelloWorld.cpp
```

## Visualization

For visualization (optional but helpful):
```shell
sudo apt-get install graphviz
```

#### Visualizing the Call Graph
```shell
clang++ -O0 -S -emit-llvm -Xclang -disable-O0-optnone -o HelloWorld.ll -c HelloWorld.cpp
opt -disable-output --dot-callgraph -enable-new-pm=0 HelloWorld.ll
dot -Tpng HelloWorld.ll.callgraph.dot -o HelloWorld.callgraph.png
```

#### Visualizing the CFG of a Function
```shell
opt -disable-output --dot-cfg -enable-new-pm=0 HelloWorld.ll
dot -Tpng ._Z3bari.dot -o bar.png
```

## Building Your First LLVM Pass

1. First navigate the directory in your LLVM source folder where the LLVM Transform passes reside. Create an empty folder (e.g. `Tutorial2`) there to store your LLVM passes.
```shell
cd ~/llvm-project/llvm/lib/Transforms
mkdir Tutorial2
```

3. Modify the CMakeLists file to add the following directory.
```cmake
add_subdirectory(Tutorial2)
```

4. Write your LLVM pass (e.g., Pass1.cpp) in the `Tutorial2` directory. You may use this [sample solution to Task #1a below](Pass1_partA.cpp) to get started. Rename this file as `Pass1.cpp`.
```shell
cd Tutorial2
mv Pass1_partA.cpp Pass1.cpp
```

5. Then, add a CMakeLists file in the `Tutorial2` directory with the following content:
```cmake
add_llvm_library( Tutorial2 MODULE
  Pass1.cpp

  PLUGIN_TOOL
  opt
  )
```

6. Now, navigate back to the LLVM Build directory and rerun the build command.
```shell
cd ~/llvm-project/build/
ninja
```

7. Navigate to the directory containing the source file you want to analyze.

8. Finally, use the `opt` tool to run your LLVM pass on the source file to be analyzed.
```shell
opt -enable-new-pm=0 -load $PATH_TO_PASS -[RegisteredNameOfPass] < HelloWorld.ll > /dev/null
```
Example command if the built pass is called `Tutorial2.so` and the pass is registered as `p1function`:
```shell
opt -enable-new-pm=0 -load ~/llvm-project/build/lib/Tutorial2.so -p1function < HelloWorld.ll > /dev/null
```

## Task 1
Analyze [HelloWorld.cpp](HelloWorld.cpp)

Goal: Print all the functions' name...
1. As a function pass [Solution](Pass1_partA.cpp)
2. As a module pass [Solution](Solutions/Pass1.cpp)

## Task 2
Analyze [CompFunc.cpp](CompFunc.cpp)

Goal: Count...
1. All comparison instructions (i.e., ==, >, <) [Solution](Solutions/Pass2_partA.cpp)
2. All the `MUL` instructions [Solution](Solutions/Pass2.cpp)

## Task 3
Analyze [CompFunc.cpp](CompFunc.cpp)

Goal: For each comparison instruction, print out the...
1. Instruction itself
2. $1^{st}$ source operand
3. $2^{nd}$ source operand

[Task 3 Solution](Solutions/Pass3.cpp)

