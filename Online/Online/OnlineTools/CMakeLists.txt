################################################################################
# Package: OnlineTools
################################################################################
gaudi_subdir(OnlineTools v1r6)

gaudi_depends_on_subdirs(Online/ROLogger)

gaudi_add_library(OnlineTools src/*.cpp
                  PUBLIC_HEADERS OnlineTools
                  LINK_LIBRARIES ROLogger)
