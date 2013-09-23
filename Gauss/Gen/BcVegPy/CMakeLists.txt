################################################################################
# Package: BcVegPy
################################################################################
gaudi_subdir(BcVegPy v3r1)

gaudi_depends_on_subdirs(Gen/GENSER)

include_directories(include)

gaudi_add_library(BcVegPy
                  src/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES pythia6forgauss)

