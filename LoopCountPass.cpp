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


bool LoopCountPass::run(Function& F) {
    DominatorTree* DT = new DominatorTree();
    DT->recalculate(F);
    loopInfo->releaseMemory();
    loopInfo->analyze(*DT);
    LoopCount = loopInfo->getLoopsInPreorder().size();
    return true;
}

void LoopCountPass::getResult(PrettyWriter<StringBuffer>& writer) {
    writer.Key("repetition_num");
    writer.Int(LoopCount);
}