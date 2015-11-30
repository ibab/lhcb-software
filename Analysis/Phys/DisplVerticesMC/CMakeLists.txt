################################################################################
# Package: DisplVerticesMC
################################################################################
gaudi_subdir(DisplVerticesMC v1r5)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/GenEvent
                         Event/HltEvent
                         Event/MCEvent
                         Kernel/HltInterfaces
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel
                         Tr/TrackInterfaces)

find_package(HepMC)

gaudi_add_module(DisplVerticesMC
                 src/*.cpp
                 INCLUDE_DIRS AIDA Tr/TrackInterfaces
                 LINK_LIBRARIES CaloDetLib GenEvent HltEvent MCEvent HltInterfaces DaVinciKernelLib DaVinciMCKernelLib)

