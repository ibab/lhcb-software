################################################################################
# Package: LbHijing
################################################################################
gaudi_subdir(LbHijing v9r0)

gaudi_depends_on_subdirs(Gen/GENSER
                         Gen/LbPythia)

gaudi_add_library(LbHijingLib
                 src/Lib/*.cpp src/Lib/*.F
                 PUBLIC_HEADERS LbHijing
                 LINK_LIBRARIES pythia6forgauss LbPythiaLib)

gaudi_add_module(LbHijing
                 src/component/*.cpp
                 LINK_LIBRARIES LbHijingLib)

