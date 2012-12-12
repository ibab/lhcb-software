################################################################################
# Package: OTSimulation
################################################################################
gaudi_subdir(OTSimulation v5r3)

gaudi_depends_on_subdirs(Det/OTDet
                         Event/MCEvent
                         OT/OTDAQ)

find_package(Boost)
find_package(GSL)

gaudi_add_module(OTSimulation
                 src/*.cpp
                 INCLUDE_DIRS GSL Boost OT/OTDAQ
                 LINK_LIBRARIES GSL Boost OTDetLib MCEvent)

