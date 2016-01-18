################################################################################
# Package: Writer
################################################################################
gaudi_subdir(Writer v1r52)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/DAQEvent
                         Online/DIM
                         Online/OnlineKernel)

gaudi_add_module(Writer src/*.cpp
                 LINK_LIBRARIES DAQEventLib MDFLib OnlineKernel dim)
set_property(TARGET Writer APPEND PROPERTY COMPILE_DEFINITIONS BUILD_WRITER=1)
