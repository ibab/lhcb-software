################################################################################
# Package: RichRecAlgorithms
################################################################################
gaudi_subdir(RichRecAlgorithms v2r21p1)

gaudi_depends_on_subdirs(Rich/RichRecBase
                         Tr/TrackInterfaces)

gaudi_add_module(RichRecAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES RichRecBase)

