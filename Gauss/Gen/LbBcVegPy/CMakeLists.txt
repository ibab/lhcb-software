################################################################################
# Package: LbBcVegPy
################################################################################
gaudi_subdir(LbBcVegPy v5r0)

gaudi_depends_on_subdirs(Gen/BcVegPy
                         Gen/LbPythia)

gaudi_add_library(LbBcVegPyLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES BcVegPy LbPythiaLib)

gaudi_add_module(LbBcVegPy
                 src/component/*.cpp
                 LINK_LIBRARIES BcVegPy LbPythiaLib LbBcVegPyLib)

