################################################################################
# Package: STAssociators
################################################################################
gaudi_subdir(STAssociators v1r8p1)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         ST/STKernel)

gaudi_add_module(STAssociators
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib STKernelLib)

gaudi_env(SET STASSOCIATORSOPTS \${STASSOCIATORSROOT}/options)

