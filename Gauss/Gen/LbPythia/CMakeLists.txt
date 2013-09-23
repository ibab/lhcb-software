################################################################################
# Package: LbPythia
################################################################################
gaudi_subdir(LbPythia v14r1)

gaudi_depends_on_subdirs(Gen/Generators
                         Gen/GENSER)

find_package(Boost COMPONENTS filesystem system)
find_package(HepMC COMPONENTS fio)

include(GENSER_headers)

gaudi_add_library(LbPythiaLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GeneratorsLib HepMC)

gaudi_add_module(LbPythia
                 src/component/*.cpp
                 LINK_LIBRARIES GeneratorsLib LbPythiaLib)

