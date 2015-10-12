################################################################################
# Package: OTAssociators
################################################################################
gaudi_subdir(OTAssociators v4r6p1)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(OTAssociators
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib)

gaudi_env(SET OTASSOCIATORSOPTS \${OTASSOCIATORSROOT}/options)

