################################################################################
# Package: IPMI
################################################################################

gaudi_subdir(IPMI v1r18)

gaudi_depends_on_subdirs(Online/DIM
      )

gaudi_add_library(IPMILib src/NewLib/*.cpp
                  PUBLIC_HEADERS IPMILib
#                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES dim crypto )

add_definitions( -DHAVE_CONFIG_H -UPACKAGE_NAME -UPACKAGE_VERSION)
string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
# Ignore pedantic warnings
#string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
#string(REPLACE "-pedantic" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
#string(REPLACE "-Wwrite-strings" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_CONFIG_H -DHAVE_PATH_H")
#set(CMAKE_C_FLAGS "-DHAVE_CONFIG_H -DHAVE_PATH_H -fno-strict-aliasing -Wreturn-type -lm")
#add_definitions( -UPACKAGE_NAME -UPACKAGE_VERSION)
#
#gaudi_common_add_build(
#                    src/lib/*.c
#                    src/plugins/imb/*.c
#                    src/plugins/ipmi_intf.c
#                    src/plugins/lan/*.c
#                    src/plugins/lanplus/*.c
#                    src/plugins/open/*.c
#                    src/plugins/serial/*.c
#                    src/ipmishell.c
#)
#
#add_library(IPMIlib STATIC ${srcs})
#target_link_libraries(IPMILib ${ARG_LINK_LIBRARIES})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IPMI ${CMAKE_CURRENT_SOURCE_DIR}/IPMI/include)
#gaudi_install_headers(MyHeaders)
#message("---------------------" ${ARG_LINK_LIBRARIES})
#macro(ipmi_exe name)
#  gaudi_add_executable(${name} ${ARGN} LINK_LIBRARIES IPMIlib crypto)
#endmacro()
#
#ipmi_exe(ipmitool src/ipmitool.c)
#ipmi_exe(ipmiltest src/ipmiltest.c)
#gaudi_common_add_build(Applib/*.cpp)
#add_library(Applib STATIC ${srcs})
#target_link_libraries(Applib ${ARG_LINK_LIBRARIES})
macro(Applexe name)
  gaudi_add_executable(${name} ${ARGN} LINK_LIBRARIES IPMILib -pthread)
endmacro()

Applexe(Test1 src/Applications/Test1.cpp)
Applexe(ipmiSrv src/Applications/ipmiSrv.cpp)
