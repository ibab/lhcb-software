################################################################################
# Package: GaussCherenkov
################################################################################
gaudi_subdir(GaussCherenkov v4r0)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Kernel/Relations
                         Rich/RichKernel
                         Sim/GaussRICH
                         Sim/GaussTools)

find_package(AIDA)

gaudi_add_module(GaussCherenkov
                 src/SensDet/*.cpp
                 src/CherenkovPhysProcess/*.cpp
                 src/CkvPhysPhotDet/*.cpp
                 src/srcG4Ckv/*.cc
                 src/CherenkovAction/*.cpp
                 src/CherenkovAnalysis/*.cpp
                 src/CherenkovMisc/*.cpp
                 src/CherenkovTestBeamPhysProcess/*.cpp
                 src/CherenkovTestBeamSensDet/*.cpp
                 src/TorchTestBeamAnalysis/*.cpp
                 INCLUDE_DIRS AIDA
                              src/SensDet
                              src/CherenkovPhysProcess
                              src/CkvPhysPhotDet
                              src/srcG4Ckv
                              src/CherenkovAction
                              src/CherenkovAnalysis
                              src/CherenkovMisc
                              src/CherenkovAssoc
                              src/CherenkovTestBeamPhysProcess
                              src/CherenkovTestBeamSensDet
                              src/TorchTestBeamAnalysis
                 LINK_LIBRARIES RichDetLib LinkerEvent MCEvent RelationsLib
                                RichKernelLib GaussRICHLib GaussToolsLib
                 GENCONF_PRELOAD GaussToolsGenConfHelperLib)

