#pragma once
#include <FunctionAnalysisBase.h>
#include <iostream>
class FunctionNamePass : public FunctionAnalysisBase {
private:
    std::string name;

public:
    bool run(llvm::Function& F) override ;
    void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
};