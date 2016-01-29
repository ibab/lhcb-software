################################################################################
# Package: CaloDigit
################################################################################
gaudi_subdir(CaloDigit v3r5)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/L0Event
                         Event/MCEvent
                         GaudiUtils)

gaudi_add_module(CaloDigit
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES CaloDetLib DAQEventLib L0Event MCEvent GaudiUtilsLib)

