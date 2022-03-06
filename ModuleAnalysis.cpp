#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <FunctionAnalysisBase.h>
#include <PassDefinitions.h>
#include <ModuleAnalysis.h>
#include <iostream>

using namespace llvm;
using namespace rapidjson;


void ModuleAnalysis::init(Module *M) {
    this->M = M;
    writer = new PrettyWriter<StringBuffer>(strbuf);
}

void ModuleAnalysis::addPass(FunctionAnalysisBase* analysisPass) {
    analysisPassManager.push_back(analysisPass);
}

void ModuleAnalysis::startFile() {
    writer->StartObject();
    writer->Key("function_num");
    writer->Int(M->size());
    writer->Key("function_summary");
    writer->StartArray();
    // writer->StartObject();
}

bool ModuleAnalysis::analyze() {
    startFile();
    for (Function &F : *M) {
        if (!F.isIntrinsic()) {
            writer->StartObject();
            for (auto pass : analysisPassManager) {
                pass->run(F);
                pass->getResult(*writer);
            }
            writer->EndObject();
        }
    }
    endFile();
    return true;
}

void ModuleAnalysis::endFile() {
    // writer->EndObject();
    writer->EndArray();
    writer->EndObject();
}

void ModuleAnalysis::emitResult() {
    std::cout<< strbuf.GetString() <<std::endl;
}
