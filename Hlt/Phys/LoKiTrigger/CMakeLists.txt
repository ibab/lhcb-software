################################################################################
# Package: LoKiTrigger
################################################################################
gaudi_subdir(LoKiTrigger v15r8)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/L0Event
                         Event/TrackEvent
                         Kernel/Relations
                         Hlt/HltBase
                         Phys/DaVinciKernel
                         Phys/KalmanFilter
                         Phys/LoKiCore
                         Phys/LoKiNumbers
                         Phys/LoKiTracks
                         Phys/LoKiArrayFunctors
                         Tr/TrackInterfaces)

gaudi_add_library(LoKiTriggerLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS Tr/TrackInterfaces
                  LINK_LIBRARIES DetDescLib L0Event TrackEvent HltBase KalmanFilter DaVinciKernelLib LoKiCoreLib LoKiTracksLib LoKiPhysLib LoKiArrayFunctorsLib)

gaudi_add_module(LoKiTrigger
                 src/Components/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES DetDescLib L0Event TrackEvent RelationsLib HltBase KalmanFilter DaVinciKernelLib LoKiCoreLib LoKiTracksLib LoKiTriggerLib)

gaudi_add_dictionary(LoKiTrigger
                     dict/LoKiTriggerDict.h
                     dict/LoKiTrigger.xml
                     INCLUDE_DIRS Tr/TrackInterfaces
                     LINK_LIBRARIES DetDescLib L0Event TrackEvent HltBase KalmanFilter DaVinciKernelLib LoKiCoreLib LoKiTracksLib LoKiTriggerLib
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
