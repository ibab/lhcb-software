################################################################################
# Package: GenXicc
################################################################################
gaudi_subdir(GenXicc v2r0)

gaudi_depends_on_subdirs(Gen/GENSER)

include_directories(include)

gaudi_add_library(GenXicc
                  src/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES pythia6forgauss)

