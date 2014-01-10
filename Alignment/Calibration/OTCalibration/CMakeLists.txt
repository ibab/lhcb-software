################################################################################
# Package: OTCalibration
################################################################################
gaudi_subdir(OTCalibration v1r6)

gaudi_depends_on_subdirs(Event/TrackEvent
                         Event/DAQEvent
                         GaudiAlg
                         Tr/TrackFitEvent)

find_package(GSL)
find_package(ROOT COMPONENTS Hist Tree Matrix Graf Minuit)

gaudi_add_module(OTCalibration
                 src/*.cpp
                 INCLUDE_DIRS GSL ROOT
                 LINK_LIBRARIES GSL ROOT DAQEventLib TrackEvent GaudiAlgLib TrackFitEvent)

gaudi_install_python_modules()

