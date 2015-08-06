################################################################################
# Package: DecayTreeFitter
################################################################################
gaudi_subdir(DecayTreeFitter v1r21)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/LoKiCore
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(CLHEP)

gaudi_add_library(DecayTreeFitter
                  src/*.cpp
                  PUBLIC_HEADERS DecayTreeFitter
                  INCLUDE_DIRS CLHEP AIDA CLHEP Tr/TrackInterfaces
                  LINK_LIBRARIES CLHEP CLHEP DaVinciKernelLib LoKiCoreLib TrackKernel)

gaudi_add_dictionary(DecayTreeFitter
                     dict/DecayTreeFitterDict.h
                     dict/DecayTreeFitterDict.xml
                     INCLUDE_DIRS CLHEP Tr/TrackInterfaces
                     LINK_LIBRARIES CLHEP DaVinciKernelLib LoKiCoreLib TrackKernel DecayTreeFitter
                     OPTIONS "-U__MINGW32__")

