################################################################################
# Package: GaussKine
################################################################################
gaudi_subdir(GaussKine v6r4)

gaudi_depends_on_subdirs(Event/GenEvent
                         Event/MCEvent
                         Phys/LoKiGen
                         Sim/GaussTools)

gaudi_add_module(GaussKine
                 src/*.cpp
                 LINK_LIBRARIES GenEvent MCEvent GaussToolsLib LoKiGenLib)

