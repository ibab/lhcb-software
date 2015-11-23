################################################################################
# Package: PrMCTools
################################################################################
gaudi_subdir(PrMCTools v2r11p1)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/FTEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         Pr/PrKernel
                         Tr/TrackInterfaces)

gaudi_add_module(PrMCTools
                 src/*.cpp
                 INCLUDE_DIRS Event/FTEvent
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib PrKernel VPDetLib LoKiMCLib)

