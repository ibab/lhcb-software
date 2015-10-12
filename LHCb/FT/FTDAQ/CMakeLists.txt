################################################################################
# Package: FTDAQ
################################################################################
gaudi_subdir(FTDAQ v1r7p1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/FTEvent
			 DAQ/DAQKernel
                         GaudiAlg)

gaudi_add_module(FTDAQ
                 src/*.cpp
                 INCLUDE_DIRS Event/FTEvent
                 LINK_LIBRARIES DAQEventLib DAQKernelLib GaudiAlgLib)

