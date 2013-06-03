################################################################################
# Package: CondDBEntityResolver
################################################################################
gaudi_subdir(CondDBEntityResolver v5r6)

gaudi_depends_on_subdirs(Det/DetCond
                         Tools/XmlTools)

gaudi_add_module(CondDBEntityResolver
                 src/component/*.cpp
                 LINK_LIBRARIES DetCondLib XmlToolsLib)

find_package(ROOT COMPONENTS Core)

gaudi_add_dictionary(CondDBEntityResolver
                     src/dict/dict.h
                     src/dict/dict.xml
                     INCLUDE_DIRS src/component ROOT
                     LINK_LIBRARIES ROOT GaudiKernel
                     OPTIONS "-U__MINGW32__")

gaudi_add_test(QMTest QMTEST)
