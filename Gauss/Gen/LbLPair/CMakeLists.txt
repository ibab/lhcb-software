################################################################################# Package: LbLPair
################################################################################
gaudi_subdir(LbLPair v1r0)

gaudi_depends_on_subdirs(Gen/LPair)

find_package(HepMC COMPONENTS fio)

gaudi_add_module(LbLPair
                 src/component/*.cpp
                 LINK_LIBRARIES LPair HepMC)

