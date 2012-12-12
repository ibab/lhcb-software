################################################################################
# Package: RichMCAssociators
################################################################################
gaudi_subdir(RichMCAssociators v2r9)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Kernel/MCInterfaces
                         Rich/RichKernel)

gaudi_add_module(RichMCAssociators
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Kernel/MCInterfaces
                 LINK_LIBRARIES LinkerEvent MCEvent RichKernelLib)

