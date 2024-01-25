//===- Pass2.cpp - LLVM Tutorial ------------------------------------------===//
//=== "Instruction Counting"
//===----------------------------------------------------------------------===//

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;


namespace {
  struct P2Comp : public FunctionPass {
    static char ID;
    P2Comp() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      unsigned int count = 0;

      for (Function::const_iterator BB = F.begin(); BB != F.end(); ++BB) {
        for (BasicBlock::const_iterator Ins = BB->begin(); Ins != BB->end(); ++Ins) {
          const Instruction *I = &*Ins;
          if (isa<CmpInst>(I)) {
            ++count;
          }
        }
      }
      errs() << "Number of Comparison instructions: " << count << "\n";
      return false;
    }
  };
}

char P2Comp::ID = 0;
static RegisterPass<P2Comp> X("p2comp", "Count Comparison Instructions");

