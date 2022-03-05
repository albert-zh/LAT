#pragma once
#include <FunctionAnalysisBase.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <iostream>
class LoopCountPass : public FunctionAnalysisBase {
private:
    int LoopCount;
    llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>* loopInfo;

public:
    bool run(llvm::Function& F) override ;
    void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

    LoopCountPass() : LoopCount(0)  {
    loopInfo = new llvm::LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    }
};