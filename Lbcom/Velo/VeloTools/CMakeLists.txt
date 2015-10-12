################################################################################
# Package: VeloTools
################################################################################
gaudi_subdir(VeloTools v4r2p1)

gaudi_depends_on_subdirs(DAQ/Tell1Kernel
                         Det/VeloDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel)

gaudi_add_module(VeloTools
                 src/*.cpp
                 INCLUDE_DIRS DAQ/Tell1Kernel Event/DigiEvent
                 LINK_LIBRARIES VeloDetLib TrackEvent GaudiAlgLib GaudiKernel LHCbKernel)

