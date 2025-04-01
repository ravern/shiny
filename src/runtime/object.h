#ifndef OBJECT_H
#define OBJECT_H

struct Upvalue {
  uint8_t index;
  bool isLocal;

  Upvalue(uint8_t index, bool isLocal) : index(index), isLocal(isLocal) {}
};

enum class ObjectKind {
  Function,
};

class Object {
public:
  explicit Object(const ObjectKind kind) : type(kind) {}
  virtual ~Object() = default;

  ObjectKind getType() const { return type; }

private:
  ObjectKind type;
};

class FunctionObject : public Object {
public:
  FunctionObject(int arity)
    : Object(ObjectKind::Function), arity(arity) {}

  int arity;
  std::vector<Upvalue> upvalues;
  Chunk chunk;
  // should name be a std::string or an Object?
};

#endif //OBJECT_H
