################################################################################
# Package: CaloMoniDigi
################################################################################
gaudi_subdir(CaloMoniDigi v2r6p2)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/DigiEvent
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(CaloMoniDigi
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES CaloDetLib MCEvent GaudiAlgLib)

gaudi_env(SET CALOMONIDIGIOPTS \${CALOMONIDIGIROOT}/options)

