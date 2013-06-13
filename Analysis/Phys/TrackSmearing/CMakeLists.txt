################################################################################
# Package: TrackSmearing
################################################################################
gaudi_subdir(TrackSmearing v1r6p1)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Phys/DaVinciKernel)

gaudi_add_module(TrackSmearing
                 src/*.cpp
                 LINK_LIBRARIES LinkerEvent DaVinciKernelLib)

