#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Analysis/LoopInfo.h>
#include <PassDefinitions.h>
#include <iostream>

using namespace llvm;
using namespace rapidjson;


bool SelectionCountPass::run(Function& F) {
    for (BasicBlock &BB : F) {
        if (!BB.getSingleSuccessor()) {
            SelectionCount++;
        }
    }
    return true;
}

void SelectionCountPass::getResult(PrettyWriter<StringBuffer>& writer) {
    writer.Key("selection_num");
    writer.Int(SelectionCount);
}