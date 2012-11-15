################################################################################
# Package: HistoStrings
################################################################################
gaudi_subdir(HistoStrings v1r3)

gaudi_depends_on_subdirs(GaudiKernel
                         GaudiUtils)

find_package(AIDA)
find_package(Boost)
find_package(ROOT COMPONENTS Hist XMLIO)

gaudi_add_library(HistoStrings
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
                  INCLUDE_DIRS AIDA ROOT Boost
                  LINK_LIBRARIES ROOT Boost GaudiKernel GaudiUtilsLib)

gaudi_add_dictionary(HistoStrings
                     dict/HistoStringsDict.h
                     dict/HistoStrings.xml
                     INCLUDE_DIRS AIDA ROOT Boost
                     LINK_LIBRARIES ROOT Boost GaudiKernel GaudiUtilsLib HistoStrings
                     OPTIONS " -U__MINGW32__ ")

gaudi_install_python_modules()

