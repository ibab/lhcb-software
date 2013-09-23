################################################################################
# Package: GaussKine
################################################################################
gaudi_subdir(GaussKine v6r3)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         Sim/GaussTools)

gaudi_add_module(GaussKine
                 src/*.cpp
                 LINK_LIBRARIES GenEvent MCEvent GaussToolsLib)

