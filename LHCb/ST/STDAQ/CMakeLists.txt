################################################################################
# Package: STDAQ
################################################################################
gaudi_subdir(STDAQ v4r7)

gaudi_depends_on_subdirs(DAQ/DAQUtils
                         Det/DetDesc
                         Det/STDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/RecEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         ST/STKernel
                         ST/STTELL1Event
                         Si/SiDAQ)


gaudi_add_module(STDAQ
                 src/*.cpp
                 INCLUDE_DIRS DAQ/DAQUtils Event/DigiEvent Si/SiDAQ
                 LINK_LIBRARIES DetDescLib STDetLib DAQEventLib RecEvent GaudiAlgLib GaudiKernel LHCbKernel STKernelLib STTELL1Event)

