################################################################################
# Package: TrackKernel
################################################################################
gaudi_subdir(TrackKernel v1r11)

gaudi_depends_on_subdirs(Event/TrackEvent
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

gaudi_add_library(TrackKernel
                  src/*.cpp
                  PUBLIC_HEADERS TrackKernel
                  LINK_LIBRARIES TrackEvent LHCbKernel LHCbMathLib)

gaudi_add_dictionary(TrackKernel
                     src/TrackKernelDict.h
                     src/TrackKernelDict.xml
                     LINK_LIBRARIES TrackEvent LHCbKernel LHCbMathLib TrackKernel
                     OPTIONS "-U__MINGW32__")

