################################################################################
# Package: LbHard
################################################################################
gaudi_subdir(LbHard v1r0)

gaudi_depends_on_subdirs(Gen/Generators
                         Gen/LbPythia
                         Gen/LbPythia8)

gaudi_add_library(LbHardLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS LbHard
                  LINK_LIBRARIES GeneratorsLib LbPythiaLib LbPythia8Lib)
