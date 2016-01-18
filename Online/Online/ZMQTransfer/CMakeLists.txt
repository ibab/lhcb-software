################################################################################
# Package: ZMQTransferLib
################################################################################
gaudi_subdir(ZMQTransfer v0r1)

gaudi_depends_on_subdirs(Online/ZeroMQ
			 Online/OnlineBase
			 Online/GaudiOnline)

find_package(Boost COMPONENTS regex serialization filesystem system thread)

gaudi_add_library(ZMQTransfer src/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Boost ${CMAKE_CURRENT_SOURCE_DIR}/ZMQTransfer
                  LINK_LIBRARIES OnlineBase Boost zmq ${CMAKE_DL_LIBS} -lrt)

gaudi_add_module(ZMQTransferComp components/*.cpp
                 INCLUDE_DIRS Boost ${CMAKE_CURRENT_SOURCE_DIR}/ZMQTransfer GaudiOnline
                 LINK_LIBRARIES GaudiKernel OnlineKernel GaudiOnline ZMQTransfer zmq ${CMAKE_DL_LIBS} -lrt)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ZeroMQ/src)
