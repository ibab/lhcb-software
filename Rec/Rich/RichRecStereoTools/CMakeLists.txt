################################################################################
# Package: RichRecStereoTools
################################################################################
gaudi_subdir(RichRecStereoTools v1r9)

gaudi_depends_on_subdirs(Rich/RichRecBase)

gaudi_add_module(RichRecStereoTools
                 src/*.cpp
                 LINK_LIBRARIES RichRecBase)

