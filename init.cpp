#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Pass.h>
#include <llvm/IR/LegacyPassManager.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <iostream>

// using namespace std;
using namespace llvm;
using namespace rapidjson;

// LLVM上下文全局变量
static ManagedStatic<LLVMContext> GlobalContext;

// 命令行位置参数全局变量, 这个参数的含义是需要处理的LLVM IR字节码的文件名
static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<filename>.bc"), cl::Required);

// class WriterWrapper {
// private:
//     static StringBuffer strbuf;
//     static Writer<StringBuffer> Writer;
// public
    
// }

class AnalysisBase {
public:
    virtual bool run(Function& F);
    virtual bool run(BasicBlock& BB);
    virtual bool run(Module& M);
    virtual void getResult(Writer<StringBuffer>& Writer);
};

class FunctionCallPass : public AnalysisBase {
private:
    std::vector<std::string> callees;
    static char ID;

public:
    bool run(Function& F) override {
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (CallInst *CallI = static_cast<CallInst*>(&I)) {
                    callees.push_back(CallI->getCalledFunction()->getName().str());
                }
            }
        }
    }

    void getResult(Writer<StringBuffer>& writer) override {
        writer.Key("called_function");
        writer.StartArray();
        for (auto callee : callees) {
            writer.String(callee.c_str());
        }
        writer.EndArray();
    }

};

int main(int argc, char **argv) {
    //  initialize 
    SMDiagnostic Err;
    cl::ParseCommandLineOptions(argc, argv);
    std::unique_ptr<Module> M = parseIRFile(InputFilename, Err, *GlobalContext);
    StringBuffer strbuf;
    Writer<StringBuffer> writer(strbuf);
    if (!M) {
        Err.print(argv[0], errs());
        return 1;
    }

    std::vector<AnalysisBase*> analysis;
    analysis.push_back(new FunctionCallPass());

    for (Function &F : *M) {
        if (!F.isIntrinsic()) {
            // get function name 
            writer.Key("function_name");
            writer.String(F.getName().str().c_str());

            // get called function
            AnalysisBase* Analysis = new FunctionCallPass();
            Analysis->run(F);
            Analysis->getResult(writer);

            DominatorTree *DT = new DominatorTree(F);
            DT->recalculate(F);
            
            LoopInfoBase<BasicBlock, Loop>* loopInfo = new LoopInfoBase<BasicBlock, Loop>();
            loopInfo->analyze(*DT);
            loopInfo->getLoopsInPreorder().size();
        }
    }
    writer.EndArray();

    std::cout<<strbuf.GetString()<<std::endl;

}
