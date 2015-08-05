################################################################################
# Package: Generators
################################################################################
gaudi_subdir(Generators v15r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         GaudiAlg
                         Gen/EvtGen
                         Gen/EvtGenExtras
                         Kernel/MCInterfaces
                         Kernel/PartProp)

find_package(Boost COMPONENTS filesystem system)

gaudi_add_library(GeneratorsLib
                  src/Lib/*.cpp src/Lib/*.F
                  PUBLIC_HEADERS Generators
                  INCLUDE_DIRS Kernel/MCInterfaces
                  LINK_LIBRARIES GenEvent GaudiAlgLib EvtGen EvtGenExtras PartPropLib)

gaudi_add_module(Generators
                 src/component/*.cpp
                 INCLUDE_DIRS Boost Kernel/MCInterfaces
                 LINK_LIBRARIES Boost GenEvent GaudiAlgLib EvtGen EvtGenExtras GeneratorsLib)

