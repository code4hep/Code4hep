#include "TypeNameConversion.h"

#include <unordered_map>

namespace c4h {

  std::string const& typeNameConversion(std::string const& iRootTypeName) {
    static std::unordered_map<std::string, std::string> s_conversion = {
        {"podio::UserDataCollection<float,void>", "podio::UserDataCollection<float>"},
        {"podio::UserDataCollection<int,void>", "podio::UserDataCollection<int32_t>"}};
    auto itFound = s_conversion.find(iRootTypeName);
    if (itFound != s_conversion.end()) {
      return itFound->second;
    }

    return iRootTypeName;
  }
}  // namespace c4h
