################################################################################
# Package: TsaKernel
################################################################################
gaudi_subdir(TsaKernel v3r7)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Event/DigiEvent
                         Event/TrackEvent
                         Kernel/LHCbMath
                         Tf/TfKernel)

find_package(GSL)

gaudi_add_library(TsaKernel
                  src/*.cpp
                  PUBLIC_HEADERS TsaKernel
                  INCLUDE_DIRS GSL Event/DigiEvent Tf/TfKernel
                  LINK_LIBRARIES GSL OTDetLib STDetLib TrackEvent LHCbMathLib)

gaudi_add_dictionary(TsaKernel
                     dict/TsaKernelDict.h
                     dict/TsaKernelDict.xml
                     INCLUDE_DIRS GSL Event/DigiEvent Tf/TfKernel
                     LINK_LIBRARIES GSL OTDetLib STDetLib TrackEvent LHCbMathLib TsaKernel
                     OPTIONS "-U__MINGW32__")

