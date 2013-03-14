################################################################################
# Package    : Tools/EventIndexer
# Maintainer : Marco Clemencic
################################################################################
gaudi_subdir(EventIndexer v1r0)

gaudi_depends_on_subdirs(GaudiAlg Event/RecEvent)

find_package(ROOT COMPONENTS Core Cint Tree)

set(dict_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/EventIndexer.h
              ${CMAKE_CURRENT_SOURCE_DIR}/src/LinkDef.h)
add_custom_command(OUTPUT EventIndexerDict.h EventIndexerDict.cpp
                   COMMAND ${ROOT_rootcint_CMD} -f EventIndexerDict.cpp -c
                           ${dict_srcs}
                   DEPENDS ${dict_srcs})

gaudi_add_module(EventIndexer
                 src/*.cpp EventIndexerDict.cpp
                 INCLUDE_DIRS ROOT ${CMAKE_CURRENT_BINARY_DIR}
                 LINK_LIBRARIES ROOT GaudiAlgLib RecEvent)

gaudi_add_test(QMTest QMTEST)
