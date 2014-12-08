################################################################################
# Package: VeloAnalysisFramework
################################################################################
gaudi_subdir(VeloAnalysisFramework v1r0)

#Modify as necessary
find_package(ROOT COMPONENTS Hist Matrix Graf)

gaudi_add_module(VeloAnalysisFramework
                 src/*.cpp
                 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES ROOT)

gaudi_install_python_modules()

