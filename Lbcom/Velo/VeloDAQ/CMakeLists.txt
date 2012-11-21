################################################################################
# Package: VeloDAQ
################################################################################
gaudi_subdir(VeloDAQ v6r7)

gaudi_depends_on_subdirs(DAQ/Tell1Kernel
                         Det/VeloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/RecEvent
                         Event/VeloEvent
                         Si/SiDAQ)

gaudi_add_module(VeloDAQ
                 src/*.cpp
                 INCLUDE_DIRS DAQ/Tell1Kernel Event/DigiEvent Event/VeloEvent Si/SiDAQ
                 LINK_LIBRARIES VeloDetLib DAQEventLib RecEvent)

gaudi_install_python_modules()

