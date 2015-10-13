################################################################################
# Package: PatFitParams
################################################################################
gaudi_subdir(PatFitParams v4r4)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Event/MCEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel)

find_package(AIDA)

gaudi_add_module(PatFitParams
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES LinkerEvent MCEvent TrackEvent GaudiAlgLib GaudiKernel)

