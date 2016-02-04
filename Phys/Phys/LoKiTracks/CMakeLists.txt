################################################################################
# Package: LoKiTracks
################################################################################
gaudi_subdir(LoKiTracks v3r5)

gaudi_depends_on_subdirs(Event/TrackEvent
                         Phys/KalmanFilter
                         Phys/LoKiCore
                         Phys/LoKiUtils
                         Tr/TrackInterfaces)

gaudi_add_library(LoKiTracksLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS Tr/TrackInterfaces
                  LINK_LIBRARIES TrackEvent KalmanFilter LoKiCoreLib LoKiUtils)

gaudi_add_module(LoKiTracks
                 src/Components/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES TrackEvent KalmanFilter LoKiCoreLib LoKiUtils LoKiTracksLib)

gaudi_add_dictionary(LoKiTracks
                     dict/LoKiTracksDict.h
                     dict/LoKiTracks.xml
                     INCLUDE_DIRS Tr/TrackInterfaces
                     LINK_LIBRARIES TrackEvent KalmanFilter LoKiCoreLib LoKiUtils LoKiTracksLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()

