#ifndef VAR_H
#define VAR_H
#include <optional>

#include "string_interner.h"
#include "type.h"

using VariableName = SymbolId;

struct Var {
  SymbolId name;
  std::optional<std::shared_ptr<Type>> type;

  explicit Var(SymbolId name)
    : name(name), type(std::nullopt) {}

  Var(SymbolId name, std::optional<std::shared_ptr<Type>> type)
    : name(name), type(std::move(type)) {}

  bool operator==(const Var& other) const {
    if (name != other.name) return false;

    // If both types are nullopt, they're equal
    if (!type.has_value() && !other.type.has_value()) return true;

    // If one has a value and the other doesn't, they're not equal
    if (type.has_value() != other.type.has_value()) return false;

    // Both have values, compare the pointed-to Types
    return *type.value() == *other.type.value();
  }

  bool operator!=(const Var &other) const = default;
};

#endif //VAR_H
