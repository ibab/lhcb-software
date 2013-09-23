################################################################################
# Package: CaloMoniSim
################################################################################
gaudi_subdir(CaloMoniSim v3r2)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/MCEvent)

gaudi_add_module(CaloMoniSim
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES CaloDetLib MCEvent)

gaudi_env(SET CALOMONISIMOPTS \${CALOMONISIMROOT}/options)

