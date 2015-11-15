################################################################################
# Package: LbSuperChic
################################################################################
gaudi_subdir(LbSuperChic v1r0)

gaudi_depends_on_subdirs(Gen/SuperChic)

find_package(HepMC COMPONENTS fio)

gaudi_add_module(LbSuperChic
                 src/component/*.cpp
                 INCLUDE_DIRS HepMC
                 LINK_LIBRARIES SuperChic HepMC)
