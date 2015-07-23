################################################################################
# Package: TisTosTobbing
################################################################################
gaudi_subdir(TisTosTobbing v9r1)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/HltEvent
                         Event/L0Event
                         Event/MicroDst
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/HltInterfaces
                         Phys/DaVinciKernel)

find_package(Boost COMPONENTS regex)

gaudi_add_library(TisTosTobbingLib
                  src/lib/*.cpp
                  PUBLIC_HEADERS TisTos
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost CaloDetLib HltEvent L0Event MicroDstLib TrackEvent GaudiAlgLib HltInterfaces DaVinciKernelLib)

gaudi_add_module(TisTosTobbing
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost CaloDetLib HltEvent L0Event MicroDstLib TrackEvent GaudiAlgLib HltInterfaces DaVinciKernelLib TisTosTobbingLib)

