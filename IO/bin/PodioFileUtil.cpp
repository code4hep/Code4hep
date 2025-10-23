/**
 *  Utility program to open and read a podio file and
 *  print out the following information about its contents:
 *
 *      - podio and edm4hep build versions used to write the file
 *      - available data models
 *      - data model definition for edm4hep (if requested)
 *      - available Frame categories
 *      - available collections in the first Frame of each category
 *
 *  \author W. David Dagenhart, created 29 October 2025
 *
 */
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <boost/program_options.hpp>

#include "edm4hep/EventHeaderCollection.h"
#include "edm4hep/VertexCollection.h"
#include "nlohmann/json.hpp"
#include "podio/CollectionBase.h"
#include "podio/Frame.h"
#include "podio/Reader.h"

int main(int argc, char* argv[]) {
  std::cout << "Executing podioFileUtil\n" << std::endl;

  boost::program_options::options_description desc("Allowed options");
  desc.add_options()("help,h", "print help message")(
      "file,f", boost::program_options::value<std::string>(), "input podio file")(
      "dumpDataModel,d", "dump the data model definition for edm4hep");

  boost::program_options::positional_options_description p;
  p.add("file", -1);

  boost::program_options::variables_map vm;

  try {
    boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  } catch (boost::program_options::error const& ex) {
    std::cerr << "Command Line Option parsing failure:\n" << ex.what() << "\n\n";
    std::cerr << "Usage: " << argv[0] << " [options] <input_file>\n";
    std::cerr << desc << "\n";
    return 1;
  }

  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [options] <input_file>\n";
    std::cout << desc << "\n";
    return 0;
  }

  std::string inputFile = (vm.count("file") ? vm["file"].as<std::string>() : std::string());

  if (inputFile.empty()) {
    std::cerr << "Error: No input file specified.\n";
    std::cerr << "Usage: " << argv[0] << " [options] <input_file>\n";
    std::cerr << desc << "\n";
    return 1;
  }
  std::cout << "Input file: " << inputFile << "\n" << std::endl;

  bool dumpDataModel = vm.count("dumpDataModel") > 0 ? true : false;

  // Done parsing the command line options, now proceed with reading the file

  try {
    podio::Reader reader = podio::makeReader(inputFile);
    std::cout << "Successfully opened input file and created podio::Reader" << std::endl;
    std::cout << "The build version of podio used to write file: " << reader.currentFileVersion() << std::endl;

    try {
      auto edm4hepversion = reader.currentFileVersion("edm4hep");
      if (edm4hepversion) {
        std::cout << "The build version of edm4hep used to write file: " << edm4hepversion.value() << std::endl;
      }

      auto availableDatamodels = reader.getAvailableDatamodels();
      std::cout << "Reader supports these data models: " << std::endl;
      for (const auto& model : availableDatamodels) {
        std::cout << "    " << model << std::endl;
      }
      if (dumpDataModel) {
        auto datamodelDefinition = reader.getDatamodelDefinition("edm4hep");
        std::cout << "Data model definition for edm4hep: " << std::endl;
        nlohmann::json parsed_json = nlohmann::json::parse(std::string(datamodelDefinition));
        std::string pretty_json_string = parsed_json.dump(2);
        std::cout << pretty_json_string << std::endl;
      }

      std::cout << "Reader has these Frame categories: " << std::endl;
      for (const auto& category : reader.getAvailableCategories()) {
        std::cout << "    " << category << "    which contains " << reader.getEntries(std::string(category))
                  << " entries" << std::endl;
      }

      for (const auto& category : reader.getAvailableCategories()) {
        std::string cat = std::string(category);

        std::size_t eventIndex = 0;
        podio::Frame frame = reader.readFrame(cat, eventIndex);

        std::cout << "\n    Available collections in first \"" << cat
                  << "\" Frame (podioFileUtil only reads the first entry): " << std::endl;
        for (const auto& collection : frame.getAvailableCollections()) {
          std::cout << "        collection name = " << std::left << std::setw(40) << collection;
          const podio::CollectionBase* collectionBase = frame.get(collection);
          std::cout << "    collection type = " << collectionBase->getTypeName() << std::endl;

          if (collection == "EventHeader") {
            auto eventHeaderCollection = dynamic_cast<const edm4hep::EventHeaderCollection*>(collectionBase);
            if (eventHeaderCollection && !eventHeaderCollection->empty()) {
              const auto& eventHeader = eventHeaderCollection->at(0);
              std::cout << "            event number = " << eventHeader.getEventNumber() << std::endl;
              std::cout << "            run number = " << eventHeader.getRunNumber() << std::endl;
              std::cout << "            timestamp = " << eventHeader.getTimeStamp() << std::endl;
              std::cout << "            weight = " << eventHeader.getWeight() << std::endl;
            }
          }

          if (collection == "VertexCollection") {
            std::cout << "            (podioFileUtil is not designed to print out the contents all data products or "
                         "data types."
                      << std::endl;
            std::cout << "            It prints some content from VertexCollection just to demonstrate it is possible "
                         "to access a data product.)"
                      << std::endl;
            auto vertexCollection = dynamic_cast<const edm4hep::VertexCollection*>(collectionBase);
            if (vertexCollection && !vertexCollection->empty()) {
              std::cout << "            number of vertices = " << vertexCollection->size() << std::endl;
              const auto& vertex = vertexCollection->at(0);
              std::cout << "            first vertex position = (" << vertex.getPosition()[0] << ", "
                        << vertex.getPosition()[1] << ", " << vertex.getPosition()[2] << ")" << std::endl;
            }
          }
        }
      }
    } catch (const std::exception& ex) {
      std::cerr << "Error: Exception while using podio::Reader to get data from the input file\n"
                << ex.what() << std::endl;
      return 1;
    }
  } catch (const std::exception& ex) {
    std::cerr << "Error: Could not open input file or failed creating podio::Reader\n" << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
