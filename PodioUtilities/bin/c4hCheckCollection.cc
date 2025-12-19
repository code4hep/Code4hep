#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBaseFactory.h"
#include "FWCore/PluginManager/interface/PluginManager.h"
#include "FWCore/PluginManager/interface/standard.h"
#include <boost/program_options.hpp>
#include <iostream>

int main(int argc, char** argv) {
  using namespace boost::program_options;

  static const char* const kCollectionOpt = "collection";
  static const char* const kHelpOpt = "help";
  static const char* const kHelpCommandOpt = "help,h";

  std::string descString(argv[0]);
  descString += " [options] <collection name>";
  descString += "\n  where <collection name> is the name of of podio collection";
  descString += "\nAllowed options";
  options_description desc(descString);
  desc.add_options()(kHelpCommandOpt, "produce help message");

  options_description hidden;
  hidden.add_options()(kCollectionOpt, value<std::string>(), "the collection name to check");

  //full list of options for the parser
  options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);

  positional_options_description p;
  p.add(kCollectionOpt, -1);

  variables_map vm;
  try {
    store(command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
    notify(vm);
  } catch (const error& iException) {
    std::cerr << iException.what();
    return 1;
  }

  if (vm.count(kHelpOpt)) {
    std::cout << desc << std::endl;
    return 0;
  }

  std::string collection;
  if (vm.count(kCollectionOpt)) {
    try {
      collection = vm[kCollectionOpt].as<std::string>();
    } catch (boost::bad_any_cast const& e) {
      std::cout << e.what() << std::endl;
      return 2;
    }
  } else {
    std::cout << "Collection name not specified." << std::endl;
    std::cout << desc << std::endl;
    return 2;
  }

  try {
    edmplugin::PluginManager::configure(edmplugin::standard::config());

    auto factory = code4hep::CollectionWrapperConverterBaseFactory::get();

    auto converter = factory->create(collection);

    if (converter == nullptr) {
      std::cout << "Failed to find collection '" << collection << "'" << std::endl;
      return 1;
    }
    std::cout << "found collection '" << collection << "'" << std::endl;

    auto empty = converter->createEmpty();

    if (not empty) {
      std::cout << "Failed to create an empty container." << std::endl;
      return 1;
    }

    if (collection != empty->getTypeName()) {
      std::cout << "The type named registered is '" << collection
                << "' which does not match the name used by the type itself: '" << empty->getTypeName() << "'"
                << std::endl;
      return 1;
    } else {
      std::cout << " Internal collection type name matches registered name." << std::endl;
    }

    auto c = converter->copy(*empty);

    std::cout << " Copying collection appears to work." << std::endl;
  } catch (std::exception const& except) {
    std::cout << "An exception was thrown \n" << except.what() << std::endl;
    return 2;
  }

  return 0;
}
