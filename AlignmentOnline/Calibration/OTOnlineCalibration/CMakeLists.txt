################################################################################
# Package: OTOnlineCalibration
################################################################################
gaudi_subdir(OTOnlineCalibration v1r1)

gaudi_depends_on_subdirs(Event/TrackEvent
                         Event/DAQEvent
                         Online/OMAlib
                         GaudiAlg
                         Tr/TrackFitEvent)

find_package(GSL)
find_package(ROOT COMPONENTS Hist Tree Matrix Graf Minuit)

gaudi_add_module(OTOnlineCalibration
                 src/*.cpp
                 INCLUDE_DIRS GSL ROOT dim
                 LINK_LIBRARIES GSL ROOT DAQEventLib TrackEvent GaudiAlgLib
                 TrackFitEvent OMAlib dim)

gaudi_install_python_modules()

