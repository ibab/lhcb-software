################################################################################
# Package: VeloOfflineGUI
################################################################################
gaudi_subdir(VeloOfflineGUI v1r0)

gaudi_depends_on_subdirs(VeloAnalysisFramework)


find_package(Qt COMPONENTS QtCore QtGui)
find_package(ROOT COMPONENTS Core RIO)

QT4_WRAP_UI(UIS_HDRS VeloView.ui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

gaudi_add_executable(VeloOfflineGUI
		 src/*.cpp 
            	 INCLUDE_DIRS VeloAnalysisFramework Qt ROOT
		 LINK_LIBRARIES Qt ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ROOT )

 gaudi_install_scripts()
