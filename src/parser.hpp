/**
 * Isa to compiler parser in code LLVM.
 * */
#pragma once 

#include <iostream>
#ifndef isaLLVMPARSER
#define isaLLVMPARSER
/* includes header LLVM */
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Type.h>
/* includes libs c */
#include <memory>
#include <string>
#include <system_error>
#include <vector>
/* includes files project */
#include "ast.hpp"



class IsaLLVM {
  public:
    IsaLLVM() {
    
    initModuleLLVM();
    externFunctions();
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
  void exec() {
    compiler();
    module->print(llvm::outs(), nullptr);
    salveModuleFile("out.ll");
  }
/*
  void exec(std::vector<std::unique_ptr<ASTNode>> program, const std::string& filename) {
    //compiler(std::move(program));
    module->print(llvm::outs(), nullptr);
    salveModuleFile(filename);
  }
 */
  private:
  
  /* compiler run */

  void compiler() {

    llvm::Function *fn = createFunction( "main", llvm::FunctionType::get(builder->getInt32Ty(), false));
    LLVMCodeGenVisitor visitor(&getBuilder(),context.get(),module.get());
    std::unique_ptr<VariableDeclarationNode> variable = std::make_unique<VariableDeclarationNode>("num","i32",std::make_unique<IntegerLiteralNode>(10));
    variable->accept(visitor);
    std::cout << "Test" << '\n';
    builder->CreateRet(builder->getInt32(0));
  
  }

  void compiler(std::vector<std::unique_ptr<ASTNode>> program) {
    // create function main ( entry ) -> isaLLVMCompiler 
    llvm::Function *fn = createFunction( "main", llvm::FunctionType::get(builder->getInt32Ty(), false));
    //auto res = codegen();
    // auto i32Result = builder->CreateIntCast(res, builder->getInt32Ty(), true );
    for(const auto &a: program) {
      if(a.get() != nullptr) {
        //auto var = dynamic_cast<VariableExpAST*>(a.get());
        //var->codegen(*builder,*context, *module); 
      }
     
    }
    // auto var = dynamic_cast<VariableExpAST*>(program[0].get());
   

    builder->CreateRet(builder->getInt32(0));
    
  }



  /**
   * Function LLVM Init
   **/
  void initModuleLLVM() {
    /* new context and module */
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("IsaLLVM", *context);  
  }

  void salveModuleFile(const std::string& filename) {
    std::error_code errorCode;
    llvm::raw_fd_ostream fdLL(filename, errorCode);
    module->print(fdLL, nullptr);
  }

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
    auto str = builder->CreateGlobalStringPtr("Olá, mundo isa!\n");

    // call print 
    auto printfn = module->getFunction("printf");
    std::vector<llvm::Value*> args {str};
    return builder->CreateCall(printfn, args);
  
  }
  
  void externFunctions() {
    auto bytePtr = builder->getInt8Ty()->getPointerTo();
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
