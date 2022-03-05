#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <FunctionAnalysisBase.h>
#include <PassDefinitions.h>
#include <iostream>

using namespace llvm;
using namespace rapidjson;


bool FunctionNamePass::run(Function& F) {
    name = F.getName().str();
    return true;
}

void FunctionNamePass::getResult(PrettyWriter<StringBuffer>& writer) {
    writer.Key("function_name");
    writer.String(name.c_str());
}