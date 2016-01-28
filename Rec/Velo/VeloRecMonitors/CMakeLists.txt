################################################################################
# Package: VeloRecMonitors
################################################################################
gaudi_subdir(VeloRecMonitors v2r34)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/HltEvent
                         Event/L0Event
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/TrackEvent
                         GaudiAlg
                         Tr/TrackFitEvent)

find_package(AIDA)

gaudi_add_module(VeloRecMonitors
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent
                 LINK_LIBRARIES VeloDetLib DAQEventLib HltEvent L0Event LinkerEvent MCEvent TrackEvent GaudiAlgLib TrackFitEvent)

gaudi_install_python_modules()

