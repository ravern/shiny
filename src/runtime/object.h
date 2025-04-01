#ifndef OBJECT_H
#define OBJECT_H

struct Upvalue;
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
  FunctionObject(int arity, int upvalueCount)
    : Object(ObjectKind::Function), arity(arity), upvalueCount(upvalueCount) {}

  int arity;
  int upvalueCount;
  std::vector<Upvalue> upvalues;
  Chunk chunk;
  // should name be a std::string or an Object?
};

#endif //OBJECT_H
