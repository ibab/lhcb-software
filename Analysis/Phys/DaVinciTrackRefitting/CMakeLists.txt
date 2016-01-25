################################################################################
# Package: DaVinciTrackRefitting
################################################################################
gaudi_subdir(DaVinciTrackRefitting v2r3)

gaudi_depends_on_subdirs(Event/TrackEvent
                         Kernel/LHCbKernel
                         Phys/DaVinciKernel
                         Phys/DecayTreeFitter
                         Phys/LoKiCore
                         Phys/LoKiPhys
                         Tr/TrackFitEvent
                         Tr/TrackKernel)

find_package(Boost)

gaudi_add_module(DaVinciTrackFitting
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost TrackEvent LHCbKernel DaVinciKernelLib DecayTreeFitter LoKiCoreLib LoKiPhysLib TrackFitEvent TrackKernel)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
