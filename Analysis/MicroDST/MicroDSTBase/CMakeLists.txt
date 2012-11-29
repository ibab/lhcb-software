################################################################################
# Package: MicroDSTBase
################################################################################
gaudi_subdir(MicroDSTBase v3r2p1)

gaudi_depends_on_subdirs(GaudiAlg
                         GaudiKernel
                         Phys/DaVinciUtils)

gaudi_add_library(MicroDSTBaseLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS MicroDST
                  INCLUDE_DIRS Phys/DaVinciUtils
                  LINK_LIBRARIES GaudiAlgLib GaudiKernel)

