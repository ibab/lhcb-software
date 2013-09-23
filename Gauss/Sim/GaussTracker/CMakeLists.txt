################################################################################
# Package: GaussTracker
################################################################################
gaudi_subdir(GaussTracker v6r2p1)

gaudi_depends_on_subdirs(Sim/GaussTools)

gaudi_add_module(GaussTracker
                 src/*.cpp
                 LINK_LIBRARIES GaussToolsLib)

gaudi_env(SET GAUSSTRACKEROPTS \${GAUSSTRACKERROOT}/options)
