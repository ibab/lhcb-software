################################################################################
# Package: Checkpointing
################################################################################
gaudi_subdir(Checkpointing v0r23)

# this package works only on 64 bits platforms
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  # Ignore variadic macros warnings (unavoidable)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvariadic-macros")
  # Ignore pedantic warnings (unavoidable)
  string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

  gaudi_add_library(Checkpointing aux/Image.cpp
                    PUBLIC_HEADERS Checkpointing
                    LINK_LIBRARIES ${CMAKE_DL_LIBS} -pthread)
  set_property(TARGET Checkpointing APPEND PROPERTY COMPILE_FLAGS " -g -O0")
  set_property(TARGET Checkpointing APPEND PROPERTY LINK_FLAGS " -g -O0")

  gaudi_add_library(Chkpt         src/*.cpp
                    NO_PUBLIC_HEADERS
                    LINK_LIBRARIES ${CMAKE_DL_LIBS})

  gaudi_add_executable(restore aux/main.cpp)
  set_property(TARGET restore APPEND PROPERTY COMPILE_FLAGS " -g -O0")
  set_property(TARGET restore APPEND PROPERTY LINK_FLAGS " -g -O0 -static")
  gaudi_install_scripts()
  gaudi_add_test(QMTest QMTEST)
endif()
