################################################################################
# Package: TrackMonitors
################################################################################
gaudi_subdir(TrackMonitors v2r10)

gaudi_depends_on_subdirs(Det/CaloDet
                         Event/MCEvent
                         Event/GenEvent
                         Event/PhysEvent
                         Event/RecEvent
                         Event/TrackEvent
                         Event/HltEvent
                         GaudiAlg
                         GaudiUtils
                         Kernel/PartProp
                         ST/STKernel
                         Tf/PatKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)

gaudi_add_module(TrackMonitors
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Tr/TrackInterfaces Tf/PatKernel
                 LINK_LIBRARIES GSL CaloDetLib MCEvent GenEvent PhysEvent RecEvent TrackEvent HltEvent GaudiAlgLib GaudiUtilsLib PartPropLib STKernelLib TrackFitEvent TrackKernel)

gaudi_install_python_modules()
