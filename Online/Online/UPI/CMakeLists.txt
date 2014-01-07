################################################################################
# Package: UPI
################################################################################
gaudi_subdir(UPI v1r24)

gaudi_depends_on_subdirs(Online/OnlineKernel)

gaudi_add_library(UPI src/*.cpp
                  PUBLIC_HEADERS UPI
                  LINK_LIBRARIES OnlineKernel)
set_property(TARGET UPI APPEND PROPERTY COMPILE_DEFINITIONS SCREEN)

gaudi_add_library(UPIR src/*.cpp
                  #PUBLIC_HEADERS UPI
                  NO_PUBLIC_HEADERS # already installed by UPI library
                  LINK_LIBRARIES OnlineKernel)
set_property(TARGET UPIR APPEND PROPERTY COMPILE_DEFINITIONS REMOTE)

gaudi_add_dictionary(UPI dict/dictionary.h dict/dictionary.xml
                     LINK_LIBRARIES UPI)

gaudi_install_python_modules()
