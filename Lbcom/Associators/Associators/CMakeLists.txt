################################################################################
# Package: Associators
################################################################################
gaudi_subdir(Associators v3r9p1)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(Associators
                 src/*.cpp
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib)

