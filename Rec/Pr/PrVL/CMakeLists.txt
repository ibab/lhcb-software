################################################################################
# Package: PrVL
################################################################################
gaudi_subdir(PrVL v1r1)

gaudi_depends_on_subdirs(Det/VLDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         Pr/PrKernel)

gaudi_add_module(PrVL
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES VLDetLib TrackEvent GaudiAlgLib PrKernel)

