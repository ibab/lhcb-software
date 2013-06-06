################################################################################
# Package: OTSimulation
################################################################################
gaudi_subdir(OTSimulation v5r5)

gaudi_depends_on_subdirs(Det/OTDet
                         Event/MCEvent
                         OT/OTDAQ)

find_package(Boost)
find_package(GSL)
find_package(AIDA)

gaudi_add_module(OTSimulation
                 src/*.cpp
                 INCLUDE_DIRS GSL Boost OT/OTDAQ AIDA
                 LINK_LIBRARIES GSL Boost OTDetLib MCEvent)

