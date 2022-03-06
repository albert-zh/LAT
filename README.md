# LAT
LAT（LLVM Analysis Tool） is a static analysis tool based on LLVM.

## 1. 构建
### 1.1 构建依赖
1. clang 10.0.0
2. llvm  10.0.0
3. cmake 3.10
4. make 4.1
5. rapidjson (已包含在include路径下)

### 1.2 构建命令

```
mkdir build
cd build
cmake ../
make
```

### 1.3 执行

1. 执行需要指定对应的分析文件
```
./LAT ../test/hello.ll
```

2. 如果输入文件不是LLVM IR文件，需要指定文件语言类型
```
./LAT ../test/hello.c C
```

## 2. 框架
1.  `LAT`框架模仿了`LLVM`优化的模块化设计，每一个分析结果都由一个分析`Pass`类负责，每一个分析`Pass`都继承了同一个虚基类`FunctionAnalysisBase`。
```c++
class FunctionAnalysisBase {
public:
    /// run analysis
    virtual bool run(llvm::Function& F) {
        return false;
    }
    virtual void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {}
};
```

2. 要求每一个`Pass`都要实现`FunctionAnalysisBase`的两个虚函数，比如：
```c++
class FunctionNamePass : public FunctionAnalysisBase {
private:
    std::string name;

public:
    bool run(llvm::Function& F) override ;
    void getResult(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
};

bool FunctionNamePass::run(Function& F) {
    name = F.getName().str();
    return true;
}

void FunctionNamePass::getResult(PrettyWriter<StringBuffer>& writer) {
    writer.Key("function_name");
    writer.String(name.c_str());
}
```

3. 通过`ModuleAnalysis`来对`Pass`进行管理，里边包含了一个`Pass`的容器`analysisPassManager`。
```c++
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
```

4. 基本用法
```c++
/// 创建一个新的ModuleAnalysis对象
ModuleAnalysis moduleAnalysis;

/// 使用一个Module初始化ModuleAnalysis对象
moduleAnalysis.init(M.get());

/// 按需添加相应的Pass
moduleAnalysis.addPass(new FunctionNamePass());

/// 执行分析
moduleAnalysis.analyze();

/// 输出结果
moduleAnalysis.emitResult();

```

## 3. LAT基本原理

### 3.1 LLVM IR

#### 3.1.1 表示形式
1. LLVM IR有bytecode(.bc)，文本(.ll)和内存三种表示形式，这三种表示形式是等价的。以下是一个简单的.ll文件中函数的示例。
```llvm
; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func2(i32 %0, i32 %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %6 = call i32 @func()
  store i32 %6, i32* %5, align 4
  %7 = load i32, i32* %5, align 4
  %8 = load i32, i32* %3, align 4
  %9 = mul nsw i32 %7, %8
  %10 = load i32, i32* %4, align 4
  %11 = mul nsw i32 %9, %10
  ret i32 %11
}
```

2. LLVM IR在内存数据结构中存在`Module, Function, BasicBlock, Instruction`结构，互相之间呈现包含关系。
```
Module
    -Function
        -BasicBlock
            -Instruction
        -BasicBlock
    -Function
        ...

```

3. `Module`对应的是一个`Compile Unit`也就是一个源文件，`Function`对应的是源文件中的一个函数，`BasicBlock`对应的是函数中的一段顺序代码，`Instruction`就对应的是源代码中相关代码的指令。 

4. 每个`BasicBlock`都会记录自己的前继节点和后继节点。  

### 3.2 当前Pass的实现

#### 3.2.1 FunctionNamePass
`FunctionNamePass`返回了函数的函数名，这是LLVM在解析IR文件到内存数据结构时自动保存的信息，直接通过API获取。

#### 3.2.2 BasicBlockCountPass
`BasicBlockCountPass`返回了函数中顺序代码块的数量。在LLVM中，顺序代码块以`BasicBlock`的形式保存在`Function`的容器中，直接获取容器容量。

#### 3.2.3 FunctionCallPass
`FunctionCallPass`返回了被调函数的列表。遍历一个`Function`所有的`Instruction`，找到`Call`指令并记录被调函数的函数名。

#### 3.2.4 SelectionCountPass
`SelectionCountPass`返回了函数分支结构的个数。遍历`Function`所有的`BasicBlock`,如果`BasicBlock`存在多个后继则存在一个分支结构，统计具有多个后继的`BasicBlock`的数量。

#### 3.2.5 LoopCountPass
`LoopCountPass`返回了函数循环结构的个数。`Function`中并没有显示的表征出`Loop`结构，但是可以根据`Function`构建`DominatorTree`并根据`DominatorTree`的信息计算出`Loop`信息。

### 3.3 基本算法原理

#### 3.3.1. `BasicBlock`的划分及`CFG`构建
1. 确定`BasicBlock`的头尾指令
    1.1. 将分支跳转指令作为`BasicBlock`的最后一条指令
    1.2. 将分支跳转到的第一条指令或者`Function`的第一条指令作为`BasicBlock`的头指令
2. 遍历所有的尾指令，为尾指令所在的`BasicBlock`创建到对应的头指令所在的`BasicBlock`的边。

#### 3.3.2. `DominatorTree`
> 《编译原理》
如果每一条从流图的入口结点到结点 n 的路径都经过结点 d, 我们就说 d 支配（dominate）n，记为 d dom n。

根据上述定义，对于`Function`中所有的`BasicBlock`我们都可以找到它对应的支配节点，如果两个`BasicBlock`间存在支配关系就认为这两个节点在`DominatorTree`上存在一条边。

公式
$
  DOM(n) = n \cup \{\cap _ {m\in pre(n)} |DOM(m) \}
$

#### 3.3.3. 根据`DominatorTree`寻找`Loop`

后序遍历`DominatorTree`，如果一个节点存在一条到它的`Dominator`的边，则这条边就是`Loop`的回边，也就找到了一个`Loop`。


## 4. 实现效果

### 4.1. 要求
1. 基于LLVM IR实现结构分析
2. 可以通过不同的语言前端将相关文件转换成LLVM IR后进行分析。
3. 开发者可以通过继承`FunctionAnalysisBase`实现自定义的分析过程。通过`ModuleAnalysis`自由配置实现的分析过程。
4. 生成的报告如下。

### 4.2. /test/hello.ll的分析输出

```json
{
    "function_num": 3,
    "function_summary": [
        {
            "function_name": "func",
            "called_function": [],
            "selection_num": 2,
            "repetition_num": 2,
            "sequence_num": 9
        },
        {
            "function_name": "func2",
            "called_function": [
                "func"
            ],
            "selection_num": 2,
            "repetition_num": 0,
            "sequence_num": 1
        },
        {
            "function_name": "getResult",
            "called_function": [
                "func",
                "func2"
            ],
            "selection_num": 3,
            "repetition_num": 0,
            "sequence_num": 4
        }
    ]
}

```


