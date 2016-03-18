################################################################################# Package: LbBound
################################################################################
gaudi_subdir(LbBound v1r0)

gaudi_depends_on_subdirs(Gen/Generators)

gaudi_add_module(LbBound
                 src/component/*.cpp
                 LINK_LIBRARIES GeneratorsLib HepMC)
