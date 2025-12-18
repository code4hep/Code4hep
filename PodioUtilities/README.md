# Code4hep/PodioUtilities Documentation

## Introduction

This package contains code to allow adaption between the podio data model and the framework's data model.
The package has a plugin system that handles translation between podio::CollectionBase and the edm::Wrapper
classes.

## Declaring a new podio based collection

For each podio based collection which needs to be made available in the framework, one must
* add a `plugins` sub-directory of the package which declares the ROOT dictionaries for the `edm::Wrapper<>`.
* in the `plugins` directory create a file containing the declaration of the type and any associated types
* in the `plugins` directory add a BuildFile.xml file that declares that a plugin needs to be built

### declaring types
A series of helper macros can be found in `Code4hep/PodioUtilities/interface/CollectionMacros.h`

#### C4H_COLLECTION macro
This macro creates a plugin specifically for the podio collection. The macro takes one argument which is the
C++ class of the collection.

#### C4H_CONTAINED_CLASS macro
podio collections internally have other classes which they use. Each such class must be declared using the macro.
The three arguments of the macro are
* C++ class of the collection
* string name podio uses for the contained C++ class type
* the compiler used name for the contained C++ class type (usually the same as the 2nd argument)

If a contained type is missing, when the program is run an exception will be thrown that says what the
collection was and what the missing type's name is.

#### example file
```C++
#include "Code4hep/PodioUtilities/interface/CollectionMacros.h"
#include "edm4hep/TrackCollection.h"
#include "edm4hep/TrackData.h"

C4H_COLLECTION(edm4hep::TrackCollection);

C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "std::int32_t", std::int32_t);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "edm4hep::TrackState", edm4hep::TrackState);
C4H_CONTAINED_CLASS(edm4hep::TrackCollection, "edm4hep::TrackData", edm4hep::TrackData);
```
### contents of BuildFile.xml

The BuildFile.xml needs to declare the dependency on
* `Code4hep/PodioUtilities`
* `podio`
* `python3`
* the package which contains the code for the collection

When specify this is a `library` to be built, one must also give a unique `name` for the library. Often the name is is just then name of the containing package with the extension `Plugin` added.

#### example BuildFile.xml
```xml
<use name="Code4hep/PodioUtilities"/>
<use name="podio"/>
<use name="edm4hep"/>
<use name="python3"/>
<library file="*.cc" name="Code4hepDataFormatsPlugins">
  <flags EDM_PLUGIN="1"/>
</library>
```

### Testing collection plugin

You can use the program `c4hCheckCollection` to check that a collection has been properly registered. The program takes the name of the collection to check as its only required input. If any part of the registration is missing, the program will report a problem.

```bash
> c4hCheckCollection edm4hep::TrackCollection
```

## Using the plugin

To get access to the plugin, one must use the appropriate plugin factory: `CollectionWrapperConverterBaseFactory`.
```C++
#include "Code4hep/PodioUtilities/interface/CollectionWrapperConverterBaseFactory.h"
...

  auto factory = CollectionWrapperConverterBaseFactory::get();

  auto converter = factory->create("podio::TrackCollection");
```

For the available abilities of the returned `CollectionWrapperConverterBase` object see the header file `Code4hep/PodioUtilities/interface/CollectionWrapperConverterBase.h`.