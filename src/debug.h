#pragma once

#include <string>

#include "bytecode.h"
#include "frontend/string_interner.h"

std::string opcodeToString(Opcode opcode);
std::string chunkToString(const Chunk& chunk, const std::string& name,
                          const StringInterner& stringInterner);
std::string instructionToString(size_t offset, Instruction instr,
                                const StringInterner& stringInterner);
std::string valueToString(const Value& value,
                          const StringInterner& stringInterner);