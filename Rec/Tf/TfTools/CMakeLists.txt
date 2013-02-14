################################################################################
# Package: TfTools
################################################################################
gaudi_subdir(TfTools v1r17)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         OT/OTDAQ
                         Tf/TfKernel)

find_package(Boost)

gaudi_add_module(TfTools
                 src/component/*.cpp
                 INCLUDE_DIRS Boost OT/OTDAQ Tf/TfKernel
                 LINK_LIBRARIES Boost OTDetLib STDetLib VeloDetLib TrackEvent GaudiAlgLib GaudiKernel LHCbKernel)

