################################################################################
# Package: PrFitParams
################################################################################
gaudi_subdir(PrFitParams v2r0p1)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/LinkerEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel)

find_package(AIDA)

gaudi_add_module(PrFitParams
                 ../src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES GaudiAlgLib GaudiKernel MCEvent LinkerEvent
                                TrackEvent)
