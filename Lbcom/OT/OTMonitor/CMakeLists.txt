################################################################################
# Package: OTMonitor
################################################################################
gaudi_subdir(OTMonitor v3r8p1)

gaudi_depends_on_subdirs(Det/OTDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg)

find_package(AIDA)

gaudi_add_module(OTMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES OTDetLib LinkerEvent MCEvent GaudiAlgLib)

