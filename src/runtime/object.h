#ifndef OBJECT_H
#define OBJECT_H

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
  FunctionObject(int arity) : Object(ObjectKind::Function), arity(arity) {}

  int arity;
  Chunk chunk;
  // should name be a std::string or an Object?
};

#endif //OBJECT_H
