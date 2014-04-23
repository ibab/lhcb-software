################################################################################
# Package: Mint
################################################################################
gaudi_subdir(Mint v2r1)

gaudi_depends_on_subdirs(GaudiAlg)

find_package(ROOT COMPONENTS Minuit Physics Matrix MathCore Gpad Tree Graf)

gaudi_add_library(Mint
                  src/Mint/Events/src/*.cpp
                  src/Mint/FitParameter/src/*.cpp
                  src/Mint/IReturnIntefaces/src/*.cpp
                  src/Mint/Minimiser/src/*.cpp
                  src/Mint/NamedParameter/src/*.cpp
                  src/Mint/Pareto/src/*.cpp
                  src/Mint/PdfAndLogL/src/*.cpp
                  src/Mint/Utils/src/*.cpp
                  src/Mojito/Amplitudes/src/*.cpp
                  src/Mojito/BetterMC/src/*.cpp
                  src/Mojito/BreitWignerMC/src/*.cpp
                  src/Mojito/BycklingKajantje/src/*.cpp
                  src/Mojito/Chi2Test/src/*.cpp
                  src/Mojito/CoherenceFactor/src/*.cpp
                  src/Mojito/DalitzEventGeneration/src/*.cpp
                  src/Mojito/DalitzEvents/src/*.cpp
                  src/Mojito/DalitzFits/src/*.cpp
                  src/Mojito/DalitzIntegrator/src/*.cpp
                  src/Mojito/DecayTrees/src/*.cpp
                  src/Mojito/FitAmplitude/src/*.cpp
                  src/Mojito/InstantIntegration/src/*.cpp
                  src/Mojito/Lineshapes/src/*.cpp
                  src/Mojito/Ratios/src/*.cpp
                  src/Mojito/ParticleProperties/src/*.cpp
                  src/Mojito/Zemach/src/*.cpp
                  src/Mojito/Symmetries/src/*.cpp
                  src/Mojito/SpinFactors/src/*.cpp
                  src/Mojito/Lineshapes_CLEO2012/src/*.cpp
                  PUBLIC_HEADERS Mint
                  LINK_LIBRARIES GaudiAlgLib ROOT)

