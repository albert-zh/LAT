#pragma once
#include <FunctionAnalysisBase.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <iostream>
class BasicBlockCountPass : public FunctionAnalysisBase {
private:
    int BBCount;
    static char ID;

public:
    bool run(llvm::Function& F) override ;
    void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

    BasicBlockCountPass() : BBCount(0) {}

};