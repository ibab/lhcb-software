################################################################################
# Package: RichRecAlgorithms
################################################################################
gaudi_subdir(RichRecAlgorithms v2r20)

gaudi_depends_on_subdirs(Rich/RichRecBase)

gaudi_add_module(RichRecAlgorithms
                 src/*.cpp
                 LINK_LIBRARIES RichRecBase)

