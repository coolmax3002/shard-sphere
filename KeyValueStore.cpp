#include "KeyValueStore.hpp"
#include <optional>

std::optional<nlohmann::json> KeyValueStore::get(const std::string& key) {
  auto it = kvs.find(key);
  if (it != kvs.end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

void KeyValueStore::put(const std::string& key, const nlohmann::json& value) {
  kvs[key] = value;
}

void KeyValueStore::deleteKey(const std::string& key) {
  kvs.erase(key);
}
