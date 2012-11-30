################################################################################
# Package: MicroDSTAlgorithm
################################################################################
gaudi_subdir(MicroDSTAlgorithm v5r7)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/HltEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/MicroDst
                         Event/PhysEvent
                         Event/RecEvent
                         Event/SwimmingEvent
                         MicroDST/MicroDSTBase
                         MicroDST/MicroDSTInterfaces
                         MicroDST/MicroDSTTools
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel)

gaudi_add_module(MicroDSTAlgorithm
                 src/*.cpp
                 INCLUDE_DIRS Event/SwimmingEvent MicroDST/MicroDSTInterfaces
                 LINK_LIBRARIES DAQEventLib HltEvent LinkerEvent MCEvent MicroDstLib PhysEvent RecEvent MicroDSTBaseLib DaVinciKernelLib DaVinciMCKernelLib)

