#include <optional>
#include <unordered_map>
#include <string>
#include "json.hpp"

class KeyValueStore {
  private:
    std::unordered_map<std::string, nlohmann::json> kvs;
  public:
    std::optional<nlohmann::json> get(const std::string& key);
    void put(const std::string& key, const nlohmann::json& value);
    void deleteKey(const std::string& key);
};
