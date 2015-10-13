################################################################################
# Package: TrackCheckers
################################################################################
gaudi_subdir(TrackCheckers v4r22p1)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiUtils
                         Kernel/MCInterfaces
                         Tf/PatKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)

gaudi_add_module(TrackCheckers
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Kernel/MCInterfaces Tf/PatKernel Tr/TrackInterfaces
                 LINK_LIBRARIES GSL LinkerEvent MCEvent RecEvent TrackEvent GaudiAlgLib GaudiUtilsLib TrackFitEvent TrackKernel)

