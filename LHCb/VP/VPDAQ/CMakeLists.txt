################################################################################
# Package: VPDAQ
################################################################################
gaudi_subdir(VPDAQ v2r5)

gaudi_depends_on_subdirs(DAQ/DAQUtils
			 DAQ/DAQKernel	
                         Det/VPDet
                         Det/VeloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/VeloEvent
                         GaudiAlg
                         Kernel/LHCbKernel
                         Si/SiDAQ)

find_package(AIDA)
find_package(GSL)

gaudi_add_module(VPDAQ
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Event/DigiEvent Event/VeloEvent Si/SiDAQ
                 LINK_LIBRARIES GSL VPDetLib VeloDetLib DAQEventLib DAQKernelLib  LinkerEvent MCEvent GaudiAlgLib LHCbKernel)

