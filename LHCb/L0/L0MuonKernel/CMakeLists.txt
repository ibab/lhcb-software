################################################################################
# Package: L0MuonKernel
################################################################################
gaudi_subdir(L0MuonKernel v9r9)

gaudi_depends_on_subdirs(L0/ProcessorKernel)

gaudi_add_library(L0MuonKernel
                  src/*.cpp
                  PUBLIC_HEADERS L0MuonKernel
                  LINK_LIBRARIES ProcessorKernel)

