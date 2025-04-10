#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "string_interner.h"

using TypeVar = uint32_t;

enum class TypeKind {
  Void,
  Integer,
  Double,
  Boolean,
  Variable,
  Function,
  Class,
  Instance,
};

class Type {
public:
  TypeKind kind;

  explicit Type(TypeKind kind) : kind(kind) {}
  virtual ~Type() = default;

  virtual bool operator==(const Type &other) const {
    return kind == other.kind;
  }
  bool operator!=(const Type &other) const = default;

  virtual std::string toString() const = 0;
};

class VoidType : public Type {
public:
  explicit VoidType() : Type(TypeKind::Void) {}

  std::string toString() const override {
    return "Void";
  }
};

class VariableType : public Type {
public:
  TypeVar typeVar;

  explicit VariableType(TypeVar typeVar)
    : Type(TypeKind::Variable), typeVar(typeVar) {}

  bool operator==(const Type &other) const override {
    if (kind != other.kind) {
      return false;
    }
    auto& otherType = static_cast<const VariableType&>(other);
    return typeVar == otherType.typeVar;
  }

  std::string toString() const override {
    return "VariableType " + std::to_string(typeVar);
  }
};

class IntegerType : public Type {
public:
  explicit IntegerType() : Type(TypeKind::Integer) {}

  std::string toString() const override {
    return "Int";
  }
};

class DoubleType : public Type {
public:
  explicit DoubleType() : Type(TypeKind::Double) {}

  std::string toString() const override {
    return "Double";
  }
};

class BoolType : public Type {
public:
  explicit BoolType() : Type(TypeKind::Boolean) {}

  std::string toString() const override {
    return "Bool";
  }
};

class FunctionType : public Type {
public:
  std::vector<std::shared_ptr<Type>> parameters;
  std::shared_ptr<Type> ret;

  FunctionType(std::vector<std::shared_ptr<Type>> parameters, std::shared_ptr<Type> ret)
    : Type(TypeKind::Function),
      parameters(std::move(parameters)),
      ret(std::move(ret)) {
  }

  bool operator==(const Type &other) const override {
    if (other.kind != TypeKind::Function) {
      return false;
    }
    auto& otherType = static_cast<const FunctionType&>(other);

    if (parameters.size() != otherType.parameters.size()) {
      return false;
    }

    for (size_t i = 0; i < parameters.size(); i++) {
      if (*parameters[i] != *(otherType.parameters[i])) {
        return false;
      }
    }

    return *ret == *(otherType.ret);
  }

  std::string toString() const override {
    std::string result = "(";

    for (size_t i = 0; i < parameters.size(); i++) {
      if (i > 0) {
        result += ", ";
      }
      result += parameters[i]->toString();
    }

    result += ") -> " + ret->toString();
    return result;
  }
};

class ClassType : public Type {
public:
  SymbolId name;
  std::vector<std::pair<SymbolId, std::shared_ptr<Type>>> members;

  ClassType(SymbolId name,
            std::vector<std::pair<SymbolId, std::shared_ptr<Type>>> members)
    : Type(TypeKind::Class),
      name(name),
      members(std::move(members)) {}

  bool operator==(const Type &other) const override {
    if (other.kind != TypeKind::Class) return false;
    const auto &o = static_cast<const ClassType&>(other);
    // TODO: compare members
    return name == o.name;
  }

  // FIXME: this should use a StringInterner
  std::string toString() const override {
    return "Class " + std::to_string(name);
  }

  int getMemberIndex(const SymbolId name) const {
    for (int i = 0; i < members.size(); ++i) {
      if (members[i].first == name) {
        return i;
      }
    }
    return -1;
  }

  std::optional<std::shared_ptr<Type>> getMemberType(const SymbolId name) {
    for (int i = 0; i < members.size(); ++i) {
      if (members[i].first == name) {
        return members[i].second;
      }
    }
    return std::nullopt;
  }

  std::optional<std::shared_ptr<Type>> getMemberType(int index) {
    if (index < 0 || index >= members.size()) {
      return std::nullopt;
    }
    return members[index].second;
  }
};

class InstanceType : public Type {
public:
  std::shared_ptr<ClassType> klass;
  InstanceType(std::shared_ptr<ClassType> klass) : Type(TypeKind::Instance), klass(std::move(klass)) {}

  bool operator==(const Type &other) const override {
    if (other.kind != TypeKind::Instance) return false;
    const auto &o = static_cast<const InstanceType&>(other);
    return klass == o.klass;
  }

  std::string toString() const override {
    return "Instance " + std::to_string(klass->name);
  }
};

#endif //TYPE_H
