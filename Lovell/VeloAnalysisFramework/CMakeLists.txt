################################################################################
# Package: VeloAnalysisFramework
################################################################################
gaudi_subdir(VeloAnalysisFramework v1r3)

find_package(ROOT COMPONENTS Hist Matrix Graf)

gaudi_depends_on_subdirs(GaudiKernel)

gaudi_add_dictionary(VeloAnalysisFramework dict/VeloAnalysisFramework.h  dict/VeloAnalysisFramework.xml
                     LINK_LIBRARIES VeloAnalysisFramework)

gaudi_add_library(VeloAnalysisFramework
                  src/*.cc
                  PUBLIC_HEADERS VeloAnalysisFramework
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT)

gaudi_install_python_modules()

gaudi_install_scripts()

gaudi_add_test(QMTest QMTEST)
