#pragma once
#include <llvm/IR/Module.h>
#include <FunctionAnalysisBase.h>
#include <iostream>
class ModuleAnalysis {
private:
    rapidjson::StringBuffer strbuf;
    llvm::Module *M;
    std::vector<FunctionAnalysisBase*> analysisPassManager;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> *writer;

public:
    void init(llvm::Module *M);
    void addPass(FunctionAnalysisBase* analysisPass);
    void startFile();
    bool analyze();
    void endFile();
    void emitResult();
};