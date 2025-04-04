# Shiny

Swift-like interpreter in C++.

## Development

For the initial build:

```bash
cmake -B build
```

For subsequent builds:

```bash
cmake --build build
```

Run the executable:

```bash
build/src/shiny
```

Run the tests:

```bash
build/tests/tests
```

## Specification

### Types

| Type     | Value |
| -------- | ----- |
| `Int`    | 1     |
| `Double` | 2     |
| `String` | 3     |
| `Array`  | 4     |

### Opcodes

| Opcode              | Operand                          | Stack (before) | Stack (after) | Action                        | Remark                                   |
| ------------------- | -------------------------------- | -------------- | ------------- | ----------------------------- | ---------------------------------------- |
| `NO_OP`             |                                  |                |               | -                             | Does absolutely nothing.                 |
| `NIL`               |                                  | - - -          | - - - A       | A = nil                       |                                          |
| `TRUE`              |                                  | - - -          | - - - A       | A = true                      |                                          |
| `FALSE`             |                                  | - - -          | - - - A       | A = false                     |                                          |
| `ARRAY`             |                                  | - - -          | - - - A       | A = []                        |                                          |
| `DICT`              |                                  | - - -          | - - - A       | A = [:]                       |                                          |
| `CONST`             | index of constant                | - - -          | - - - A       | A = constants[operand]        |                                          |
| `CLOSURE`           | index of function definition     | - - -          | - - - A       | A = functionDefs[operand]     |                                          |
| `BUILT_IN`          | index of built-in function       | - - -          | - - - A       | A = builtInFunctions[operand] |                                          |
| `ADD`               | type of operands (1, 2, 3, 4)    | - - - A B      | - - - C       | C = A + B                     |                                          |
| `SUB`               | type of operands (1, 2)          | - - - A B      | - - - C       | C = A - B                     |                                          |
| `MUL`               | type of operands (1, 2)          | - - - A B      | - - - C       | C = A * B                     |                                          |
| `DIV`               | type of operands (1, 2)          | - - - A B      | - - - C       | C = A / B                     |                                          |
| `MOD`               | -                                | - - - A B      | - - - C       | C = A % B                     |                                          |
| `NEG`               | type of operands (1, 2)          | - - - A        | - - - B       | B = -A                        |                                          |
| `EQ`                | -                                | - - - A B      | - - - C       | C = A == B                    |                                          |
| `NEQ`               | -                                | - - - A B      | - - - C       | C = A != B                    |                                          |
| `LT`                | type of operands (1, 2)          | - - - A B      | - - - C       | C = A < B                     |                                          |
| `LTE`               | type of operands (1, 2)          | - - - A B      | - - - C       | C = A <= B                    |                                          |
| `GT`                | type of operands (1, 2)          | - - - A B      | - - - C       | C = A > B                     |                                          |
| `GTE`               | type of operands (1, 2)          | - - - A B      | - - - C       | C = A >= B                    |                                          |
| `AND`               | -                                | - - - A B      | - - - C       | C = A && B                    |                                          |
| `OR`                | -                                | - - - A B      | - - - C       | C = A \|\| B                  |                                          |
| `NOT`               | -                                | - - - A        | - - - B       | C = !A                        |                                          |
| `BIT_AND`           | -                                | - - - A B      | - - - C       | C = A & B                     |                                          |
| `BIT_OR`            | -                                | - - - A B      | - - - C       | C = A \| B                    |                                          |
| `BIT_XOR`           | -                                | - - - A B      | - - - C       | C = A ^ B                     |                                          |
| `BIT_NOT`           | -                                | - - - A        | - - - B       | B = ~A                        |                                          |
| `SHIFT_LEFT`        | -                                | - - - A B      | - - - C       | C = A << B                    |                                          |
| `SHIFT_RIGHT`       | -                                | - - - A B      | - - - C       | C = A >> B                    |                                          |
| `LOAD`              | stack slot of local              | - - -          | - - - A       | A = stack[base + operand]     |                                          |
| `STORE`             | stack slot of local              | - - - A        | - - -         | stack[base + operand] = A     |                                          |
| `DUP`               | -                                | - - - A        | - - - A A     | -                             |                                          |
| `POP`               | -                                | - - - A        | - - -         | -                             |                                          |
| `TEST`              | -                                | - - - A        | - - -         | if A then pc = pc + 1         |                                          |
| `JUMP`              | offset of instruction to jump to | - - -          | - - -         | pc = operand                  |                                          |
| `CALL`              | number of arguments              | - - - A B C    | - - - D       | D = A(B, C)                   | Example of `CALL` with 2 arguments.      |
| `TAIL_CALL`         | number of arguments              | - - - A B C    | - - - D       | D = A(B, C)                   | Example of `TAIL_CALL` with 2 arguments. |
| `RETURN`            |                                  | - - -          | - - -         | -                             | Pops the call frame.                     |
| `UPVALUE_LOAD`      | index of upvalue                 | - - -          | - - - A       | A = upvalues[operand]         |                                          |
| `UPVALUE_STORE`     | index of upvalue                 | - - - A        | - - -         | upvalues[operand] = A         |                                          |
| `UPVALUE_CLOSE`     |                                  | - - - A        | - - -         | closeUpvalue(A)               |                                          |
| `OBJECT_GET_MEMBER` | index of member                  | - - - A        | - - - B       | B = A.members[operand]        |                                          |
| `OBJECT_SET_MEMBER` | index of member                  | - - - A B      | - - -         | A.members[operand] = B        |                                          |
| `OBJECT_GET_METHOD` | index of method definition       | - - - A        | - - - B       | B = A.methods[operand]        |                                          |