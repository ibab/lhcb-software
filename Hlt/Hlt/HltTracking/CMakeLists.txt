################################################################################
# Package: HltTracking
################################################################################
gaudi_subdir(HltTracking v13r9)

gaudi_depends_on_subdirs(Event/HltEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/Relations
                         Hlt/HltBase
                         Hlt/HltCommon
                         Hlt/HltLine
                         Tr/TrackInterfaces)

gaudi_add_module(HltTracking
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES HltEvent RelationsLib TrackEvent GaudiAlgLib GaudiKernel HltBase)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES HltTracking.Hlt2Tracking HltTracking.HltRecoConf)
gaudi_install_python_modules()
add_dependencies(HltTrackingConfUserDB HltLineConf)

gaudi_add_test(QMTest QMTEST)
