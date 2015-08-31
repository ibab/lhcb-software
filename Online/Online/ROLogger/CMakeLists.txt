################################################################################
# Package: ROLogger
################################################################################
gaudi_subdir(ROLogger v0r32)

gaudi_depends_on_subdirs(Online/UPI
                         Online/DIM)

gaudi_add_library(ROLogger src/*.cpp
                  PUBLIC_HEADERS ROLogger
                  LINK_LIBRARIES UPI dim)
