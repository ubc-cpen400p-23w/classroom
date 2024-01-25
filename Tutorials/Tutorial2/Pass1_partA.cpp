//===- Pass1.cpp - LLVM Tutorial ------------------------------------------===//
//=== "A LLVM Hello World in Two Ways"
//===----------------------------------------------------------------------===//

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;


namespace {
  struct P1Function : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    P1Function() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      errs() << "Hello: ";
      errs() << F.getName() << '\n';
      return false; // Return true for transformation passes; otherwise return false
    }
  };
}

char P1Function::ID = 0;
static RegisterPass<P1Function> X("p1function", "Hello World Function Pass");

