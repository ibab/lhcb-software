################################################################################
# Package: LbGenXicc
################################################################################
gaudi_subdir(LbGenXicc v2r0p1)

gaudi_depends_on_subdirs(Gen/GenXicc
                         Gen/LbPythia)

gaudi_add_library(LbGenXiccLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GenXicc LbPythiaLib)

gaudi_add_module(LbGenXicc
                 src/component/*.cpp
                 LINK_LIBRARIES GenXicc LbPythiaLib LbGenXiccLib)

