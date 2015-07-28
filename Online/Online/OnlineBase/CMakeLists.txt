################################################################################
# Package: OnlineBase
################################################################################
gaudi_subdir(OnlineBase v1r36)

gaudi_add_library(OnlineBase
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
                  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/OnlineBase
                  LINK_LIBRARIES ${CMAKE_DL_LIBS} -lrt)

message(WARNING "OnlineBase is used in CMT to define compile options")

