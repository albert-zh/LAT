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


bool BasicBlockCountPass::run(Function& F) {
    BBCount = F.size();
    return true;
}

void BasicBlockCountPass::getResult(PrettyWriter<StringBuffer>& writer) {
    writer.Key("sequence_num");
    writer.Int(BBCount);
}