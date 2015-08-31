################################################################################
# Package: Camera
################################################################################
gaudi_subdir(Camera v1r34)

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
find_package(Boost COMPONENTS regex filesystem)

gaudi_add_library(CameraLib
                    src/lib/*.C
                  PUBLIC_HEADERS Camera
                  INCLUDE_DIRS ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Camera ${CMAKE_CURRENT_SOURCE_DIR}/src
                  LINK_LIBRARIES ROOT)

gaudi_add_module(Camera
                   src/component/*.cpp src/lib/container.C
                 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES ROOT CameraLib GaudiAlgLib dim OnlineKernel)

gaudi_install_python_modules()

macro(cam_exe name)
  gaudi_add_executable(${name} ${ARGN}
                       LINK_LIBRARIES Boost CameraLib)
endmacro()

cam_exe(CameraSwitch     ../src/applications/camswitch.C)
cam_exe(CameraCommand    ../src/applications/sendcmd.C)
cam_exe(CameraTest       ../src/applications/testwarn.C)
cam_exe(CameraServer     ../src/applications/sserver.C ../src/applications/getnsave.C)
cam_exe(CameraWebServer  ../src/applications/webserver.C)

#cam_exe(CameraSend ../src/applications/sendone.C)
#cam_exe(CameraWebGet ../src/applications/get.C)


set(dict_srcs ${CMAKE_CURRENT_SOURCE_DIR}/Camera/MessagePresenter.h
              ${CMAKE_CURRENT_SOURCE_DIR}/Camera/LinkDef.h)

if(ROOT_rootcint_CMD)
  set(dict_cmd ${ROOT_rootcint_CMD})
else()
  # ROOT 6
  set(dict_cmd ${ROOT_rootcling_CMD})
endif()

set(dict_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/cintBootstrap.h
              ${CMAKE_CURRENT_SOURCE_DIR}/src/LinkDef.h)

if(ROOT_HAS_PCMS)
  # this section is copied and adapted from gaudi_add_dictionary in GaudiProjectConfig
  file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  add_custom_command(OUTPUT cintBootstrapDictCamera.cpp cintBootstrapDictCamera_rdict.pcm ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cintBootstrapDictCamera_rdict.pcm
                     COMMAND ${env_cmd} --xml ${env_xml}
                       ${dict_cmd} -f cintBootstrapDictCamera.cpp -c -inlineInputHeader
                             ${dict_srcs}
                     COMMAND ${CMAKE_COMMAND} -E copy cintBootstrapDictCamera_rdict.pcm ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cintBootstrapDictCamera_rdict.pcm
                     DEPENDS ${dict_srcs})
  add_custom_target(cintBootstrapDictCameraGen ALL
                    DEPENDS cintBootstrapDictCamera.cpp)

  install(FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cintBootstrapDictCamera_rdict.pcm DESTINATION lib OPTIONAL)

else()

  add_custom_command(OUTPUT cintBootstrapDictCamera.h cintBootstrapDictCamera.cpp
                     COMMAND ${env_cmd} --xml ${env_xml}
                       ${dict_cmd} -f cintBootstrapDictCamera.cpp -c -inlineInputHeader
                             ${dict_srcs}
                     DEPENDS ${dict_srcs})

endif()

cam_exe(CameraGui src/applications/MessagePresenter.C src/applications/Elog.cpp src/applications/ElogDialog.cpp cintBootstrapDictCamera.cpp)
target_link_libraries(CameraGui ${CMAKE_DL_LIBS})
# ensure that cintBootstrapDictCamera.cpp is generated only once in parallel builds
add_dependencies(CameraGui cintBootstrapDictCameraGen)

#cam_exe(CameraViewContainer ../src/applications/ViewCont.C MessagePresenterDict.cpp)
