################################################################################
# Package: DaVinciTypes
################################################################################
gaudi_subdir(DaVinciTypes v2r15)

gaudi_depends_on_subdirs(Event/PhysEvent
                         Event/RecEvent
                         GaudiAlg
                         Kernel/Relations)

gaudi_add_library(DaVinciTypesLib
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
                  LINK_LIBRARIES PhysEvent RecEvent GaudiAlgLib RelationsLib)

gaudi_add_module(DaVinciTypes
                 tests/src/*.cpp
                 LINK_LIBRARIES PhysEvent RecEvent GaudiAlgLib RelationsLib DaVinciTypesLib)

gaudi_add_dictionary(DaVinciTypes
                     dict/DaVinciTypesDict.h
                     dict/DaVinciTypes.xml
                     LINK_LIBRARIES PhysEvent RecEvent GaudiAlgLib RelationsLib DaVinciTypesLib
                     OPTIONS " -U__MINGW32__ ")


gaudi_add_test(QMTest QMTEST)
