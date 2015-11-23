################################################################################
# Package: DetCond
################################################################################
gaudi_subdir(DetCond v12r45)

gaudi_depends_on_subdirs(Det/DetDesc
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel)

find_package(Boost COMPONENTS system thread filesystem)
find_package(COOL COMPONENTS CoolKernel CoolApplication)
find_package(CORAL COMPONENTS CoralBase CoralKernel RelationalAccess)

gaudi_add_library(DetCondLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS DetCond
                  INCLUDE_DIRS Boost COOL CORAL
                  LINK_LIBRARIES Boost COOL CORAL DetDescLib GaudiAlgLib GaudiKernel LHCbKernel)

gaudi_add_module(DetCond
                 src/component/*.cpp
                 tests/src/*.cpp
                 INCLUDE_DIRS Boost COOL CORAL
                 LINK_LIBRARIES Boost COOL CORAL DetDescLib GaudiAlgLib GaudiKernel LHCbKernel DetCondLib)

gaudi_add_dictionary(DetCond
                     src/dict/DetCondDict.h
                     src/dict/DetCondDict.xml
                     INCLUDE_DIRS Boost COOL CORAL
                     LINK_LIBRARIES Boost COOL CORAL DetDescLib GaudiAlgLib GaudiKernel LHCbKernel DetCondLib
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
