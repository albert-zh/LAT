#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/CommandLine.h>

// using namespace std;
using namespace llvm;

// LLVM上下文全局变量
static ManagedStatic<LLVMContext> GlobalContext;

// 命令行位置参数全局变量, 这个参数的含义是需要处理的LLVM IR字节码的文件名
static cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<filename>.bc"), cl::Required);

int main(int argc, char **argv) {

    SMDiagnostic Err;
    // 格式化命令行参数,
    cl::ParseCommandLineOptions(argc, argv);

    std::unique_ptr<Module> M = parseIRFile(InputFilename, Err, *GlobalContext);
    // 错误处理
    if (!M) {
        Err.print(argv[0], errs());
        return 1;
    }
    // 遍历Module中的每一个Function
    for (Function &F:*M) {
        // 过滤掉那些以llvm.开头的无关函数
        if (!F.isIntrinsic()) {
            // 打印函数返回类型
            outs() << *(F.getReturnType());
            // 打印函数名
            outs() << ' ' << F.getName() << '(';
            // 遍历函数的每一个参数
            for (Function::arg_iterator it = F.arg_begin(), ie = F.arg_end(); it != ie; it++) {
                // 打印参数类型
                outs() << *(it->getType());
                if (it != ie - 1) {
                    outs() << ", ";
                }
            }
            outs() << ")\n";
        }
    }

}
