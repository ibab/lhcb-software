################################################################################
# Package: Gaucho
################################################################################
gaudi_subdir(Gaucho v7r42)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/DAQEvent
                         GaudiAlg
                         Online/DIM
                         Online/OnlineKernel
                         Online/Trending)

find_package(Boost COMPONENTS regex)

gaudi_add_library(GauchoLib src/*.cpp
                  PUBLIC_HEADERS Gaucho
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost GaudiAlgLib dim OnlineKernel)

gaudi_add_module(Gaucho src/components/*.cpp
                 LINK_LIBRARIES GauchoLib DAQEventLib)
