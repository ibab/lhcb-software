################################################################################
# Package: DIM
################################################################################

gaudi_subdir(DIM v20r7p4)

find_package(X11)
find_package(Motif)

# Ignore pedantic warnings
string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REPLACE "-pedantic" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
string(REPLACE "-Wwrite-strings" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
# Ignore unused arguments
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused")

add_definitions(-DPROTOCOL=1 -DMIPSEL -Df2cFortran)
gaudi_add_library(dim
                    src/conn_handler.c src/copy_swap.c src/dic.c src/dim_thr.c
                    src/dis.c src/dll.c src/dna.c src/dtq.c src/hash.c
                    src/open_dns.c src/sll.c src/swap.c src/tcpip.c
                    src/utilities.c src/diccpp.cxx src/dimcpp.cxx src/discpp.cxx
                    src/tokenstring.cxx
                  PUBLIC_HEADERS dim/.
                  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/dim
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} -lrt)

macro(dim_exe name)
  gaudi_add_executable(${name} ${ARGN} LINK_LIBRARIES dim)
endmacro()

dim_exe(dns src/dns.c)

include_directories(${MOTIF_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/did)
dim_exe(did src/did/make_did.c)
target_link_libraries(did ${MOTIF_LIBRARIES} ${X11_LIBRARIES} ${X11_Xt_LIB})

dim_exe(test_server src/examples/test_server.c)
dim_exe(test_client src/examples/test_client.c)
#dim_exe(test_big_server src/examples/test_big_server.c)
#dim_exe(test_big_client src/examples/test_big_client.c)
# dim_exe(demo_server src/examples/demo_server.c)
# dim_exe(demo_client src/examples/demo_client.c)
#dim_exe(rpc_server src/examples/rpc_server.cxx)
#dim_exe(rpc_client src/examples/rpc_client.cxx)
#dim_exe(pvss_dim_server src/examples/pvss_dim_server.cxx)
#dim_exe(pvss_dim_client src/examples/pvss_dim_client.cxx)
#dim_exe(benchServer src/benchmark/benchServer.cxx)
#dim_exe(benchClient src/benchmark/benchClient.cxx)
dim_exe(check_dns src/util/check_dns.cxx)
dim_exe(dimbridge src/util/dimbridge.cxx)
dim_exe(dim_get_service src/util/dim_get_service.c)
dim_exe(dim_send_command src/util/dim_send_command.c)
# dim_exe(bigServer src/benchmark/bigServer.cxx)
# dim_exe(bigClient src/benchmark/bigClient.cxx)
