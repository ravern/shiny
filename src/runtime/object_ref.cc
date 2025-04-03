#include "object_ref.h"

#include "object_ref_inner.h"

ObjectRef::ObjectRef(FunctionObject&& object)
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

const FunctionObject& ObjectRef::toFunction() const {
  return std::get<FunctionObject>(inner->object);
}

FunctionObject& ObjectRef::toFunction() {
  return std::get<FunctionObject>(inner->object);
}
