################################################################################
# Package: LbGDML
################################################################################
gaudi_subdir(LbGDML v1r0)

gaudi_depends_on_subdirs(GaudiAlg
                         Sim/GaussTools)

FindG4libs(persistency)

find_package(XercesC)

gaudi_add_module(LbGDML
                 src/*.cpp
                 INCLUDE_DIRS XercesC
                 LINK_LIBRARIES GaudiAlgLib GaussToolsLib
                                XercesC
                                ${GEANT4_LIBS}
                 GENCONF_PRELOAD GaussToolsGenConfHelperLib)

