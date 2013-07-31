################################################################################
# Package    : Tools/EventIndexer
# Maintainer : Marco Clemencic
################################################################################
gaudi_subdir(EventIndexer v1r1)

gaudi_depends_on_subdirs(GaudiAlg Event/RecEvent Event/HltEvent)

find_package(ROOT COMPONENTS Core Cint Tree)
find_package(RELAX)

set(dict_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/EventIndexer.h
              ${CMAKE_CURRENT_SOURCE_DIR}/src/LinkDef.h)
add_custom_command(OUTPUT EventIndexerDict.h EventIndexerDict.cpp
                   COMMAND ${env_cmd} --xml ${env_xml}
                     ${ROOT_rootcint_CMD} -f EventIndexerDict.cpp -c
                           ${dict_srcs}
                   DEPENDS ${dict_srcs})

gaudi_add_module(EventIndexer
                 src/*.cpp EventIndexerDict.cpp
                 INCLUDE_DIRS ROOT ${CMAKE_CURRENT_BINARY_DIR}
                 LINK_LIBRARIES ROOT GaudiAlgLib RecEvent HltEvent)

gaudi_add_test(QMTest QMTEST)
