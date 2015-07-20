################################################################################
# Package: HCDAQ
################################################################################
gaudi_subdir(HCDAQ v1r4)

gaudi_depends_on_subdirs(DAQ/DAQUtils
			 DAQ/DAQKernel	
                         Event/DAQEvent
                         Event/DigiEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

find_package(AIDA)

gaudi_add_module(HCDAQ
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES DAQEventLib DAQKernelLib GaudiAlgLib LHCbKernel)

