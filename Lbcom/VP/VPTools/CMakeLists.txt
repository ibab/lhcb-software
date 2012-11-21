################################################################################
# Package: VPTools
################################################################################
gaudi_subdir(VPTools v2r0)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiGSL
                         Tr/TrackInterfaces)

gaudi_add_module(VPTools
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tr/TrackInterfaces
                 LINK_LIBRARIES VPDetLib TrackEvent GaudiAlgLib GaudiGSLLib)

