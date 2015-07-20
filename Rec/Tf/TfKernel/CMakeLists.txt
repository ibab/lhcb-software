################################################################################
# Package: TfKernel
################################################################################
gaudi_subdir(TfKernel v2r33)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         Event/DAQEvent
                         Event/RecEvent
                         GaudiAlg
                         OT/OTDAQ)

gaudi_add_dictionary(TfKernel
                     dict/TfKernelDict.h
                     dict/TfKernelDict.xml
                     INCLUDE_DIRS OT/OTDAQ
                     LINK_LIBRARIES OTDetLib STDetLib VeloDetLib DAQEventLib RecEvent GaudiAlgLib
                     OPTIONS "-U__MINGW32__ -DBOOST_DISABLE_ASSERTS")

gaudi_install_headers(TfKernel)

