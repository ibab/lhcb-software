################################################################################
# Package: Particle2MCTruth
################################################################################
gaudi_subdir(Particle2MCTruth v2r6)

gaudi_depends_on_subdirs(Phys/DaVinciMCKernel
                         Phys/LoKiAlgoMC)

find_package(CLHEP)

gaudi_add_library(Particle2MCTruthLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS P2MCP
                  INCLUDE_DIRS CLHEP
                  LINK_LIBRARIES CLHEP DaVinciMCKernelLib LoKiAlgoMCLib)

gaudi_add_module(Particle2MCTruth
                 src/*.cpp
                 INCLUDE_DIRS CLHEP
                 LINK_LIBRARIES CLHEP DaVinciMCKernelLib LoKiAlgoMCLib Particle2MCTruthLib)

gaudi_add_dictionary(P2MCP
                     dict/Particle2MCTruthDict.h
                     dict/Particle2MCTruthDict.xml
                     INCLUDE_DIRS CLHEP
                     LINK_LIBRARIES CLHEP DaVinciMCKernelLib LoKiAlgoMCLib Particle2MCTruthLib
                     OPTIONS "-U__MINGW32__")

