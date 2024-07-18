# Documentação da Linguagem Isa

## Visão Geral
A linguagem Isa é projetada para ser uma linguagem de programação com suporte para tipos primitivos, arrays, strings, estruturas (structs), e várias estruturas de controle, como laços e condicionais. Abaixo, segue a definição detalhada da gramática da linguagem.

### 1. Estrutura do Programa
O programa consiste em uma lista de declarações, que podem ser variáveis, funções ou estruturas.

```bnf
<program> ::= <declaration_list>

<declaration_list> ::= <declaration> <declaration_list>
                     | <declaration>
```

### 2. Declarações
As declarações podem ser de variáveis, funções ou structs.

```bnf
<declaration> ::= <variable_declaration>
                | <function_declaration>
                | <struct_declaration>
```

#### 2.1 Declaração de Variáveis
Uma variável é declarada usando a palavra-chave `let`, seguida do tipo, identificador e valor.

```bnf
<variable_declaration> ::= "let" ":" <type> <identifier> "=" <expression> ";"
                         | "let" ":" <type> <identifier> ";"
```

Os tipos suportados incluem inteiros, floats, booleanos, strings, arrays de tamanho fixo e structs.

```bnf
<type> ::= "i8" 
         | "i16"
         | "i32"
         | "i64"
         | "u8"
         | "u16"
         | "u32"
         | "u64"
         | "f16"
         | "f32"
         | "f64"
         | "bool"
         | "string"
         | "str[" <integer_literal> "]"
         | "array[" <type> "," <integer_literal> "]"
         | <type> "*"
         | <type> "&"
         | <identifier>  ; para structs
```

#### 2.2 Declaração de Funções
Funções são declaradas com a palavra-chave `fn`, seguida do nome, parâmetros, tipo de retorno e corpo.

```bnf
<function_declaration> ::= "fn" <identifier> "(" <parameter_list> ")" "->" <type> "{" <statement_list> "}"

<parameter_list> ::= <parameter> 
                   | <parameter> "," <parameter_list>
                   | /* vazio */

<parameter> ::= <type> <identifier>
```

#### 2.3 Declaração de Structs
Structs são coleções de variáveis agrupadas e podem incluir um construtor.

```bnf
<struct_declaration> ::= "struct" <identifier> "{" <struct_member_list> "}" <constructor>?

<struct_member_list> ::= <struct_member> <struct_member_list>
                       | <struct_member>

<struct_member> ::= "let" ":" <type> <identifier> ";"

<constructor> ::= "fn" <identifier> "(" <parameter_list> ")" "{" <statement_list> "}"
```

### 3. Bloco de Instruções
Instruções podem ser declarações de variáveis, expressões, retornos, laços, condicionais, impressões, atribuições ou declarações switch.

```bnf
<statement_list> ::= <statement> <statement_list>
                   | <statement>

<statement> ::= <variable_declaration>
              | <expression_statement>
              | <return_statement>
              | <for_statement>
              | <while_statement>
              | <if_statement>
              | <print_statement>
              | <assignment_statement>
              | <switch_statement>

<expression_statement> ::= <expression> ";"
<return_statement> ::= "return" <expression> ";"
<print_statement> ::= "println" "(" <string_literal> "," <expression> ")" ";"
<assignment_statement> ::= <identifier> "=" <expression> ";"
```

#### 3.1 Laço `for`
O laço `for` segue a sintaxe de inicialização, condição e atualização.

```bnf
<for_statement> ::= "for" "(" <variable_declaration> <expression> ";" <expression> ")" "{" <statement_list> "}"
```

#### 3.2 Laço `while`
O laço `while` executa enquanto a expressão for verdadeira.

```bnf
<while_statement> ::= "while" "(" <expression> ")" "{" <statement_list> "}"
```

#### 3.3 Condicional `if`
O condicional `if` pode incluir um `else` ou `else if`.

```bnf
<if_statement> ::= "if" "(" <expression> ")" "{" <statement_list> "}" <else_clause>?

<else_clause> ::= "else" "{" <statement_list> "}"
                | "else if" "(" <expression> ")" "{" <statement_list> "}" <else_clause>?
                | /* vazio */
```

#### 3.4 Declaração `switch`
A declaração `switch` avalia uma expressão e executa o caso correspondente.

```bnf
<switch_statement> ::= "switch" "(" <expression> ")" "{" <case_list> "}"

<case_list> ::= <case> <case_list>
              | <case>

<case> ::= "case" <expression> ":" <statement_list>
         | "default" ":" <statement_list>
```

### 4. Expressões
Expressões podem ser literais, identificadores, operações binárias, chamadas de função, etc.

```bnf
<expression> ::= <literal>
               | <identifier>
               | <binary_expression>
               | <parenthesized_expression>
               | <array_expression>
               | <struct_expression>
               | <function_call>
               | <pointer_expression>
               | <reference_expression>

<binary_expression> ::= <expression> <binary_operator> <expression>
<binary_operator> ::= "+" | "-" | "*" | "/" | "==" | "!=" | "<" | ">" | "<=" | ">="

<parenthesized_expression> ::= "(" <expression> ")"

<literal> ::= <integer_literal> 
            | <float_literal>
            | <bool_literal>
            | <string_literal>

<integer_literal> ::= [0-9]+
<float_literal> ::= [0-9]+"."[0-9]+
<bool_literal> ::= "true" | "false"
<string_literal> ::= "\"" .*? "\""

<array_expression> ::= "{" <expression_list> "}"

<expression_list> ::= <expression>
                    | <expression> "," <expression_list>

<struct_expression> ::= <identifier> "{" <struct_field_list> "}"

<struct_field_list> ::= <struct_field>
                      | <struct_field> "," <struct_field_list>

<struct_field> ::= <identifier> "=" <expression>

<function_call> ::= <identifier> "(" <argument_list> ")"

<argument_list> ::= <expression>
                  | <expression> "," <argument_list>

<identifier> ::= [a-zA-Z_][a-zA-Z0-9_]*

<pointer_expression> ::= "new" <type> "(" <expression> ")"
<reference_expression> ::= "&" <identifier>
```

### 5. Exemplo de Código Expandido
Abaixo, um exemplo de código utilizando a gramática descrita:

```plaintext
let: i32 num = 10;
let: f32 ponto = 0.40;
let: array[i32, 3] vetor = { 10, 5, 3 };
let: str[10] fixed_str = "Olá, mundo!";
let: string dynamic_str = "Olá, mundo! Isa";
let: u8 small_number = 255;
let: u32 large_number = 4294967295;

struct Tipo {
    let: i32 value;
    let: string nome;

    fn new(v: i32, n: string) -> Tipo {
        let: Tipo t = Tipo { value = v, nome = n };
        return t;
    }
};

fn main() -> i32 {
    let: Tipo t = Tipo::new(42, "Exemplo");
    for (let: i8 i = 0; i < 10; i++) {
        println("{}", i);
    }

    while (num > 0) {
        println("{}", num);
        num = num - 1;
    }

    if (num == 0) {
        println("End of loop");
    } else {
        println("Error");
    }

    switch (num) {
        case 0:
            println("Zero");
            break;
        case 1:
            println("One");
            break;
        default:
            println("Other");
    }

    let: i32 *p_num = new i32(10);
    let: i32 &ref_num = num;
    ref_num = 20;

    return 0;
}
```

Esta documentação detalha a gramática da linguagem Isa e inclui suporte para tipos primitivos, arrays, strings, structs, funções, e várias estruturas de controle.

---