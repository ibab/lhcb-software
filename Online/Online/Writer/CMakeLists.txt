################################################################################
# Package: Writer
################################################################################
gaudi_subdir(Writer v1r49)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/DAQEvent
                         Online/DIM
                         Online/OnlineKernel)

#find_package(ROOT)

gaudi_add_library(Writer src/*.cpp
                  PUBLIC_HEADERS Writer
                  LINK_LIBRARIES DAQEventLib MDFLib OnlineKernel dim)
set_property(TARGET Writer APPEND PROPERTY COMPILE_DEFINITIONS BUILD_WRITER=1)

message(WARNING "library containing plugins!!!")
gaudi_generate_componentslist(Writer)
