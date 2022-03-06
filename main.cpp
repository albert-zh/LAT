#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <FunctionAnalysisBase.h>
#include <stdlib.h>
#include <PassDefinitions.h>
#include <ModuleAnalysis.h>
#include <iostream>

// using namespace std;
using namespace llvm;
using namespace rapidjson;

static ManagedStatic<LLVMContext> GlobalContext;

static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("Input file name (default to llvm ir)\n"), cl::Required);

static cl::opt<std::string> InputFileFormat(cl::Positional, cl::desc("format of input file (C)\n"), cl::Optional);

static cl::opt<bool> DisableCalleeCount("disable-callee-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis\n"),
                                        cl::init(false));

static cl::opt<bool> DisableBranchCount("disable-branch-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis\n"),
                                        cl::init(false));

static cl::opt<bool> DisableLoopCount("disable-loop-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis\n"),
                                        cl::init(false));

static cl::opt<bool> DisableBBCount("disable-BB-count", cl::Hidden,
                                        cl::desc("Disable callee count anslysis\n"),
                                        cl::init(false));

static cl::opt<bool> DisableFunctionName("disable-function-name", cl::Hidden,
                                        cl::desc("Disable callee count anslysis\n"),
                                        cl::init(false));

int main(int argc, char **argv) {
    //  parse arguments
    cl::ParseCommandLineOptions(argc, argv);
    SMDiagnostic Err;
    std::string fileName = InputFilename;

    if (InputFileFormat.compare("C") == 0) {
        std::string command("clang -S -emit-llvm " );
        command.append(InputFilename);
        int returnvalue = system(command.c_str());
        if (returnvalue) {
            outs() << "Error: Fail to open c file! \n";
            return returnvalue;
        }
        fileName = InputFilename.substr(0, InputFilename.length() - 2).append(".ll");
    }

    std::unique_ptr<Module> M = parseIRFile(fileName, Err, *GlobalContext);
    if (!M) {
        outs() << "Error: Fail to open ir file! \n";
        return 1;
    }

    // new a basic analysis module
    ModuleAnalysis moduleAnalysis;
    moduleAnalysis.init(M.get());

    // add analysis pass as you want
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

    // run analysis pass
    moduleAnalysis.analyze();

    // print result
    moduleAnalysis.emitResult();

}
