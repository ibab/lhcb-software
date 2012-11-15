################################################################################
# Package: MDF_ROOT
################################################################################
gaudi_subdir(MDF_ROOT v1r8)

gaudi_depends_on_subdirs(DAQ/MDF)

find_package(ROOT COMPONENTS RIO)

gaudi_add_library(MDF_ROOT
                  src/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT MDFLib)

