################################################################################
# Package: CondDBUI
################################################################################
gaudi_subdir(CondDBUI v3r35)

gaudi_depends_on_subdirs(Det/DDDB)

find_package(Boost)
find_package(COOL COMPONENTS CoolKernel CoolApplication)
find_package(CORAL COMPONENTS CoralBase CoralKernel RelationalAccess)
find_package(PythonLibs)
find_package(Qt)
find_package(ROOT)
find_package(XercesC)
find_package(pygraphics)
find_package(pytools)

gaudi_add_library(CondDBUIHelperLib src/helper/*.cpp
                  INCLUDE_DIRS PythonLibs COOL
                  LINK_LIBRARIES PythonLibs COOL
                  PUBLIC_HEADERS CondDBUIHelper)

gaudi_add_dictionary(CondDBUI
                     dict/conddbui_dictionary.h
                     dict/dictionary.xml
                     INCLUDE_DIRS ROOT PythonLibs COOL Boost CORAL
                     LINK_LIBRARIES -Wl,--no-as-needed CondDBUIHelperLib -Wl,--as-needed ROOT PythonLibs COOL Boost CORAL)

# gen_pyqt_* functions are provided by 'pygraphics'
gen_pyqt_resource(CondDBUI.Browser.Resources CondDBUI/Browser qt_resources/*.qrc)
gen_pyqt_uic(CondDBUI.Browser.UI CondDBUI/Browser qt_resources/*.ui)

gaudi_install_python_modules()
gaudi_install_scripts()

