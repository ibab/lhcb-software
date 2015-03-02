################################################################################
# Package: VeloOfflineGUI
################################################################################
gaudi_subdir(VeloOfflineGUI v1r1)

gaudi_depends_on_subdirs(VeloAnalysisFramework)

find_package(ROOT COMPONENTS Core RIO)

find_package(Qt COMPONENTS QtCore QtGui)
include(${QT_USE_FILE})

file(GLOB HRDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h)
QT4_WRAP_CPP(MOCS ${HRDS})

QT4_WRAP_UI(UIS_HDRS src/VeloView.ui)
add_custom_target(GenUIHeaders DEPENDS ${UIS_HDRS})

gaudi_add_executable(VeloOfflineGUI src/*.cpp ${MOCS}
                     INCLUDE_DIRS VeloAnalysisFramework Qt ROOT
                     LINK_LIBRARIES Qt ROOT)

add_dependencies(VeloOfflineGUI GenUIHeaders)

gaudi_install_scripts()
