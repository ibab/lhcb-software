################################################################################
# Package: TrackInterfaces
################################################################################
gaudi_subdir(TrackInterfaces v5r0)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiKernel
                         Kernel/LHCbKernel)

gaudi_add_dictionary(TrackInterfaces
                     dict/TrackInterfacesDict.h
                     dict/TrackInterfacesDict.xml
                     LINK_LIBRARIES DetDescLib RecEvent TrackEvent GaudiKernel LHCbKernel
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(TrackInterfaces)

