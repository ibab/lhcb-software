################################################################################
# Package: LoKiGen
################################################################################
gaudi_subdir(LoKiGen v10r22)

find_package(Graphviz) 
gaudi_depends_on_subdirs(Event/GenEvent
                         Phys/LoKiCore
                         Phys/LoKiNumbers)

gaudi_add_library(LoKiGenLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  LINK_LIBRARIES GenEvent LoKiCoreLib)

gaudi_add_module(LoKiGen
                 src/Components/*.cpp
                 LINK_LIBRARIES GenEvent LoKiCoreLib LoKiGenLib)

gaudi_add_dictionary(LoKiGen
                     dict/LoKiGenDict.h
                     dict/LoKiGen.xml
                     LINK_LIBRARIES GenEvent LoKiCoreLib LoKiGenLib
                     OPTIONS " -U__MINGW32__ ")

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(GenDecayGrammarTest
                       src/tests/GenDecayGrammarTest.cpp
                       LINK_LIBRARIES GenEvent LoKiCoreLib LoKiGenLib)
endif()

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
