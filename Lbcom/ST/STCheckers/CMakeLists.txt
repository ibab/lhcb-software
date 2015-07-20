################################################################################
# Package: STCheckers
################################################################################
gaudi_subdir(STCheckers v4r3)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         GaudiAlg
                         Kernel/MCInterfaces
                         ST/STKernel)

find_package(AIDA)
find_package(GSL)

gaudi_add_module(STCheckers
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Event/DigiEvent Kernel/MCInterfaces
                 LINK_LIBRARIES GSL STDetLib LinkerEvent MCEvent RecEvent GaudiAlgLib STKernelLib)

