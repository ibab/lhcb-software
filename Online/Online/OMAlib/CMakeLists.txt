################################################################################
# Package: OMAlib
################################################################################
gaudi_subdir(OMAlib v4r5)

gaudi_depends_on_subdirs(GaudiAlg
                         Online/DIM
                         Online/OnlineHistDB
                         Online/Trending)

find_package(ROOT COMPONENTS Hist Graf RIO Core Tree MathCore)
find_package(Boost COMPONENTS regex thread filesystem)

gaudi_add_library(OMAlib src/*.cpp algorithms/*.cpp
                  PUBLIC_HEADERS OMAlib
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT GaudiAlgLib OnlineHistDB dim)
gaudi_add_module(OMAlibComp components/*.cpp
                 LINK_LIBRARIES OMAlib)

gaudi_install_python_modules()


gaudi_add_executable(Saveset2HistDB main/Saveset2HistDB.cpp
                     LINK_LIBRARIES OMAlib)
gaudi_add_executable(DumpOMAlarms main/DumpOMAlarms.cpp
                     LINK_LIBRARIES OMAlib)
