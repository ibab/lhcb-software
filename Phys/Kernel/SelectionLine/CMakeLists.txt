################################################################################
# Package: SelectionLine
################################################################################
gaudi_subdir(SelectionLine v1r6)

gaudi_depends_on_subdirs(Event/HltEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiUtils
                         Kernel/HltInterfaces)

find_package(AIDA)
find_package(Boost)

gaudi_add_library(SelectionLine
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
                  INCLUDE_DIRS AIDA Boost
                  LINK_LIBRARIES Boost HltEvent GaudiAlgLib GaudiKernel GaudiUtilsLib HltInterfaces)

