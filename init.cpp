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
#include <rapidjson/prettywriter.h>
#include <iostream>

// using namespace std;
using namespace llvm;
using namespace rapidjson;

// LLVM上下文全局变量
static ManagedStatic<LLVMContext> GlobalContext;

// 命令行位置参数全局变量, 这个参数的含义是需要处理的LLVM IR字节码的文件名
static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<filename>.bc"), cl::Required);

static cl::opt<bool> DisableCalleeCount("disable-callee-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis"),
                                        cl::init(false));

static cl::opt<bool> DisableBranchCount("disable-branch-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis"),
                                        cl::init(false));

static cl::opt<bool> DisableLoopCount("disable-loop-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis"),
                                        cl::init(false));

static cl::opt<bool> DisableBBCount("disable-BB-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis"),
                                        cl::init(false));

static cl::opt<bool> DisableFunctionName("disable-function-name", cl::Hidden,
                                        cl::desc("Disable callee count anslysis"),
                                        cl::init(false));

class FunctionAnalysisBase {
public:
    virtual bool run(Function& F) {
        return false;
    }
    virtual void getResult(PrettyWriter<StringBuffer>& writer) {}
};

class FunctionCallPass : public FunctionAnalysisBase {
private:
    std::vector<std::string> callees;
    static char ID;

public:
    bool run(Function& F) override {
        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                if (CallInst *CallI = static_cast<CallInst*>(&I)) {
                    if (CallI->getCalledFunction())
                        callees.push_back(CallI->getCalledFunction()->getName().str());
                }
            }
        }
        return true;
    }

    void getResult(PrettyWriter<StringBuffer>& writer) override {
        writer.Key("called_function");
        writer.StartArray();
        for (auto callee : callees) {
            writer.String(callee.c_str());
        }
        writer.EndArray();
    }

};

class SelectionCountPass : public FunctionAnalysisBase {
private:
    int SelectionCount;
    static char ID;

public:
    bool run(Function& F) override {
        for (BasicBlock &BB : F) {
            if (!BB.getSingleSuccessor()) {
                SelectionCount++;
            }
        }
        return true;
    }

    void getResult(PrettyWriter<StringBuffer>& writer) override {
        writer.Key("selection_num");
        writer.Int(SelectionCount);
    }

    SelectionCountPass() : SelectionCount(0) {}
};

class FunctionNamePass : public FunctionAnalysisBase {

private:
    std::string name;

public:
    bool run(Function& F) override {
        name = F.getName().str();
        return true;
    }

    void getResult(PrettyWriter<StringBuffer>& writer) override {
        writer.Key("function_name");
        writer.String(name.c_str());
    }

};

class LoopCountPass : public FunctionAnalysisBase {
private:
    int LoopCount;
    LoopInfoBase<llvm::BasicBlock, llvm::Loop>* loopInfo;

public:
    bool run(Function& F) override {
        DominatorTree* DT = new DominatorTree();
        DT->recalculate(F);
        loopInfo->releaseMemory();
        loopInfo->analyze(*DT);
        LoopCount = loopInfo->getLoopsInPreorder().size();
        return true;
    }

    void getResult(PrettyWriter<StringBuffer>& writer) override {
        writer.Key("repetition_num");
        writer.Int(LoopCount);
    }

    LoopCountPass() : LoopCount(0)  {
        loopInfo = new LoopInfoBase<llvm::BasicBlock, llvm::Loop>();
    }
};

class BasicBlockCountPass : public FunctionAnalysisBase {
private:
    int BBCount;
    static char ID;

public:
    bool run(Function& F) override {
        BBCount = F.size();
        return true;
    }

    void getResult(PrettyWriter<StringBuffer>& writer) override {
        writer.Key("sequence_num");
        writer.Int(BBCount);
    }

    BasicBlockCountPass() : BBCount(0) {}
};

class ModuleAnalysis {
private:
    StringBuffer strbuf;
    SMDiagnostic Err;
    std::unique_ptr<Module> M;
    std::vector<FunctionAnalysisBase*> analysisPassManager;
    PrettyWriter<StringBuffer> *writer;

public:
    bool init() {
        M = parseIRFile(InputFilename, Err, *GlobalContext);
        if (!M) {
            return false;
        }
        writer = new PrettyWriter<StringBuffer>(strbuf);
        return true;
    }

    void addPass(FunctionAnalysisBase* analysisPass) {
        analysisPassManager.push_back(analysisPass);
    }

    void startFile() {
        writer->StartObject();
        writer->Key("function_num");
        writer->Int(M->size());
        writer->Key("function_summary");
        writer->StartArray();
        writer->StartObject();
    }

    bool analyze() {
        startFile();
        for (Function &F : *M) {
            if (!F.isIntrinsic()) {
                for (auto pass : analysisPassManager) {
                    pass->run(F);
                    pass->getResult(*writer);
                }
            }
        }
        endFile();
        return true;
    }

    void endFile() {
        writer->EndObject();
        writer->EndArray();
        writer->EndObject();
    }

    void emitResult() {
        std::cout<< strbuf.GetString() <<std::endl;
    }
};

int main(int argc, char **argv) {
    //  initialize 
    cl::ParseCommandLineOptions(argc, argv);

    ModuleAnalysis moduleAnalysis;
    if (!moduleAnalysis.init()) {
        outs() << "Error: Fail to open ir file! \n";
        return 1;
    }

    if (!DisableFunctionName) {
        moduleAnalysis.addPass(new FunctionNamePass());
    }

    if (!DisableCalleeCount) {
        moduleAnalysis.addPass(new FunctionCallPass());
    }

    if (!DisableBranchCount) {
        moduleAnalysis.addPass(new SelectionCountPass());
    }

    if (!DisableLoopCount) {
        moduleAnalysis.addPass(new LoopCountPass());
    }

    if (!DisableBBCount) {
        moduleAnalysis.addPass(new BasicBlockCountPass());
    }
        
    moduleAnalysis.analyze();
    moduleAnalysis.emitResult();

}
