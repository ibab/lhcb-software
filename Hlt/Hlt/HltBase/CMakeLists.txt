################################################################################
# Package: HltBase
################################################################################
gaudi_subdir(HltBase v15r7)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/HltEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiObjDesc
                         Kernel/HltInterfaces
                         Kernel/LHCbMath
                         Phys/LoKiCore)

find_package(Boost COMPONENTS regex)

gaudi_add_library(HltBase
                  src/*.cpp
                  PUBLIC_HEADERS Event HltBase
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost DetDescLib HltEvent RecEvent TrackEvent GaudiAlgLib HltInterfaces LHCbMathLib LoKiCoreLib)

gaudi_add_dictionary(HltBase
                     dict/HltBaseDict.h
                     dict/HltBaseDict.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost DetDescLib HltEvent RecEvent TrackEvent GaudiAlgLib HltInterfaces LHCbMathLib LoKiCoreLib HltBase
                     OPTIONS "-U__MINGW32__")


gaudi_add_test(QMTest QMTEST)
