################################################################################
# Package: Presenter
################################################################################
gaudi_subdir(Presenter v3r13)

gaudi_depends_on_subdirs(Online/Gaucho
                         Online/OMAlib
                         Online/OnlineHistDB
                         Online/PresenterDoc)

# first try ROOT 6 (Cling)
find_package(ROOT COMPONENTS Core Cling Thread Gui Graf3d Ged GuiHtml Gpad)
if(ROOT_FOUND)
  set(dict_cmd ${ROOT_rootcling_CMD})
else()
  # otherwise try ROOT 5 (Cint)
  find_package(ROOT COMPONENTS Core Cint Thread Gui Graf3d Ged GuiHtml Gpad)
  set(dict_cmd ${ROOT_rootcint_CMD})
endif()

find_package(Boost COMPONENTS filesystem date_time program_options serialization
                              thread system regex)

if(Boost_VERSION STREQUAL "105900")
  # See https://svn.boost.org/trac/boost/ticket/11743
  add_definitions(-UNDEBUG)
endif()

set(dict_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/cintBootstrap.h
              ${CMAKE_CURRENT_SOURCE_DIR}/src/LinkDef.h)

if(ROOT_HAS_PCMS)
  # this section is copied and adapted from gaudi_add_dictionary in GaudiProjectConfig
  file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  add_custom_command(OUTPUT cintBootstrapDict.cpp cintBootstrapDict_rdict.pcm ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cintBootstrapDict_rdict.pcm
                     COMMAND ${env_cmd} --xml ${env_xml}
                       ${dict_cmd} -f cintBootstrapDict.cpp -c -inlineInputHeader
                             ${dict_srcs}
                     COMMAND ${CMAKE_COMMAND} -E copy cintBootstrapDict_rdict.pcm ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cintBootstrapDict_rdict.pcm
                     DEPENDS ${dict_srcs})
  add_custom_target(cintBootstrapDictGen ALL
                    DEPENDS cintBootstrapDict.cpp)

  install(FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cintBootstrapDict_rdict.pcm DESTINATION lib OPTIONAL)

else()

  add_custom_command(OUTPUT cintBootstrapDict.h cintBootstrapDict.cpp
                     COMMAND ${env_cmd} --xml ${env_xml}
                       ${dict_cmd} -f cintBootstrapDict.cpp -c -inlineInputHeader
                             ${dict_srcs}
                     DEPENDS ${dict_srcs})

endif()

gaudi_add_executable(presenter src/*.cpp cintBootstrapDict.cpp
                     INCLUDE_DIRS Boost ROOT
                     LINK_LIBRARIES Boost ROOT GauchoLib OnlineHistDB OMAlib)

gaudi_install_headers(Presenter)
