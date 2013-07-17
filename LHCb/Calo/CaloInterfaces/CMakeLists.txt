################################################################################
# Package: CaloInterfaces
################################################################################
gaudi_subdir(CaloInterfaces v8r6)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/DigiEvent
                         Event/L0Event
                         Event/RecEvent
                         GaudiKernel
                         Kernel/LHCbKernel)

gaudi_add_library(CaloInterfaces
                  src/*.cpp
                  PUBLIC_HEADERS CaloInterfaces
                  INCLUDE_DIRS Event/DigiEvent
                  LINK_LIBRARIES CaloDetLib L0Event RecEvent GaudiKernel LHCbKernel)

gaudi_add_dictionary(CaloInterfaces
                     dict/CaloInterfacesDict.h
                     dict/CaloInterfacesDict.xml
                     INCLUDE_DIRS Event/DigiEvent
                     LINK_LIBRARIES CaloDetLib L0Event RecEvent GaudiKernel LHCbKernel CaloInterfaces
                     OPTIONS "-U__MINGW32__")

