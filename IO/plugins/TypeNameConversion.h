#ifndef Code4hep_IO_TypeNameConversion_h
#define Code4hep_IO_TypeNameConversion_h

#include <string>

namespace c4h {
  // Convert a typename from the name used in a ProductDescription
  // to the name expected by the CollectionWrapperConverterBaseFactory.
  // In most cases, these are the same, but in some cases they differ,
  // e.g. for UserDataCollection.
  std::string const& typeNameConversion(std::string const&);
}  // namespace c4h
#endif
