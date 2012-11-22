################################################################################
# Package: TrackMonitors
################################################################################
gaudi_subdir(TrackMonitors v1r40)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiUtils
                         Kernel/PartProp
                         ST/STKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)

gaudi_add_module(TrackMonitors
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Tr/TrackInterfaces
                 LINK_LIBRARIES GSL CaloDetLib MCEvent PhysEvent RecEvent TrackEvent GaudiAlgLib GaudiUtilsLib PartPropLib STKernelLib TrackFitEvent TrackKernel)

gaudi_install_python_modules()

