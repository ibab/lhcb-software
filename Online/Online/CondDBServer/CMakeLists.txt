################################################################################
# Package: CondDBServer
################################################################################
gaudi_subdir(CondDBServer v1r5)

gaudi_depends_on_subdirs(Online/DIM)

find_package(Boost COMPONENTS system)
find_package(CORAL)
find_package(COOL)

# runtime
find_package(SQLite)
find_package(Oracle)

gaudi_add_executable(condDBServer src/*.cpp
                     INCLUDE_DIRS Boost CORAL COOL
                     LINK_LIBRARIES dim Boost CORAL COOL)

gaudi_install_scripts()

gaudi_env(SET CORAL_DBLOOKUP_PATH \${CONDDBSERVERROOT}/cmt
          SET CORAL_AUTH_PATH     \${HOME}/private
          SET DIM_DNS_NODE            ecs01.lbdaq.cern.ch)
