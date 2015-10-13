################################################################################
# Package: PatKernel
################################################################################
gaudi_subdir(PatKernel v2r10p1)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         Event/TrackEvent
                         Tf/TfKernel)

gaudi_add_module(PatKernel
                 src/*.cpp
                 INCLUDE_DIRS Tf/TfKernel
                 LINK_LIBRARIES OTDetLib STDetLib VeloDetLib TrackEvent)

gaudi_add_dictionary(PatKernel
                     dict/PatKernelDict.h
                     dict/PatKernelDict.xml
                     INCLUDE_DIRS Tf/TfKernel
                     LINK_LIBRARIES OTDetLib STDetLib VeloDetLib TrackEvent
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(PatKernel)

