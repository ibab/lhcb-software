################################################################################
# Package: EvtGenExtras
################################################################################
gaudi_subdir(EvtGenExtras v3r8)

gaudi_depends_on_subdirs(Gen/EvtGen
                         Gen/Mint)

find_package(GSL)

gaudi_add_library(EvtGenExtras
                  src/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS GSL
                  LINK_LIBRARIES GSL EvtGen Mint)

