################################################################################
# Package: VeloAssociators
################################################################################
gaudi_subdir(VeloAssociators v2r5p1)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/VeloEvent
                         GaudiAlg)

gaudi_add_module(VeloAssociators
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Event/VeloEvent
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib)

