################################################################################
# Package: PatVelo
################################################################################
gaudi_subdir(PatVelo v3r32p1)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/LHCbMath
                         Tf/PatKernel
                         Tf/TfKernel
                         Tf/TfTools
                         Tr/TrackInterfaces)

find_package(AIDA)

gaudi_add_module(PatVelo
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tf/PatKernel Tf/TfKernel Tr/TrackInterfaces AIDA
                 LINK_LIBRARIES VeloDetLib TrackEvent GaudiAlgLib LHCbMathLib)

