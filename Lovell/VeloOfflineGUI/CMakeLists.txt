################################################################################
# Package: VeloOfflineGUI
################################################################################
gaudi_subdir(VeloOfflineGUI v1r3)

gaudi_depends_on_subdirs(VeloAnalysisFramework)

find_package(pyanalysis)
find_package(pygraphics)
find_package(PythonLibs)
find_package(pytools)

find_package(Boost COMPONENTS system)

find_package(ROOT COMPONENTS Core RIO)

find_package(Qt COMPONENTS QtCore QtGui)
include(${QT_USE_FILE})

QT4_ADD_RESOURCES(RC_SRCS qt_resources/Resources.qrc)

gaudi_install_python_modules()
gaudi_install_scripts()
