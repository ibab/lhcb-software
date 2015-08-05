################################################################################
# Package: LbSuperChic
################################################################################
gaudi_subdir(LbSuperChic v1r0)

gaudi_depends_on_subdirs(Gen/SuperChic)

gaudi_add_module(LbSuperChic
                 src/component/*.cpp
                 LINK_LIBRARIES SuperChic)

