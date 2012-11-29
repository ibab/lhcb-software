################################################################################
# Package: MvaPreSelB2DH
################################################################################
gaudi_subdir(MvaPreSelB2DH v1r3)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/L0Event
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Phys/DaVinciKernel
                         Phys/PhysDict)

find_package(AIDA)

gaudi_add_module(MvaPreSelB2DH
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES DAQEventLib L0Event LinkerEvent MCEvent PhysEvent RecEvent DaVinciKernelLib)

