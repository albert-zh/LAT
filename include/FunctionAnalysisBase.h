#pragma once
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <llvm/IR/Function.h>

class FunctionAnalysisBase {
public:
    virtual bool run(llvm::Function& F) {
        return false;
    }
    virtual void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {}
};