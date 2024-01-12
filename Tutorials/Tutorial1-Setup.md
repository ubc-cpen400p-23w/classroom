# Tutorial 1

## Objectives:

1. To setup up LLVM, Z3, KLEE, AFL, and Dafny development environments.
2. How to write, build, and invoke a custom LLVM pass.

## Prerequisites

1. Basic knowledge of C++, pointers, and STL types.
2. Familiarity with the CMake build system (https://cmake.org/) and Makefiles syntax (Refer the following blog: http://derekmolloy.ie/hello-world-introductions-to-cmake/).
3. Familiarity with the Git CLI. (Refer the following blog: http://guides.beanstalkapp.com/version-control/common-git-commands.html).

## Link to documentation:

- Virtual Box 7.0: https://www.virtualbox.org/
- LLVM 13.0: https://llvm.org/docs/
- GraphViz: https://graphviz.org/
- Z3: https://github.com/Z3Prover/z3
- KLEE 2.2:   https://klee.github.io/tutorials/
- AFL 2.52b: https://lcamtuf.coredump.cx/afl/, https://github.com/AFLplusplus/AFLplusplus
- Dafny 3.12+ : https://dafny.org/

### The Hard Way:
------------

#### Setting up the development environment:

1. In order to have a common development environment, we would suggest all the students to use Ubuntu 18.04 for the programming assignments. Therefore, if you have a different host operating system, you can use Ubuntu's virtual machine. Please refer to the following tutorial on installing Ubuntu's virtual machine: [How to install Ubuntu 18.04 on VirtualBox](https://linuxhint.com/install_ubuntu_18-04_virtualbox/). Note: Please allocate at least 30GB of hard disk while creating the virtual machine.

2. Run the following commands:
```shell
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install build-essential curl libcap-dev git libncurses5-dev python2-minimal python3-pip unzip libtcmalloc-minimal4 libgoogle-perftools-dev libsqlite3-dev doxygen ninja-build gcc g++ clang-9 llvm-9 llvm-9-dev llvm-9-tools autoconf
pip3 install lit tabulate wllvm
```

3. Download and install CMake 3.22.1 https://cmake.org/download/. After downloading and extracting the pre-built binaries, copy all the content to the /usr/local directory `cp -R * /usr/local/`.

4. Install Ninja build `sudo apt-get install ninja-build`.


#### Setting up LLVM:

1. Run the following commands:
```shell
git clone https://github.com/llvm/llvm-project.git
cd llvm-project && git checkout release/13.x && cd ..
mkdir llvm-project/build
cd llvm-project/build
cmake -G Ninja ../llvm -DLLVM_ENABLE_PROJECTS="tools;clang;compiler-rt"  -DLLVM_TARGETS_TO_BUILD="host"  -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_ENABLE_RTTI=ON -DLLVM_OPTIMIZED_TABLEGEN=ON -DCMAKE_BUILD_TYPE=Release
ninja -j4
sudo ninja install
```

Note that if you encounter link errors while building LLVM, try reducing the number of threads in Ninja. Example: ```ninja -j1```

2. If you want to visualize the control flow graph (CFG) produced by LLVM, install the GraphViz tool.
```shell
sudo apt-get install graphviz
```


#### Setting up Z3 and KLEE:

1. Build and install the [Z3 prover](https://github.com/Z3Prover/z3).
```shell
git clone https://github.com/Z3Prover/z3.git
python scripts/mk_make.py
cd build
make
sudo make install
```

2. Run the following commands in the Z3 folder:
```shell
cd build
mkdir -p ./include
cp ../src/api/*.h ./include/
cp ../src/api/c++/z3++.h ./include/z3++.h
```

3. Run the following commands to install KLEE and KLEE-uClibC:
```shell
git clone https://github.com/klee/klee-uclibc.git
cd klee-uclibc
./configure --make-llvm-lib
make KLEE_CFLAGS="-DKLEE_SYM_PRINTF"
cd ..
git clone https://github.com/klee/klee.git
cd klee && mkdir build && cd build
cmake -DENABLE_SOLVER_Z3=ON -DENABLE_UNIT_TESTS=OFF -DENABLE_SYSTEM_TESTS=OFF -DZ3_INCLUDE_DIRS=$HOME/z3/build/include -DENABLE_TCMALLOC=OFF -DHAVE_Z3_GET_ERROR_MSG_NEEDS_CONTEXT=ON -DENABLE_POSIX_RUNTIME=ON -DENABLE_KLEE_UCLIBC=ON -DKLEE_UCLIBC_PATH=$HOME/klee-uclibc -DLLVMCC=$HOME/llvm-project/build/bin/clang -DLLVMCXX=$HOME/llvm-project/build/bin/clang++ ..
make -j2
sudo make install
```

Modify $HOME to point to your home directory.


4. KLEE should now be installed in your path. Try printing KLEE's version:
```shell
klee --version
```

5. Test KLEE by running it on the [GetSign() function](https://klee.github.io/tutorials/testing-function/)


#### Setting up AFL (AFL++):


1. Grab the AFL++ GitHub repo
```shell
git clone https://github.com/AFLplusplus/AFLplusplus
```

2. Make AFLplusplus
```shell
cd AFLplusplus  &&  make distrib
```

3. Install AFLplusplus
```shell
sudo make install
```

4. Test AFL by running it on the libxml2 library: https://aflplus.plus/docs/tutorials/libxml2_tutorial/


#### Setting up Dafny:

1. Add the Microsoft package repository. Feel free to replace 18.04 in the link if you have a different version of Ubuntu.
```shell
wget https://packages.microsoft.com/config/ubuntu/18.04/packages-microsoft-prod.deb -O packages-microsoft-prod.deb
sudo dpkg -i packages-microsoft-prod.deb
rm packages-microsoft-prod.deb
```

2. Install .NET 6.0 and ASP.NET Core 6.0. Dafny requires an older version of .NET (6.0) in order to work.
```shell
sudo apt-get update && sudo apt-get install -y dotnet-sdk-6.0
sudo apt-get install -y aspnetcore-runtime-6.0
```

3. Install Dafny's prebuilt release 3.12+ [here](https://github.com/dafny-lang/dafny/releases/download/v3.12.0/dafny-3.12.0-x64-ubuntu-16.04.zip). You may use a [newer release](https://github.com/dafny-lang/dafny/releases) of Dafny if you have a different version of Ubuntu.

4. Test installation of Dafny by navigating to the `dafny/` folder and running
```shell
./quicktest.sh
```


### The Easy Way (Optional)
------------

We have already set up an Ubuntu virtual machine with LLVM, KLEE, AFL, and Dafny installed. You can simply download the virtual machine from the following link and import it on VirtualBox. Refer to the following tutorial on [importing virtual machines on VirtualBox](https://docs.oracle.com/cd/E26217_01/E26796/html/qs-import-vm.html "importing Virtual machines on VirtualBox").

Download Virtual Machine [username: cpen, password: root]: [LINK](https://ubcca-my.sharepoint.com/:u:/g/personal/abrahamc_student_ubc_ca/EeLeihI5ip5GrEsEo-tTchYBBEU1KFpqGvPU08Yuddg-bA?e=aQvJjZ) . Please use your UBC email to access the file.

**Note:-**  While importing the virtual machine, please allocate at least 4GB RAM to the VM. Building LLVM requires a considerable amount of RAM.


## Writing LLVM Passes

1. Refer to the QuickStart section of the following tutorial on writing LLVM passes: https://llvm.org/docs/WritingAnLLVMPass.html. For the assignments, you will most likely require only Function and Module passes.

2. Feel free to refer to the HelloWorld (https://llvm.org/docs/WritingAnLLVMPass.html#basic-code-required), OpCodeCounter (https://github.com/banach-space/llvm-tutor#opcodecounter), and InjectFunctionCall (https://github.com/banach-space/llvm-tutor#injectfunccall) example LLVM passes. These example passes provide an overview of how to analyze code and do instrumentation in LLVM. **We highly suggest going through these examples before attempting the assignments.**


## Building and Executing LLVM Passes

1. LLVM 13, by default, supports a new pass manager. We'd suggest using the legacy pass manager as it is much easier to use, and the LLVM HelloWorld tutorial also uses the legacy pass manager. To use the legacy pass manager, specify the `-enable-new-pm=0` while using the OPT tool.

2. After writing the LLVM pass, don't forget to add it to the Makefile (https://llvm.org/docs/WritingAnLLVMPass.html#setting-up-the-build-environment) and rebuild the LLVM.

3. After building successfully, you can find your LLVM pass shared library under the "llvm-project/build/lib/" folder.

4. Now, you can simply invoke your pass by using the OPT tool. For instance:
```shell
~/llvm-project/llvm/lib/Transforms/Hello$ ../../../../build/bin/opt -load ../../../../build/lib/LLVMHello.so -enable-new-pm=0 -hello2 < Hello.bc >/dev/null
```

