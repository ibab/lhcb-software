################################################################################
# Package: ROMon
################################################################################
gaudi_subdir(ROMon v1r44)

gaudi_depends_on_subdirs(Online/OnlineKernel
                         Online/DIM)

find_package(XercesC)
find_package(Oracle)
find_package(ROOT COMPONENTS Hist Rint)

gaudi_add_library(ROMon src/*.cpp rpc/*.cpp
                  PUBLIC_HEADERS ROMon
                  INCLUDE_DIRS XercesC
                  LINK_LIBRARIES OnlineKernel dim XercesC)

gaudi_add_library(XMLRPC rpc/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES OnlineKernel)

gaudi_install_python_modules()

gaudi_env(SET ROMONDATA \${ROMONROOT}/xml)

