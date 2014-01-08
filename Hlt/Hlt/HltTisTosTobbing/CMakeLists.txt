################################################################################
# Package: HltTisTosTobbing
################################################################################
gaudi_subdir(HltTisTosTobbing v2r0p1)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/HltEvent
                         Event/TrackEvent
                         GaudiAlg
                         Hlt/HltBase
                         Kernel/HltInterfaces
                         Phys/DaVinciKernel
                         Phys/TisTosTobbing)

find_package(Boost COMPONENTS regex)

gaudi_add_module(HltTisTosTobbing
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost CaloDetLib HltEvent TrackEvent GaudiAlgLib HltBase HltInterfaces DaVinciKernelLib TisTosTobbingLib)


gaudi_add_test(QMTest QMTEST)
