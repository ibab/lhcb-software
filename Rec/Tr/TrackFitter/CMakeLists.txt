################################################################################
# Package: TrackFitter
################################################################################
gaudi_subdir(TrackFitter v5r7)

gaudi_depends_on_subdirs(Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbMath
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)

gaudi_add_module(TrackFitter
                 src/*.cpp
                 INCLUDE_DIRS GSL Tr/TrackInterfaces
                 LINK_LIBRARIES GSL TrackEvent GaudiAlgLib GaudiKernel LHCbMathLib TrackFitEvent TrackKernel)

gaudi_install_python_modules()

