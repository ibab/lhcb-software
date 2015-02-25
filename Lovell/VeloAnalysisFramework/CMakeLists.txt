################################################################################
# Package: VeloAnalysisFramework
################################################################################
gaudi_subdir(VeloAnalysisFramework v1r0)

#Modify as necessary
find_package(ROOT COMPONENTS Hist Matrix Graf)

gaudi_depends_on_subdirs(GaudiKernel)

gaudi_add_library(VeloGUIUtils
                  src/*.cc
                  # PUBLIC_HEADERS src
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT)

gaudi_add_dictionary(VeloGUIUtils src/VeloGUIUtilsRflx.h  src/VeloGUIUtilsRflx.xml
                     LINK_LIBRARIES VeloGUIUtils)

# gaudi_add_module(VeloGUIUtils
#                  src/*.cc
#                  INCLUDE_DIRS ROOT
#                  LINK_LIBRARIES ROOT)

gaudi_install_python_modules()

gaudi_install_scripts()

gaudi_add_test(QMTest QMTEST)
