################################################################################
# Package: Presenter
################################################################################
gaudi_subdir(Presenter v3r7)

gaudi_depends_on_subdirs(Online/Gaucho
                         Online/OMAlib
                         Online/OnlineHistDB
                         Online/PresenterDoc)

find_package(ROOT COMPONENTS Core Cint Thread Gui Graf3d Ged GuiHtml Gpad)
find_package(Boost COMPONENTS filesystem date_time program_options serialization
                              thread system regex)

set(dict_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/cintBootstrap.h
              ${CMAKE_CURRENT_SOURCE_DIR}/src/LinkDef.h)
add_custom_command(OUTPUT cintBootstrapDict.h cintBootstrapDict.cpp
                   COMMAND ${env_cmd} --xml ${env_xml}
                     ${ROOT_rootcint_CMD} -f cintBootstrapDict.cpp -c
                           -inlineInputHeader
                           ${dict_srcs}
                   DEPENDS ${dict_srcs})

gaudi_add_executable(presenter src/*.cpp cintBootstrapDict.cpp
                     INCLUDE_DIRS Boost ROOT
                     LINK_LIBRARIES Boost ROOT GauchoLib OnlineHistDB OMAlib)
