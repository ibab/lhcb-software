################################################################################
# Package: LbPythia8
################################################################################
gaudi_subdir(LbPythia8 v7r4)

gaudi_depends_on_subdirs(Gen/Generators)

find_package(Pythia8)
find_package(HepMC COMPONENTS fio)

gaudi_add_library(LbPythia8Lib
                  src/Lib/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Pythia8
                  LINK_LIBRARIES GeneratorsLib Pythia8)

find_library(hepmcinterface hepmcinterface)
gaudi_add_module(LbPythia8
                 src/component/*.cpp
                 LINK_LIBRARIES GeneratorsLib LbPythia8Lib HepMC
                                ${hepmcinterface})

gaudi_env(SET PYTHIA8XML ${PYTHIA8_XML})

