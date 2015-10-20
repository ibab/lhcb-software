################################################################################
# Package: VeloOfflineGUI
################################################################################
gaudi_subdir(VeloOfflineGUI v1r3)

gaudi_depends_on_subdirs(VeloAnalysisFramework)

find_package(pyanalysis)
find_package(pygraphics)
find_package(PythonLibs)

find_package(Boost COMPONENTS system)

find_package(ROOT COMPONENTS Core RIO)

find_package(Qt COMPONENTS QtCore QtGui)
include(${QT_USE_FILE})

file(GLOB HRDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)
QT4_WRAP_CPP(MOCS ${HRDS})

QT4_WRAP_UI(UIS_HDRS src/VeloView.ui)
add_custom_target(GenUIHeaders DEPENDS ${UIS_HDRS})

QT4_ADD_RESOURCES(RC_SRCS qt_resources/Resources.qrc)

gaudi_add_executable(VeloOfflineGUI src/*.cpp ${MOCS} ${RC_SRCS}
                     INCLUDE_DIRS VeloAnalysisFramework Qt ROOT Boost PythonLibs
                     LINK_LIBRARIES Qt ROOT Boost PythonLibs)

add_dependencies(VeloOfflineGUI GenUIHeaders)

gaudi_install_scripts()
