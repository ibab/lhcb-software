################################################################################
# Package: MuonKernel
################################################################################
gaudi_subdir(MuonKernel v5r0)

gaudi_depends_on_subdirs(Kernel/LHCbKernel)

gaudi_add_library(MuonKernelLib
                  src/*.cpp
                  PUBLIC_HEADERS MuonKernel
                  LINK_LIBRARIES LHCbKernel)

