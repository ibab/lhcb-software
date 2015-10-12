################################################################################
# Package: XmlTools
################################################################################
gaudi_subdir(XmlTools v6r8p2)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(XercesC)

gaudi_add_library(XmlToolsLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS XmlTools
                  INCLUDE_DIRS XercesC
                  LINK_LIBRARIES XercesC GaudiKernel)

gaudi_add_module(XmlTools
                 src/component/*.cpp
                 INCLUDE_DIRS XercesC
                 LINK_LIBRARIES XercesC GaudiKernel XmlToolsLib)

gaudi_add_dictionary(XmlTools
                     dict/XmlToolsDict.h
                     dict/XmlToolsDict.xml
                     INCLUDE_DIRS XercesC
                     LINK_LIBRARIES XercesC GaudiKernel XmlToolsLib
                     OPTIONS "-U__MINGW32__")

