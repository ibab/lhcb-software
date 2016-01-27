################################################################################
# Package: STCheckers
################################################################################
gaudi_subdir(STCheckers v4r5)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         GaudiAlg
                         Kernel/MCInterfaces
                         ST/STKernel)

find_package(AIDA)

gaudi_add_module(STCheckers
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent Kernel/MCInterfaces
                 LINK_LIBRARIES STDetLib LinkerEvent MCEvent RecEvent GaudiAlgLib STKernelLib)

