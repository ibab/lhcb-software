################################################################################
# Package: EvtGen
################################################################################
gaudi_subdir(EvtGen v14r3)

gaudi_depends_on_subdirs(Gen/GENSER)

find_package(CLHEP)
find_package(HepMC)

find_package(Pythia8)
find_package(Photos++)
find_package(Tauola++)

if(EXISTS "${PYTHIA8_INCLUDE_DIR}/Pythia8/Pythia.h")
  # include directory changed from Pythia8 175 to 183
  include_directories(${PYTHIA8_INCLUDE_DIR}/Pythia8)
endif()

gaudi_add_library(EvtGen
                  src/*.cpp
                  src/*.F
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS HepMC CLHEP Tauola++ Pythia8 Photos++
                  LINK_LIBRARIES HepMC CLHEP Tauola++ Pythia8 Photos++ pythia6forgauss)

