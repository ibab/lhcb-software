################################################################################
# Package: GaudiCheckpointing
################################################################################
gaudi_subdir(GaudiCheckpointing v0r25)

gaudi_depends_on_subdirs(GaudiKernel
                         Online/Checkpointing
                         Online/DIM
                         Online/GaudiOnline
                         Online/OnlineBase)

# this package works only on 64 bits platforms
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  gaudi_add_module(GaudiCheckpointing src/*.cpp
                   LINK_LIBRARIES GaudiKernel Checkpointing dim OnlineBase)
  set_property(TARGET GaudiCheckpointing APPEND PROPERTY COMPILE_FLAGS " -g -O0")
  set_property(TARGET GaudiCheckpointing APPEND PROPERTY LINK_FLAGS " -g -O0")
endif()
