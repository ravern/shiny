#pragma once

#include <string>

#include "bytecode.h"

std::string opcodeToString(Opcode opcode);
void disassembleChunk(const Chunk& chunk, const std::string& name);
