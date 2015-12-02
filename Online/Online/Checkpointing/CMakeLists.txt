################################################################################
# Package: Checkpointing
################################################################################
gaudi_subdir(Checkpointing v0r25)

# this package works only on 64 bits platforms
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

  # Ignore variadic macros warnings (unavoidable)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros")

  # Ignore pedantic warnings (unavoidable)
  string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

  gaudi_add_library(Checkpointing aux/Image.cpp
                    PUBLIC_HEADERS Checkpointing
                    LINK_LIBRARIES ${CMAKE_DL_LIBS} -pthread)
  set_property(TARGET Checkpointing APPEND PROPERTY COMPILE_FLAGS " -g -O0")
  set_property(TARGET Checkpointing APPEND PROPERTY LINK_FLAGS " -g -O0")

  gaudi_add_library(Chkpt         src/*.cpp
                    NO_PUBLIC_HEADERS
                    LINK_LIBRARIES ${CMAKE_DL_LIBS} -pthread)

  string(REPLACE "-pthread" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  gaudi_add_executable(restore aux/main.cpp)
  set_property(TARGET restore APPEND PROPERTY COMPILE_FLAGS " -g -O0 -fPIE" )
  set_property(TARGET restore APPEND PROPERTY LINK_FLAGS " -g -O0 -pie -static" )
  get_property(link_flg TARGET restore PROPERTY LINK_FLAGS )
  string(REPLACE "-rdynamic" "" link_flg ${link_flg} )
  set_property(TARGET restore PROPERTY LINK_FLAGS ${link_flg} )

  gaudi_install_scripts()
  gaudi_add_test(QMTest QMTEST)
endif()
