################################################################################
# Package: HltTracking
################################################################################
gaudi_subdir(HltTracking v10r0)

gaudi_depends_on_subdirs(Event/HltEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Hlt/HltBase
                         Hlt/HltCommon
                         Tr/TrackInterfaces)

gaudi_add_module(HltTracking
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES HltEvent TrackEvent GaudiAlgLib GaudiKernel HltBase)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES HltTracking.Hlt2Tracking)
gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
