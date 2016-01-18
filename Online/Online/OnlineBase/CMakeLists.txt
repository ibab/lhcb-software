################################################################################
# Package: OnlineBase
################################################################################
gaudi_subdir(OnlineBase v1r39)

find_package(Boost COMPONENTS system)
gaudi_add_library(OnlineBase
                    src/ASIO/*.cpp
                    src/AMS/*.cpp
                    src/CPP/*.cpp
                    src/MBM/*.cpp
                    src/NET/*.cpp
                    src/PUBAREA/*.cpp
                    src/RTL/*.cpp
                    src/SCR/*.cpp
                    src/TAN/*.cpp
                    src/WT/*.cpp
                  PUBLIC_HEADERS OnlineBase/.
                  INCLUDE_DIRS Boost ${CMAKE_CURRENT_SOURCE_DIR}/OnlineBase
                  LINK_LIBRARIES Boost ${CMAKE_DL_LIBS} -lrt)

#message(WARNING "OnlineBase is used in CMT to define compile options")

