################################################################################
# Package: Camera
################################################################################
gaudi_subdir(Camera v1r30)

gaudi_depends_on_subdirs(GaudiAlg)

# to publish own DIM service (without MonitorSvc) and to get UTGID
gaudi_depends_on_subdirs(Online/DIM
                         Online/OnlineKernel)

find_package(ROOT)
if(ROOT_VERSION_MAJOR EQUAL 6)
  find_package(ROOT COMPONENTS Core Cling Hist Gpad Graf Gui)
else()
  find_package(ROOT COMPONENTS Core Cint Hist Gpad Graf Gui)
endif()
find_package(Boost COMPONENTS regex)

gaudi_add_library(CameraLib
                    src/camline.C src/container.C src/container_ROOT.C
                    src/display.C src/proto.C
                  PUBLIC_HEADERS Camera
                  INCLUDE_DIRS ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Camera ${CMAKE_CURRENT_SOURCE_DIR}/src
                  LINK_LIBRARIES ROOT)

gaudi_add_module(Camera
                   src/*.cpp src/container.C
                 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES ROOT CameraLib GaudiAlgLib dim OnlineKernel)

gaudi_install_python_modules()

macro(cam_exe name)
  gaudi_add_executable(${name} ${ARGN}
                       LINK_LIBRARIES CameraLib)
endmacro()

cam_exe(CameraSwitch     camswitch.C)
cam_exe(CameraCommand    sendcmd.C)
cam_exe(CameraTest       testwarn.C)
cam_exe(CameraServer     sserver.C getnsave.C)
cam_exe(CameraWebServer  webserver.C)

#cam_exe(CameraSend sendone.C)
#cam_exe(CameraWebGet get.C)


set(dict_srcs ${CMAKE_CURRENT_SOURCE_DIR}/Camera/MessagePresenter.h
              ${CMAKE_CURRENT_SOURCE_DIR}/Camera/LinkDef.h)

if(ROOT_rootcint_CMD)
  set(dict_cmd ${ROOT_rootcint_CMD})
else()
  # ROOT 6
  set(dict_cmd ${ROOT_rootcling_CMD})
endif()

add_custom_command(OUTPUT MessagePresenterDict.cpp
                   COMMAND ${env_cmd} --xml ${env_xml}
                     ${dict_cmd} -f MessagePresenterDict.cpp -c
                           -inlineInputHeader
                           ${dict_srcs}
                   DEPENDS ${dict_srcs})

cam_exe(CameraGui src/MessagePresenter.C MessagePresenterDict.cpp)
target_link_libraries(CameraGui ${CMAKE_DL_LIBS})

#cam_exe(CameraViewContainer src/ViewCont.C MessagePresenterDict.cpp)
