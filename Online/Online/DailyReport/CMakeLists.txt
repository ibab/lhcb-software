################################################################################
# Package: DailyReport
################################################################################
gaudi_subdir(DailyReport v1r2)

gaudi_depends_on_subdirs(Online/OMAlib)

find_package(Boost COMPONENTS filesystem date_time program_options serialization
                              thread system regex)

gaudi_add_executable(MakeReport src/*.cpp
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost OMAlib)
