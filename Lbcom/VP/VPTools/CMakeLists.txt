################################################################################
# Package: VPTools
################################################################################
gaudi_subdir(VPTools v2r5p1)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiGSL
                         Tr/LHCbTrackInterfaces)

gaudi_add_module(VPTools
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tr/LHCbTrackInterfaces
                 LINK_LIBRARIES VPDetLib TrackEvent GaudiAlgLib GaudiGSLLib)

