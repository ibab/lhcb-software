################################################################################
# Package: VeloOfflineGUI
################################################################################
gaudi_subdir(VeloOfflineGUI v1r0)

gaudi_depends_on_subdirs(VeloAnalysisFramework)

gaudi_add_module(VeloOfflineGUI
                 src/*.cpp
                 INCLUDE_DIRS VeloAnalysisFramework)

gaudi_install_python_modules()

