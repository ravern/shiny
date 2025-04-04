include(FetchContent)

FetchContent_Declare(argparse GIT_REPOSITORY https://github.com/p-ranav/argparse.git)
FetchContent_MakeAvailable(argparse)

add_library(core
        core.cc
        debug.cc
        error.h
        bytecode.h
        frontend/scanner.h
        frontend/expr.h
        frontend/type.h
        frontend/type_inference.h
        frontend/type_constraint.h
        frontend/ast_visitor.h
        frontend/union_find.h
        frontend/ast_pretty_printer.h
        frontend/stmt.h
        frontend/factory.h
        frontend/string_interner.h
        frontend/var.h
        frontend/compiler.h
        runtime/value.cc
        runtime/object.cc
        runtime/object_ptr.cc
        vm/vm.cc
        frontend/error.h
        frontend/parser.h
        frontend/token.h
)

add_executable(shiny main.cc)
target_link_libraries(shiny core argparse)
