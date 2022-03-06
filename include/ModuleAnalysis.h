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

    void endFile();
    void startFile();

public:
    void init(llvm::Module *M);
    void addPass(FunctionAnalysisBase* analysisPass);
    bool analyze();
    void emitResult();
};