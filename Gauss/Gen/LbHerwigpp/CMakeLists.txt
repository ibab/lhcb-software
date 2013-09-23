################################################################################
# Package: LbHerwigpp
################################################################################
gaudi_subdir(LbHerwigpp v2r0)

gaudi_depends_on_subdirs(Gen/Generators)

find_package(ThePEG)
find_package(Herwig++)

gaudi_add_module(LbHerwigpp
                 src/component/*.cpp
                 INCLUDE_DIRS ThePEG
                 LINK_LIBRARIES ThePEG GeneratorsLib)

string(REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
gaudi_add_library(LbHerwigppRandom
                  src/Lib/*.cpp
                  NO_PUBLIC_HEADERS)

