#pragma once

#include "object.h"

struct ObjectRefInner {
  Object object;
  uint64_t refCount;
};