#pragma once
#include <FunctionAnalysisBase.h>
#include <iostream>
class SelectionCountPass : public FunctionAnalysisBase {
private:
    int SelectionCount;
    static char ID;

public:
    bool run(llvm::Function& F) override ;
    void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
};