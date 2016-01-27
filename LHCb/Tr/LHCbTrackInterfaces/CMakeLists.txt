################################################################################
# Package: LHCbTrackInterfaces
################################################################################
gaudi_subdir(LHCbTrackInterfaces v2r3)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiKernel
                         Kernel/LHCbKernel)

gaudi_add_dictionary(LHCbTrackInterfaces
                     dict/LHCbTrackInterfacesDict.h
                     dict/LHCbTrackInterfacesDict.xml
                     LINK_LIBRARIES DetDescLib RecEvent TrackEvent GaudiKernel LHCbKernel
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(TrackInterfaces)

