################################################################################
# Package: DIM
################################################################################

gaudi_subdir(ZeroMQ v4r0p5)

gaudi_add_library(zmq src/*.cpp
                  PUBLIC_HEADERS zmq/.
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} -lrt)

gaudi_add_executable(curve_keygen tools/curve_keygen.c LINK_LIBRARIES zmq)
