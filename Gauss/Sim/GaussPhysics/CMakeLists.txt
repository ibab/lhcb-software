################################################################################
# Package: GaussPhysics
################################################################################
gaudi_subdir(GaussPhysics v11r1p1)

gaudi_depends_on_subdirs(Sim/GaussTools)

FindG4libs(LHCblists physics_lists)

gaudi_add_module(GaussPhysics
                 src/*.cpp
                 LINK_LIBRARIES GaussToolsLib
                                ${GEANT4_LIBS})

