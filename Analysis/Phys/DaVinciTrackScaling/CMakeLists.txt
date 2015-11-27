################################################################################
# Package: DaVinciTrackScaling
################################################################################
gaudi_subdir(DaVinciTrackScaling v1r5)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/TrackEvent
                         Det/DetDesc)

gaudi_add_module(DaVinciTrackScaling
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES LHCbMathLib TrackEvent DetDescLib)

gaudi_install_python_modules()
