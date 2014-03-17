################################################################################
# Package: RichKernel
################################################################################
gaudi_subdir(RichKernel v7r55)

gaudi_depends_on_subdirs(Event/DAQEvent
                         GaudiAlg
                         GaudiObjDesc
                         GaudiUtils
                         Kernel/LHCbKernel)

message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
find_package(AIDA)
find_package(Boost)
find_package(VDT)

gaudi_add_library(RichKernelLib
                  src/lib/*.cpp
                  PUBLIC_HEADERS RichKernel
                  INCLUDE_DIRS AIDA Boost VDT
                  LINK_LIBRARIES Boost VDT DAQEventLib GaudiAlgLib GaudiUtilsLib LHCbKernel)

gaudi_add_module(RichKernel
                 src/component/*.cpp
                 INCLUDE_DIRS AIDA Boost
                 LINK_LIBRARIES Boost DAQEventLib GaudiAlgLib GaudiUtilsLib LHCbKernel RichKernelLib)

gaudi_add_dictionary(RichKernel
                     dict/RichKernelDict.h
                     dict/RichKernelDict.xml
                     INCLUDE_DIRS AIDA Boost
                     LINK_LIBRARIES Boost DAQEventLib GaudiAlgLib GaudiUtilsLib LHCbKernel RichKernelLib
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()

