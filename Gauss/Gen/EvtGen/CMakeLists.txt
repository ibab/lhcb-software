################################################################################
# Package: EvtGen
################################################################################
gaudi_subdir(EvtGen v13r7)

gaudi_depends_on_subdirs(Gen/GENSER)

find_package(CLHEP)
find_package(HepMC)

find_package(Pythia8)
find_package(Photos++)
find_package(Tauola++)

gaudi_add_library(EvtGen
                  src/*.cpp
                  src/*.F
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS HepMC CLHEP Tauola++ Pythia8 Photos++
                  LINK_LIBRARIES HepMC CLHEP Tauola++ Pythia8 Photos++ pythia6forgauss)

