################################################################################
# Package: GaussTracker
################################################################################
gaudi_subdir(GaussTracker v7r0p1)

gaudi_depends_on_subdirs(Sim/GaussTools)

gaudi_add_module(GaussTracker
                 src/*.cpp
                 LINK_LIBRARIES GaussToolsLib)

gaudi_env(SET GAUSSTRACKEROPTS \${GAUSSTRACKERROOT}/options)
