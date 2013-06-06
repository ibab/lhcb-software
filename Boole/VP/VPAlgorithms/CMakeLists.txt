################################################################################
# Package: VPAlgorithms
################################################################################
gaudi_subdir(VPAlgorithms v3r3)

gaudi_depends_on_subdirs(Det/VPDet
                         Det/VeloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/VeloEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

find_package(GSL)

gaudi_add_module(VPAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Event/DigiEvent Event/VeloEvent
                 LINK_LIBRARIES GSL VPDetLib VeloDetLib DAQEventLib LinkerEvent MCEvent GaudiAlgLib LHCbKernel)

