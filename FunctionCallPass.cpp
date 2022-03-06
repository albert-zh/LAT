#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Analysis/LoopInfo.h>
#include <PassDefinitions.h>
#include <iostream>

using namespace llvm;
using namespace rapidjson;


bool FunctionCallPass::run(Function& F) {
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            if (CallInst *CallI = static_cast<CallInst*>(&I)) {
                if (CallI->getCalledFunction())
                    callees.insert(CallI->getCalledFunction()->getName().str());
            }
        }
    }
    return true;
}

void FunctionCallPass::getResult(PrettyWriter<StringBuffer>& writer) {
    writer.Key("called_function");
    writer.StartArray();
    for (auto callee : callees) {
        writer.String(callee.c_str());
    }
    writer.EndArray();
}