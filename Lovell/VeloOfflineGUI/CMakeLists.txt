################################################################################
# Package: VeloOfflineGUI
################################################################################
gaudi_subdir(VeloOfflineGUI v1r0)

gaudi_depends_on_subdirs(VeloAnalysisFramework)


find_package(Qt)
find_package(ROOT)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

gaudi_add_module(VeloOfflineGUI
                 src/*.cpp
                 INCLUDE_DIRS VeloAnalysisFramework Qt ROOT
		LINK_LIBRARIES Qt ROOT)

gaudi_install_python_modules()

