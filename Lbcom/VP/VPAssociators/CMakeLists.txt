################################################################################
# Package: VPAssociators
################################################################################
gaudi_subdir(VPAssociators v2r7)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

gaudi_add_module(VPAssociators
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent 
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib LHCbKernel)

