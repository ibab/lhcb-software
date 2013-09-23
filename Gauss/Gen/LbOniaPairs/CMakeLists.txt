################################################################################
# Package: LbOniaPairs
################################################################################
gaudi_subdir(LbOniaPairs v1r2)

gaudi_depends_on_subdirs(Gen/GENSER
                         Gen/LbPythia)

find_package(Boost)

gaudi_add_library(LbOniaPairsLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost pythia6forgauss LbPythiaLib)

gaudi_add_module(LbOniaPairs
                 src/Component/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost pythia6forgauss LbPythiaLib LbOniaPairsLib)

