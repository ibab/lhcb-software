################################################################################
# Package: VeloAlgorithms
################################################################################
gaudi_subdir(VeloAlgorithms v5r7p2)

gaudi_depends_on_subdirs(DAQ/Tell1Kernel
                         Det/VeloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/VeloEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

find_package(AIDA)
find_package(Boost)

gaudi_add_module(VeloAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS AIDA DAQ/Tell1Kernel Event/DigiEvent Event/VeloEvent
                 LINK_LIBRARIES VeloDetLib DAQEventLib LinkerEvent MCEvent GaudiAlgLib LHCbKernel)

