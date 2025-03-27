#ifndef STRING_INTERNER_H
#define STRING_INTERNER_H
#include <cstdint>
#include <unordered_map>
#include <vector>

// Using a uint32_t as the index type for interned strings
using SymbolId = uint32_t;

class StringInterner {
    std::vector<std::string> symbols;
    std::unordered_map<std::string, SymbolId> symbolToId;

public:
    StringInterner() {
        // Reserve ID 0 for an empty string
        symbols.emplace_back("");
        symbolToId[""] = 0;
    }

    // Get the ID for a string, creating a new entry if it doesn't exist
    SymbolId intern(const std::string& str) {
        auto it = symbolToId.find(str);
        if (it != symbolToId.end()) {
            return it->second;
        }

        // Create a new entry
        auto id = static_cast<SymbolId>(symbols.size());
        symbols.push_back(str);
        symbolToId[str] = id;
        return id;
    }

    // Look up a string by its ID
    const std::string& get(SymbolId id) const {
        if (id < symbols.size()) {
            return symbols[id];
        }
        throw std::runtime_error("Invalid symbol id " + std::to_string(id));
    }

    // Get the number of interned strings
    size_t size() const {
        return symbols.size();
    }
};

#endif // STRING_INTERNER_H
