################################################################################
# Package: IndependenceTests
################################################################################
gaudi_subdir(IndependenceTests v1r13)

gaudi_depends_on_subdirs(Event/HltEvent
                         GaudiAlg
                         GaudiKernel
                         Hlt/HltDAQ
                         Hlt/HltMonitoring)

find_package(Boost COMPONENTS program_options)

gaudi_add_module(Independence
                 src/lib/TupleHltDecReports.cpp
                 LINK_LIBRARIES HltEvent GaudiAlgLib GaudiKernel)

gaudi_add_library(DecisionMap
                  src/dm/DecisionMap.cpp
                  PUBLIC_HEADERS Independence
                  LINK_LIBRARIES HltEvent GaudiAlgLib GaudiKernel)

gaudi_add_executable(find_mismatches src/app/find_mismatches.cpp
                     LINK_LIBRARIES DecisionMap Boost)

gaudi_add_dictionary(Independence
                     dict/Independence.h
                     dict/Independence.xml
                     LINK_LIBRARIES HltEvent GaudiAlgLib GaudiKernel DecisionMap
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
