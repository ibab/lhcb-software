################################################################################
# Package: VeloDAQ
################################################################################
gaudi_subdir(VeloDAQ v6r12p1)

gaudi_depends_on_subdirs(DAQ/Tell1Kernel
                         Det/VeloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/RecEvent
                         Event/VeloEvent
			 DAQ/DAQKernel
                         Si/SiDAQ)

gaudi_add_module(VeloDAQ
                 src/*.cpp
                 INCLUDE_DIRS DAQ/Tell1Kernel Event/DigiEvent Event/VeloEvent Si/SiDAQ DAQ/DAQKernel
                 LINK_LIBRARIES VeloDetLib DAQEventLib DAQKernelLib RecEvent)

gaudi_install_python_modules()

