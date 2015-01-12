################################################################################
# Package: OTOnlineCalibration
################################################################################
gaudi_subdir(OTOnlineCalibration v1r0)

gaudi_depends_on_subdirs(Event/TrackEvent
                         Event/DAQEvent
                         GaudiAlg
                         Tr/TrackFitEvent)

find_package(GSL)
find_package(ROOT COMPONENTS Hist Tree Matrix Graf Minuit)

gaudi_add_module(OTOnlineCalibration
                 src/*.cpp
                 INCLUDE_DIRS GSL ROOT
                 LINK_LIBRARIES GSL ROOT DAQEventLib TrackEvent GaudiAlgLib TrackFitEvent)

gaudi_install_python_modules()

