################################################################################
# Package: RichKernel
################################################################################
gaudi_subdir(RichKernel v7r60)

gaudi_depends_on_subdirs(Event/DAQEvent
                         GaudiAlg
                         GaudiObjDesc
                         GaudiUtils
                         Kernel/LHCbKernel)

message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
find_package(AIDA)
find_package(Boost)
find_package(VDT)

#find_package(ROOT COMPONENTS Vc)

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

#if(GAUDI_BUILD_TESTS)
#  gaudi_add_executable(RichTestSigFunc   
#                       src/tests/TestSigFunc.cpp
#                       INCLUDE_DIRS Boost VDT Vc
#                       LINK_LIBRARIES Boost VDT Vc RichKernelLib)
#endif()

gaudi_install_python_modules()
