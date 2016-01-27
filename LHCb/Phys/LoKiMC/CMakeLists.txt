################################################################################
# Package: LoKiMC
################################################################################
gaudi_subdir(LoKiMC v10r21)

find_package(Graphviz)

gaudi_depends_on_subdirs(Event/MCEvent
                         Kernel/MCInterfaces
                         Phys/LoKiCore
                         Phys/LoKiNumbers)

gaudi_add_library(LoKiMCLib
                  src/*.cpp
                  PUBLIC_HEADERS LoKi
                  INCLUDE_DIRS Kernel/MCInterfaces
                  LINK_LIBRARIES MCEvent LoKiCoreLib)

gaudi_add_module(LoKiMC
                 src/Components/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES MCEvent LoKiCoreLib LoKiMCLib)

gaudi_add_dictionary(LoKiMC
                     dict/LoKiMCDict.h
                     dict/LoKiMC.xml
                     INCLUDE_DIRS Kernel/MCInterfaces
                     LINK_LIBRARIES MCEvent LoKiCoreLib LoKiMCLib
                     OPTIONS " -U__MINGW32__ ")

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(MCDecayGrammarTest
                       src/tests/MCDecayGrammarTest.cpp
                       INCLUDE_DIRS Kernel/MCInterfaces
                       LINK_LIBRARIES MCEvent LoKiCoreLib LoKiMCLib)
endif()

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
