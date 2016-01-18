################################################################################
# Package: DailyReport
################################################################################
gaudi_subdir(DailyReport v1r4)

gaudi_depends_on_subdirs(Online/OMAlib)

find_package(Boost COMPONENTS filesystem date_time program_options serialization
                              thread system regex)

if(Boost_VERSION STREQUAL "105900")
  # See https://svn.boost.org/trac/boost/ticket/11743
  add_definitions(-UNDEBUG)
endif()

gaudi_add_executable(MakeReport src/*.cpp
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost OMAlib)
