################################################################################
# Package: Checkpointing
################################################################################
gaudi_subdir(Checkpointing v0r26)

# this package works only on 64 bits platforms
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")

  # Ignore variadic macros warnings (unavoidable)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros -Wno-unused")

  # Ignore pedantic warnings (unavoidable)
  string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # disable optimizations (on all BUILD_TYPEs)
  set(CMAKE_CXX_FLAGS_RELEASE "-g -O0")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O0")

  gaudi_add_library(Checkpointing aux/Image.cpp
                    PUBLIC_HEADERS Checkpointing
                    LINK_LIBRARIES ${CMAKE_DL_LIBS} -pthread)

  gaudi_add_library(Chkpt         src/*.cpp
                    NO_PUBLIC_HEADERS
                    LINK_LIBRARIES ${CMAKE_DL_LIBS} -pthread)

  # do not use pthreads
  string(REPLACE "-pthread" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  # make a static executable
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

  gaudi_add_executable(restore aux/main.cpp)

  gaudi_install_scripts()
  gaudi_add_test(QMTest QMTEST)
endif()
