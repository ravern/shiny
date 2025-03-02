#ifndef TYPE_H
#define TYPE_H

enum Type {
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_NONE,
};

Type mapTypeAnnotation(std::string_view annotation) {
  static const std::unordered_map<std::string_view, Type> typeMap = {
    {"Int", TYPE_INT},
    {"Float", TYPE_FLOAT},
  };

  auto it = typeMap.find(annotation);
  if (it != typeMap.end()) {
    return it->second;
  }

  throw std::runtime_error("Unknown type annotation: " + std::string(annotation));
}

#endif //TYPE_H
