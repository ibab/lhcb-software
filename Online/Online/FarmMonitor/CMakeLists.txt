################################################################################
# Package: FarmMonitor
################################################################################
gaudi_subdir(FarmMonitor v2r12)

gaudi_depends_on_subdirs(Online/ROMon
                         Online/DIM
                         Online/Trending
                         GaudiKernel)

find_package(ROOT COMPONENTS Thread)
find_package(Boost COMPONENTS regex thread filesystem date_time serialization system)

gaudi_add_library(FarmMonitor src/*.cpp
                  PUBLIC_HEADERS FarmMonitor
                  INCLUDE_DIRS Boost ROOT
                  LINK_LIBRARIES Boost ROOT dim GaudiKernel ROMon)
