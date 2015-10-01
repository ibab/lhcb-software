################################################################################
# Package: OnlineHistDB
################################################################################
gaudi_subdir(OnlineHistDB v8r3)

find_package(Oracle)

gaudi_add_library(OnlineHistDB src/*.cpp
                  PUBLIC_HEADERS OnlineHistDB
                  INCLUDE_DIRS Oracle
                  LINK_LIBRARIES Oracle)

gaudi_add_executable(FixDoubleAlgName main/FixDoubleAlgName.cpp
                     LINK_LIBRARIES OnlineHistDB)
gaudi_add_executable(ChangeTaskName main/ChangeTaskName.cpp
                     LINK_LIBRARIES OnlineHistDB)
gaudi_add_executable(ChangeAlgorithmName main/ChangeAlgorithmName.cpp
                     LINK_LIBRARIES OnlineHistDB)
