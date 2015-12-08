################################################################################
# Package: Checkpointing
################################################################################
gaudi_subdir(Checkpointing v0r25)

# this package works only on 64 bits platforms
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

  # Ignore variadic macros warnings (unavoidable)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros -Wno-unused")

  # Ignore pedantic warnings (unavoidable)
  string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "-O2" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  string(REPLACE "-Og" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

  gaudi_add_library(Checkpointing aux/Image.cpp
                    PUBLIC_HEADERS Checkpointing
                    LINK_LIBRARIES ${CMAKE_DL_LIBS} -pthread)
  set_property(TARGET Checkpointing APPEND PROPERTY COMPILE_FLAGS " -g -O0")
  set_property(TARGET Checkpointing APPEND PROPERTY LINK_FLAGS " -g -O0")

  gaudi_add_library(Chkpt         src/*.cpp
                    NO_PUBLIC_HEADERS
                    LINK_LIBRARIES ${CMAKE_DL_LIBS} -pthread)

  #set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
  #  set(CMAKE_EXE_LINK_DYNAMIC_C_FLAGS)       # remove -Wl,-Bdynamic
  set(CMAKE_EXE_LINKER_FLAGS)
  string(REPLACE "-pthread" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  #gaudi_add_executable(restore aux/main.cpp)
  add_executable(restore aux/main.cpp)
  set_target_properties(restore PROPERTIES SUFFIX .exe)
  set_property(TARGET restore PROPERTY COMPILE_FLAGS " -g -O0 -fPIE" )
  if ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    set_property(TARGET restore PROPERTY LINK_FLAGS " -g -O0 -static -static-libgcc -static-libstdc++ -pie" )
  else()
    set_property(TARGET restore PROPERTY LINK_FLAGS " -g -O0 -static -static-libgcc -static-libstdc++" )
  endif()
  set_property(TARGET restore PROPERTY LINK_DEPENDS_NO_SHARED ON)
  install(TARGETS restore RUNTIME DESTINATION bin OPTIONAL)

  gaudi_install_scripts()
  gaudi_add_test(QMTest QMTEST)
endif()
