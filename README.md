# Bluejay Compiler Design with C

[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/rptgfToH)

## Introduction

The Bluejay Compiler is a project designed to compile the Bluejay programming language. This project is implemented in C and includes various components such as lexical analysis, parsing, semantic analysis, and code generation.

## Features

- Lexical analysis using Flex
- Parsing using Bison
- Semantic analysis for symbol resolution and type checking
- Frame computation for function calls
- Detailed error reporting


## Bluejay Language Specifications

Bluejay is a statically-typed, compiled programming language designed for simplicity and performance.

### Syntax

- **Variables**: Declared using `let` keyword.
  ```bluejay
  let x: Int = 10;

## Installation

To set up the project locally, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/SiddhiDabholkar10/Bluejay-Compiler-Design-with-C.git
    ```
2. Navigate to the project directory:
   
3. Build the project using `make`:
    ```sh
    make clean
    make
    ```


## Bluejay Language Specification

The Bluejay language includes the following features:
- Basic data types: integers, strings, and arrays
- Control structures: if-else statements, loops
- Functions: declaration, definition, and calls
- Variables: local and global scope

For a detailed specification, refer to the [Bluejay Language Specification](path/to/specification.pdf).

## Compiler Design

The Bluejay compiler consists of the following components:

1. **Lexical Analysis**: Implemented in [`lexer.lex`](lexer.lex), it converts the input source code into tokens.
2. **Parsing**: Implemented in [`parser.y`](parser.y), it constructs the abstract syntax tree (AST) from tokens.
3. **Semantic Analysis**: Implemented in [`semantic_analysis_symbols.c`](semantic_analysis_symbols.c) and [`semantic_analysis_types.c`](semantic_analysis_types.c), it performs symbol resolution and type checking.
4. **Frame Computation**: Implemented in [`frames.c`](frames.c), it computes the frames for function calls.
5. **Code Generation**: Not implemented in this project.

## Testing

To run the tests, execute the following script:
```sh
./runtests.sh
```
The tests are located in the `tests/` directory and cover various aspects of the Bluejay language.

## Contributing

Contributions are welcome! Please follow these steps to contribute:

1. Fork the repository.
2. Create a new branch:
    ```sh
    git checkout -b feature-branch
    ```
3. Make your changes and commit them:
    ```sh
    git commit -m "Description of changes"
    ```
4. Push to the branch:
    ```sh
    git push origin feature-branch
    ```
5. Create a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
