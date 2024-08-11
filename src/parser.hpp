/**
 * Isa to compiler parser in code LLVM.
 * */
#ifndef isaLLVMPARSER
#define isaLLVMPARSER
/* includes header LLVM */
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
/* includes libs c */
#include <memory>
/* includes files project */
// #include "lexer.hpp"
#include "token.hpp"
#pragma once 

class IsaLLVM {
  public:
    IsaLLVM() {

  }
  /**
   * Execute program
   **/
  void exec(const std::vector<Token>& program) {
    
  }
  /**
   * 1 parser compiler AST
   *
   * 2 codegen compiler LLVM
   *
   * compiler(AST)
   * salve module IR to file 
   * */
   // salveModuleFile("./out.ll");
  private:
  /**
   * Function LLVM Init
   **/
  void initModuleLLVM() {
    /* new context and module */
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("IsaLLVM", *context);  
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
};

#endif // !
