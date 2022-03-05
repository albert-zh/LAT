#pragma once
#include <FunctionAnalysisBase.h>
#include <iostream>
class FunctionCallPass : public FunctionAnalysisBase {
private:
    std::vector<std::string> callees;
    static char ID;

public:
    bool run(llvm::Function& F) override ;
    void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
};