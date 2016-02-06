################################################################################
# Package: FTDAQ
################################################################################
gaudi_subdir(FTDAQ v1r7p1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/FTEvent
                         DAQ/DAQKernel
                         GaudiAlg)

find_package(Boost)

gaudi_add_module(FTDAQ
                 src/*.cpp
                 INCLUDE_DIRS Boost Event/FTEvent
                 LINK_LIBRARIES DAQEventLib DAQKernelLib GaudiAlgLib)

