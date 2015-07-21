################################################################################
# Package: AlignOnline
################################################################################
gaudi_subdir(AlignOnline v1r4)

gaudi_depends_on_subdirs(Alignment/AlignKernel
                         Det/DetDesc
                         Online/OnlineKernel
                         Online/EventBuilder) # Temporary - MM, 2015/04/07

# This is a special trick to handle the non standard headers'
# locations in Online.
find_path(RTL_include_dir RTL/rtl.h
          PATH_SUFFIXES ../include/OnlineBase)

gaudi_add_module(AlignOnline
                 src/*.cpp
                 INCLUDE_DIRS ${RTL_include_dir}
                 LINK_LIBRARIES AlignKernel DetDescLib OnlineKernel GaudiAlgLib)

gaudi_add_library(AlignOnlinelib
                 src/*.cpp
                 NO_PUBLIC_HEADERS
                 LINK_LIBRARIES AlignKernel DetDescLib OnlineKernel GaudiAlgLib)

gaudi_install_python_modules()
macro(ga_test name)
  gaudi_add_executable(${name} ${ARGN}
                       LINK_LIBRARIES AlignOnlinelib)
endmacro()
ga_test(ASDAnalyzer src/Tests/ASDAnalyzer.cpp)
