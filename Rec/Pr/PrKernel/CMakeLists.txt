################################################################################
# Package: PrKernel
################################################################################
gaudi_subdir(PrKernel v1r10)

gaudi_depends_on_subdirs(Event/RecEvent
                         Tf/TfKernel     
                         GaudiAlg)

gaudi_add_library(PrKernel
                  src/*.cpp
                  PUBLIC_HEADERS PrKernel
                  LINK_LIBRARIES RecEvent GaudiAlgLib)

gaudi_add_dictionary(PrKernel
                     dict/PrKernelDict.h
                     dict/PrKernelDict.xml
                     INCLUDE_DIRS Pr/PrKernel
                     LINK_LIBRARIES RecEvent GaudiAlgLib
                     OPTIONS "-U__MINGW32__")

