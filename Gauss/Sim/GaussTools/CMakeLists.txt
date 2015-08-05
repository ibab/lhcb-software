################################################################################
# Package: GaussTools
################################################################################
gaudi_subdir(GaussTools v20r0)

gaudi_depends_on_subdirs(Sim/GiGaCnv
                         Sim/SimSvc)

FindG4libs(run intercoms)
find_package(CLHEP COMPONENTS Random)

gaudi_add_library(GaussToolsLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS GaussTools
                  INCLUDE_DIRS Sim/SimSvc
                  LINK_LIBRARIES GiGaCnvLib)

gaudi_add_library(GaussToolsGenConfHelperLib
                  src/genConf/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES CLHEP ${GEANT4_LIBS})

gaudi_add_module(GaussTools
                 src/Components/*.cpp
                 INCLUDE_DIRS Sim/SimSvc
                 LINK_LIBRARIES GiGaCnvLib GaussToolsLib ${GEANT4_LIBS}
                 GENCONF_PRELOAD GaussToolsGenConfHelperLib)

