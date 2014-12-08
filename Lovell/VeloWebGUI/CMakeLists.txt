################################################################################
# Package: VeloWebGUI
################################################################################
gaudi_subdir(VeloWebGUI v1r0)

gaudi_depends_on_subdirs(VeloAnalysisFramework)

gaudi_add_module(VeloWebGUI
                 src/*.cpp
                 INCLUDE_DIRS VeloAnalysisFramework)

gaudi_install_python_modules()

