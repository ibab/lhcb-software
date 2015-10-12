################################################################################
# Package: FileStager
################################################################################
gaudi_subdir(FileStager v1r14p1)

gaudi_depends_on_subdirs(GaudiKernel)

find_package(Boost COMPONENTS system regex iostreams thread filesystem)

gaudi_add_library(FileStagerFunctions
                  src/Lib/*.cpp
                  PUBLIC_HEADERS FileStager
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost GaudiKernel)

gaudi_add_module(FileStager
                 src/Component/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost GaudiKernel FileStagerFunctions)

gaudi_add_dictionary(FileStager
                     dict/FileStagerDict.h
                     dict/FileStagerDict.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost GaudiKernel FileStagerFunctions
                     OPTIONS "-U__MINGW32__")

# only for the applications
find_package(Boost COMPONENTS program_options)

gaudi_add_executable(garbage
                     src/app/garbage.cpp
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost GaudiKernel FileStagerFunctions)

gaudi_add_executable(hash_filename
                     src/app/hash_filename.cpp
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost GaudiKernel FileStagerFunctions)

gaudi_install_python_modules()

