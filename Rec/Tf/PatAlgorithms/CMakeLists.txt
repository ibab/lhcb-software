################################################################################
# Package: PatAlgorithms
################################################################################
gaudi_subdir(PatAlgorithms v4r44)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Event/RecEvent
                         Event/TrackEvent
                         Kernel/PartProp
                         OT/OTDAQ
                         Tf/PatKernel
                         Tf/TfKernel
                         Tr/TrackInterfaces
                         Tr/TrackTools)

gaudi_add_module(PatAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS OT/OTDAQ Tf/PatKernel Tf/TfKernel Tr/TrackInterfaces
                 LINK_LIBRARIES OTDetLib STDetLib RecEvent TrackEvent PartPropLib)

gaudi_install_python_modules()

