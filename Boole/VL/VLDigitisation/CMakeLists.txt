################################################################################
# Package: VLDigitisation
################################################################################
gaudi_subdir(VLDigitisation v1r5)

gaudi_depends_on_subdirs(Det/VLDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         GaudiGSL)

gaudi_add_module(VLDigitisation
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES VLDetLib LinkerEvent MCEvent GaudiAlgLib GaudiGSLLib)

