################################################################################
# Package: Relations
################################################################################
gaudi_subdir(Relations v6r3)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(Boost)
find_package(ROOT)

gaudi_add_library(RelationsLib
                  src/*.cpp
                  PUBLIC_HEADERS Relations
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost GaudiKernel)

gaudi_add_dictionary(Relations
                     dict/selection.h
                     dict/selection.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost GaudiKernel RelationsLib
                     OPTIONS " -U__MINGW32__ ")

if(GAUDI_BUILD_TESTS)
  gaudi_add_executable(RelationsTests
                       tests/RelationsTest.cpp
                       INCLUDE_DIRS Boost
                       LINK_LIBRARIES Boost GaudiKernel RelationsLib)
endif()

gaudi_install_python_modules()

