################################################################################
# Package: MDF
################################################################################
gaudi_subdir(MDF v3r44)

gaudi_depends_on_subdirs(Event/DAQEvent
                         GaudiKernel)

find_package(ROOT)

gaudi_add_library(MDFLib
                  src/*.cpp
                  PUBLIC_HEADERS MDF
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT GaudiKernel DAQEventLib)

gaudi_add_module(MDF
                 components/*.cpp
                 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES ROOT GaudiKernel DAQEventLib MDFLib)

