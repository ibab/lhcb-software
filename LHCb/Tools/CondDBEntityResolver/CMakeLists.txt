################################################################################
# Package: CondDBEntityResolver
################################################################################
gaudi_subdir(CondDBEntityResolver v5r9)

gaudi_depends_on_subdirs(GaudiKernel
                         Det/DetCond
                         Tools/XmlTools)

find_package(Boost COMPONENTS python)
find_package(ROOT COMPONENTS Core)
find_package(PythonLibs)

gaudi_add_module(CondDBEntityResolver
                 src/component/*.cpp
                 LINK_LIBRARIES DetCondLib XmlToolsLib)

gaudi_add_python_module(CondDBCompression
                          src/component/CondDBCompression.cpp
                          src/python/module.cpp
                        LINK_LIBRARIES ${Boost_PYTHON_LIBRARY} ROOT GaudiKernel
                        INCLUDE_DIRS Boost ROOT PythonLibs)

gaudi_add_test(QMTest QMTEST)
