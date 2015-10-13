################################################################################
# Package: RichENNRingFinder
################################################################################
gaudi_subdir(RichENNRingFinder v1r5p2)

gaudi_depends_on_subdirs(Rich/RichRecBase)

gaudi_add_module(RichENNRingFinder
                 src/*.cpp
                 LINK_LIBRARIES RichRecBase)

gaudi_install_python_modules()

