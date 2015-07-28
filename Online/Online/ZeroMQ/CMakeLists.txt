################################################################################
# Package: ZeroMQ
################################################################################

gaudi_subdir(ZeroMQ v4r1p3)

add_definitions(-D_REENTRANT -D_THREAD_SAFE -DZMQ_USE_POLL)
gaudi_add_library(zmq src/*.cpp
                  PUBLIC_HEADERS zmq
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} -lrt)

gaudi_install_headers(ZeroMQ)

gaudi_add_module(ZeroMQ component/*.cpp
                 LINK_LIBRARIES GaudiKernel zmq)

# gaudi_add_dictionary(ZeroMQ
#                      dict/ZeroMQDict.h
#                      dict/ZeroMQDict.xml
#                      INCLUDE_DIRS zmq
#                      LINK_LIBRARIES GaudiKernel zmq
#                      OPTIONS "-U__MINGW32__")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/zmq)
gaudi_add_executable(curve_keygen tools/curve_keygen.c LINK_LIBRARIES zmq)
