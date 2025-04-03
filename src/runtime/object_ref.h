#pragma once

class FunctionObject;

struct ObjectRefInner;

class ObjectRef {
 public:
  ObjectRef(FunctionObject&& object);
  ObjectRef(const ObjectRef& other);
  ObjectRef(ObjectRef&& other);
  ~ObjectRef();

 public:
  ObjectRef& operator=(const ObjectRef& other);
  ObjectRef& operator=(ObjectRef&& other);

 public:
  const FunctionObject& toFunction() const;
  FunctionObject& toFunction();

 private:
  ObjectRefInner* inner;
};