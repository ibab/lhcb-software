################################################################################
# Package: DAQKernel
################################################################################
gaudi_subdir(DAQKernel v1r1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         GaudiAlg
                         GaudiUtils
                         Kernel/LHCbKernel)

message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

gaudi_add_library(DAQKernelLib
                  src/lib/*.cpp
                  PUBLIC_HEADERS DAQKernel
                  LINK_LIBRARIES DAQEventLib GaudiAlgLib GaudiUtilsLib LHCbKernel)


