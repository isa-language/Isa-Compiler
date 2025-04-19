/**
 * Isa to compiler parser in code LLVM.
 * */
#pragma once 
#include "flags.hpp"
#include "frontend/utils/initialized_flags.hpp"
#include <algorithm>
#ifndef isaLLVMPARSER
#define isaLLVMPARSER
#include <llvm/MC/TargetRegistry.h>
#include "frontend/lexer/token.hpp"
#include <iostream>
#include <utility>
/* includes header LLVM */
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/GenericLoopInfo.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
//#include <llvm/Support/Host.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Error.h>
#include <llvm/TargetParser/Host.h> 
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
/* includes libs c */
#include <memory>
#include <string>
#include <system_error>
#include <vector>
/* includes files project */
#include "ast.hpp"
#include <termcolor/termcolor.hpp>
#include <indicators/progress_spinner.hpp>

using namespace indicators;

#include "llvm/IRReader/IRReader.h" 
#include "llvm/Linker/Linker.h"
#include "llvm/Support/SourceMgr.h"




class IsaLLVM {
  public:
    IsaLLVM() {
    
    initModuleLLVM();
    //externFunctions();
  }
  /**
   * Execute program
   **/
  /*
  void exec(std::vector<std::unique_ptr<ASTNode>> program) {
  **
    * 1 parser compiler AST
    *
    * 2 codegen compiler LLVM
    *
    * compiler(AST)
    **

    // 2 -> codegen 
    compiler();
    module->print(llvm::outs(), nullptr);
    // * salve module IR to file 
    salveModuleFile("out.ll");
  }
  */
  llvm::IRBuilder<>& getBuilder() {
    return *builder;
  }
  void exec(std::vector<std::unique_ptr<ASTNode>> program, std::string outputfile) {
    this->output = outputfile;
    compiler(std::move(program));

    //module->print(llvm::outs(), nullptr);
    saveModuleFile((output.empty() ? "out.ll" : output));
    exec_program();
    //generateExecutable("out.o");
  }
/*
  void exec(std::vector<std::unique_ptr<ASTNode>> program, const std::string& filename) {
    //compiler(std::move(program));
    module->print(llvm::outs(), nullptr);
    salveModuleFile(filename);
  }
 */
  private:
  std::string output;
  /* compiler run */
void compiler() {
    indicators::ProgressSpinner spinner{
        indicators::option::PostfixText{"Compilando código Isa! 😼"},
        indicators::option::ForegroundColor{indicators::Color::yellow},
        indicators::option::SpinnerStates{std::vector<std::string>{"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"}},
        indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
    };
    LLVMCodeGenVisitor visitor(&getBuilder(), context.get(), module.get());
    std::vector<std::unique_ptr<ASTNode>> program;
    std::vector<std::unique_ptr<VariableDeclarationNode>> printlnpar;
    printlnpar.push_back(std::make_unique<VariableDeclarationNode>("str", "string", std::make_unique<StringLiteralNode>("\"\\n\"")));
    auto printfDecl = std::make_unique<FunctionInstantiationNode>("printf", "i32", std::move(printlnpar), true);
    program.push_back(std::move(printfDecl));

    std::unique_ptr<FunctionNode> function; 
    std::vector<std::unique_ptr<VariableDeclarationNode>> functionParams;

    functionParams.push_back(std::make_unique<VariableDeclarationNode>("argc", "i32"));
    functionParams.push_back(std::make_unique<VariableDeclarationNode>("argv", "string"));

    std::vector<std::unique_ptr<ASTNode>> functionBody;
    
    functionBody.push_back(std::make_unique<VariableDeclarationNode>("format", "string", std::make_unique<StringLiteralNode>("Olá, mundo Isa", "format", true)));
    std::vector<std::unique_ptr<ASTNode>> printlnArgs;
    auto formatExpr = std::make_unique<VariableValueNode>("format", "string");
    printlnArgs.push_back(std::move(formatExpr));
    auto printfCall = std::make_unique<FunctionCallNode>(
        "println",
        std::move(printlnArgs)
    );
    functionBody.push_back(std::move(printfCall));
    functionBody.push_back(std::make_unique<ReturnNode>(std::make_unique<IntegerLiteralNode>("i32", 0)));

    function = std::make_unique<FunctionNode>("main", "i32", std::move(functionParams), std::move(functionBody));
    program.push_back(std::move(function));
    /*
    std::vector<std::unique_ptr<VariableDeclarationNode>> printfParams;
    printfParams.push_back(std::make_unique<VariableDeclarationNode>("format", "string", std::make_unique<StringLiteralNode>("\"%d\\n\"")));
    //printfParams.push_back(std::make_unique<VariableDeclarationNode>("value", "i32", std::make_unique<IntegerLiteralNode>("i", 0)));
    auto printfDecl = std::make_unique<FunctionInstantiationNode>("printf", "i32", std::move(printfParams), true, true);
    program.push_back(std::move(printfDecl));

    std::unique_ptr<FunctionNode> function; 
    std::vector<std::unique_ptr<VariableDeclarationNode>> functionParams;

    functionParams.push_back(std::make_unique<VariableDeclarationNode>("argc", "i32"));
    functionParams.push_back(std::make_unique<VariableDeclarationNode>("argv", "string"));

    std::vector<std::unique_ptr<ASTNode>> functionBody;
    functionBody.push_back(std::make_unique<VariableDeclarationNode>("format", "string", std::make_unique<StringLiteralNode>("%d\n", "format", true)));

    functionBody.push_back(std::make_unique<VariableDeclarationNode>("num", "i32", std::make_unique<IntegerLiteralNode>("i32", 3000000)));
    functionBody.push_back(std::make_unique<VariableDeclarationNode>("sum", "i32", std::make_unique<IntegerLiteralNode>("i32", 0)));

    auto forInit = std::make_unique<VariableDeclarationNode>("i", "i32", std::make_unique<IntegerLiteralNode>("i32", 0));

    auto forCondition = std::make_unique<BinaryExpressionNode>(
        std::make_unique<VariableReferenceNode>("i", "i32"),
        "<=",
        std::make_unique<IntegerLiteralNode>("i32", 3000000) 
    );

    auto forIncrement = std::make_unique<BinaryExpressionNode>(
        std::make_unique<VariableReferenceNode>("i", "i32"),
        "+=",
        std::make_unique<IntegerLiteralNode>("i32", 1)
    );

    std::vector<std::unique_ptr<ASTNode>> forBody;

    auto formatExpr = std::make_unique<VariableValueNode>("format", "string");

    std::vector<std::unique_ptr<ASTNode>> printfArgs;
    printfArgs.push_back(std::move(formatExpr));
    printfArgs.push_back(std::make_unique<VariableValueNode>("i", "i32"));

    auto printfCall = std::make_unique<FunctionCallNode>(
        "printf", 
        std::move(printfArgs)
    );
    forBody.push_back(std::move(printfCall));
    forBody.push_back(std::make_unique<BinaryExpressionNode>(
        std::make_unique<VariableReferenceNode>("sum", "i32"),
        "+=",
        std::make_unique<VariableReferenceNode>("i", "i32")
    ));

    auto forNode = std::make_unique<ForNode>(
        std::move(forInit), 
        std::move(forCondition), 
        std::move(forIncrement), 
        std::make_unique<BlockNode>(std::move(forBody))
    );

    functionBody.push_back(std::move(forNode));


    functionBody.push_back(std::make_unique<ReturnNode>(std::make_unique<IntegerLiteralNode>("i32", 0)));

    function = std::make_unique<FunctionNode>("main", "i32", std::move(functionParams), std::move(functionBody));
    program.push_back(std::move(function));
    */
    for (int i = 0; i < program.size(); i++) {
        program[i]->accept(visitor);
        if (i == program.size()-1) {
                spinner.set_option(indicators::option::ForegroundColor{indicators::Color::green});
                spinner.set_option(indicators::option::PrefixText{"✔"});
                spinner.set_option(indicators::option::ShowSpinner{false});
                spinner.set_option(indicators::option::ShowPercentage{false});
                spinner.set_option(indicators::option::PostfixText{"Compiled successfully!"});
                spinner.mark_as_completed();
                break;
            } else {
                
                spinner.tick();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}




  void compiler(std::vector<std::unique_ptr<ASTNode>> program) {
    LLVMCodeGenVisitor visitor(&getBuilder(), context.get(), module.get());
    indicators::ProgressSpinner spinner{
        indicators::option::PostfixText{"Compiling Isa code! 😼"},
        indicators::option::ForegroundColor{indicators::Color::yellow},
        indicators::option::SpinnerStates{std::vector<std::string>{"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"}},
        indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
    };
    for (int i = 0; i < program.size(); i++) {
        program[i]->accept(visitor);
        if (i == program.size()-1) {
                spinner.set_option(indicators::option::ForegroundColor{indicators::Color::green});
                spinner.set_option(indicators::option::PrefixText{"✔"});
                spinner.set_option(indicators::option::ShowSpinner{false});
                spinner.set_option(indicators::option::ShowPercentage{false});
                spinner.set_option(indicators::option::PostfixText{"Compiled successfully! 😼"});
                spinner.mark_as_completed();
                break;
            } else {
                
                spinner.tick();
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    
  }



  /**
   * Function LLVM Init
   **/
   void init_all_targets() {
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmPrinters();
    LLVMInitializeAllAsmParsers();
}

void initModuleLLVM() {
    init_all_targets();

    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("IsaLLVM", *context);

    std::string targetTripleStr = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(targetTripleStr);  

    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTripleStr, error);
    if (!target) {
        llvm::errs() << "Error finding target: " << error << "\n";
        return;
    }

    llvm::TargetOptions opt;
    auto targetMachine = target->createTargetMachine(
        targetTripleStr,  
        "generic", 
        "", 
        opt,
        std::optional<llvm::Reloc::Model>(),
        std::nullopt,
        llvm::CodeGenOptLevel::Default
    );

    if (!targetMachine) {
        llvm::errs() << "Error creating target machine\n";
        return;
    }

    module->setDataLayout(targetMachine->createDataLayout());
}

void saveModuleFile(const std::string& filename) {
    std::error_code errorCode;
    llvm::raw_fd_ostream fdLL(filename, errorCode);
    if (errorCode) {
        llvm::errs() << "Error opening file " << filename << ": " << errorCode.message() << "\n";
        return;
    }
    module->print(fdLL, nullptr);
}

void saveModuleBinary(const std::string& filename) {
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

    if (ec) {
        llvm::errs() << "Error opening file " << filename << ": " << ec.message() << "\n";
        return;
    }

    if (!targetMachine) {
        llvm::errs() << "Target machine not initialized!\n";
        return;
    }

    llvm::legacy::PassManager passManager;
    if (targetMachine->addPassesToEmitFile(passManager, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        llvm::errs() << "Target does not support object file generation!\n";
        return;
    }

    passManager.run(*module);
    dest.flush();
    std::cout << termcolor::color<211, 54, 130> << "Object file generated: " << filename << "\n";
}


// 
void exec_program() {
  std::string command = "lli " + output; 
  if (flags.isActive(FlagID::RunInputFile)) {
    int result = std::system(command.c_str());
  }
}

void linkObjectFile(const std::string& objectFilename, const std::string& outputExecutable) {
    std::string command = "clang " + objectFilename + " -o " + outputExecutable + " -lSDL2";
    int result = std::system(command.c_str());
    
    if (result != 0) {
        llvm::errs() << "Error linking executable\n";
    } else {
        std::string targetTriple = module->getTargetTriple();
        std::cout << termcolor::color<211, 54, 130> 
                  << "[-] " << targetTriple << " executable generated: " 
                  << outputExecutable << std::endl;
    }
}

void generateExecutable(const std::string& filename) {
    std::string objectFile = filename + ".o";
    std::string executableFile = filename;

    saveModuleBinary(objectFile);
    linkObjectFile(objectFile, executableFile);
}


  std::unique_ptr<llvm::TargetMachine> targetMachine; 
  /**
   * Global LLVM Context.
   **/
  std::unique_ptr<llvm::LLVMContext> context;
  /** 
   * O context é como um conteiner de modulos.
   * Atravez dele podemos encapsula informações globais usadas em todo o processo de compilação. 
   * No LLVM, muitos dos objetos, como tipos e constantes, são armazenados e gerenciados dentro de um contexto.
   * O LLVMContext tambem pode ajudor no gerenciamento de err e Depuração.
   **/

  std::unique_ptr<llvm::Module> module; /* Cada modulo pode set compilado e executado de forma independent */
  /**
   * Módulos são o contêiner de nível superior de todos os outros objetos de Representação Intermediária (IR) do LLVM. 
   * Cada módulo contém diretamente uma lista de variáveis globais, uma lista de funções, uma lista de bibliotecas (ou outros módulos) 
   * dos quais esse módulo depende, uma tabela de símbolos e vários dados sobre as características do alvo.
   **/

  std::unique_ptr<llvm::IRBuilder<>> builder;
  /**
   * O IRBuilder é uma class auxiliary que facilita a construção de instruções LLVM IR 
   * */

  llvm::Function* fn;
  
  llvm::Value *codegen() {
    // return builder->getInt32(0);
    auto str = builder->CreateGlobalString("%i\n");

    auto printfn = module->getFunction("printf");
    std::vector<llvm::Value*> args {str};
    return builder->CreateCall(printfn, args);
  
  }
  
  void externFunctions() {
    auto bytePtr = llvm::PointerType::get(builder->getInt8Ty(), 0);
    module->getOrInsertFunction("printf", llvm::FunctionType::get(builder->getInt32Ty(), bytePtr, true));
  }

  /**
   * Create function main -> entry 
   * */
    llvm::Function *createFunction(const std::string& name, llvm::FunctionType *type) {
    
    auto fn = module->getFunction(name);
    if( fn == nullptr ) {
      fn = createFunctionStat(name, type);
    }
    createFunctionBlock(fn);
    llvm::verifyFunction(*fn);
    return fn;
  }

  llvm::Function *createFunctionStat(const std::string& name, llvm::FunctionType *type) {
    llvm::Function * statefun = llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, *module);
    return statefun;
  }

  void createFunctionBlock(llvm::Function *fn) {
    auto entry = createB(fn, "entry");
    builder->SetInsertPoint(entry);
  }
/**
  * BasicBlock
  *
  * Um BasicBlock é uma sequência contínua de instruções no LLVM IR que tem as seguintes características:
  *
  * Linearidade: As instruções em um bloco básico são executadas de maneira linear, de cima para baixo,
  * sem saltos ou desvios dentro do bloco.
  *
  * Um Ponto de Entrada: Cada bloco básico tem exatamente um ponto de entrada. Isso significa que a execução
  * do código dentro de um bloco básico sempre começa na primeira instrução do bloco.
  *
  * Um Ponto de Saída: Cada bloco básico tem exatamente um ponto de saída. A execução sempre sai do bloco básico
  * na última instrução, que deve set uma instrução de control de fluxo (como ret, br, ou switch), indicando
  * para onde a execução deve ir em seguida.
  *
  * Unidade de Control de Fluxo: Blocos básicos são as unidades que compõem o control de fluxo de funções.
  * A execução de uma função em LLVM IR é uma sequência de transições de um bloco básico para outro.
   */

  llvm::BasicBlock *createB( llvm::Function *starfun, const std::string& name ) {
    return llvm::BasicBlock::Create(*context, name,starfun);
  }
};

#endif // !
