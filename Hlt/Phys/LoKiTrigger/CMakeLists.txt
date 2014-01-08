################################################################################
# Package: LoKiTrigger
################################################################################
gaudi_subdir(LoKiTrigger v14r0)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/L0Event
                         Event/TrackEvent
                         Hlt/HltBase
                         Phys/KalmanFilter
                         Phys/LoKiCore
                         Phys/LoKiNumbers
                         Phys/LoKiTracks
                         Tr/TrackInterfaces)

gaudi_add_library(LoKiTriggerLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS Tr/TrackInterfaces
                  LINK_LIBRARIES DetDescLib L0Event TrackEvent HltBase KalmanFilter LoKiCoreLib LoKiTracksLib)

gaudi_add_module(LoKiTrigger
                 src/Components/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES DetDescLib L0Event TrackEvent HltBase KalmanFilter LoKiCoreLib LoKiTracksLib LoKiTriggerLib)

gaudi_add_dictionary(LoKiTrigger
                     dict/LoKiTriggerDict.h
                     dict/LoKiTrigger.xml
                     INCLUDE_DIRS Tr/TrackInterfaces
                     LINK_LIBRARIES DetDescLib L0Event TrackEvent HltBase KalmanFilter LoKiCoreLib LoKiTracksLib LoKiTriggerLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
