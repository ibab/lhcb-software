################################################################################
# Package: ProcessorKernel
################################################################################
gaudi_subdir(ProcessorKernel v7r1)

gaudi_depends_on_subdirs(Kernel/LHCbKernel)

find_package(Boost)
find_package(XercesC)

gaudi_add_library(ProcessorKernel
                  src/*.cpp
                  PUBLIC_HEADERS ProcessorKernel
                  INCLUDE_DIRS Boost XercesC
                  LINK_LIBRARIES Boost XercesC LHCbKernel)

