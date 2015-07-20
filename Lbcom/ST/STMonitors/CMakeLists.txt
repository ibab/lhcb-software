################################################################################
# Package: STMonitors
################################################################################
gaudi_subdir(STMonitors v4r3)

gaudi_depends_on_subdirs(Det/DDDB
                         Event/RecEvent
                         GaudiAlg
                         GaudiConf
                         ST/STKernel
                         ST/STTELL1Event)

find_package(AIDA)

gaudi_add_module(STMonitors
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES RecEvent GaudiAlgLib STKernelLib STTELL1Event)

