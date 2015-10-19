################################################################################
# Package: MicroDSTTools
################################################################################
gaudi_subdir(MicroDSTTools v2r14)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Event/TrackEvent
                         Event/LinkerEvent
                         MicroDST/MicroDSTBase
                         MicroDST/MicroDSTInterfaces)

find_package(GSL)

gaudi_add_module(MicroDSTTools
                 src/*.cpp
                 INCLUDE_DIRS GSL MicroDST/MicroDSTInterfaces
                 LINK_LIBRARIES GSL MCEvent PhysEvent RecEvent TrackEvent MicroDSTBaseLib LinkerEvent)

