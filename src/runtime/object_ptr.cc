#include "object_ref.h"

ObjectRef::ObjectRef(FunctionObject&& object)
    : inner(new ObjectRefInner{std::move(object), 1}) {}
ObjectRef::ObjectRef(ClosureObject&& object)
    : inner(new ObjectRefInner{std::move(object), 1}) {}

ObjectRef::ObjectRef(const ObjectRef& other) : inner(other.inner) {
  inner = other.inner;
  if (inner != nullptr) {
    ++inner->refCount;
  }
}

ObjectRef::ObjectRef(ObjectRef&& other) : inner(other.inner) {
  inner = other.inner;
  other.inner = nullptr;
}

ObjectRef::~ObjectRef() {
  if (inner) {
    --inner->refCount;
    if (inner->refCount == 0) {
      delete inner;
    }
  }
}

ObjectRef& ObjectRef::operator=(const ObjectRef& other) {
  inner = other.inner;
  if (inner != nullptr) {
    ++inner->refCount;
  }
  return *this;
}

ObjectRef& ObjectRef::operator=(ObjectRef&& other) {
  inner = other.inner;
  other.inner = nullptr;
  return *this;
}

bool ObjectRef::isFunction() const {
  return std::holds_alternative<FunctionObject>(inner->object);
}

bool ObjectRef::isClosure() const {
  return std::holds_alternative<ClosureObject>(inner->object);
}

const FunctionObject& ObjectRef::toFunction() const {
  return std::get<FunctionObject>(inner->object);
}

const ClosureObject& ObjectRef::toClosure() const {
  return std::get<ClosureObject>(inner->object);
}

FunctionObject& ObjectRef::toFunction() {
  return std::get<FunctionObject>(inner->object);
}

ClosureObject& ObjectRef::toClosure() {
  return std::get<ClosureObject>(inner->object);
}
