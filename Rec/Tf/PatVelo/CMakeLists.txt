################################################################################
# Package: PatVelo
################################################################################
gaudi_subdir(PatVelo v3r26)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/LHCbMath
                         Tf/PatKernel
                         Tf/TfKernel
                         Tf/TfTools
                         Tr/TrackInterfaces)

gaudi_add_module(PatVelo
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tf/PatKernel Tf/TfKernel Tr/TrackInterfaces
                 LINK_LIBRARIES VeloDetLib TrackEvent GaudiAlgLib LHCbMathLib)

