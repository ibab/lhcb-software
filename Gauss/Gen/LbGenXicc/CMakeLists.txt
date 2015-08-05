################################################################################
# Package: LbGenXicc
################################################################################
gaudi_subdir(LbGenXicc v5r0p1)

gaudi_depends_on_subdirs(Gen/GenXicc
                         Gen/LbHard)

gaudi_add_library(LbGenXiccLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GenXicc LbPythiaLib)

gaudi_add_module(LbGenXicc
                 src/component/*.cpp
                 LINK_LIBRARIES GenXicc LbHardLib LbPythiaLib LbGenXiccLib)

