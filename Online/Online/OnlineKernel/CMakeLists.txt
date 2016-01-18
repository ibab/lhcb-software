################################################################################
# Package: OnlineKernel
################################################################################
gaudi_subdir(OnlineKernel v2r25)

gaudi_depends_on_subdirs(Online/OnlineBase)

find_package(PythonLibs)
find_package(ROOT)

gaudi_add_library(OnlineKernel
                    src/RTL/*.cpp
                    src/MBM/*.cpp
                    src/MICFSM/*.cpp
                  PUBLIC_HEADERS OnlineKernel/.
                  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/OnlineKernel
                  LINK_LIBRARIES OnlineBase)

gaudi_add_dictionary(OnlineKernel dict/dictionary.h  dict/dictionary.xml
                     INCLUDE_DIRS PythonLibs
                     LINK_LIBRARIES OnlineKernel PythonLibs)

gaudi_add_executable(gentest     main/gentest.cpp LINK_LIBRARIES ${CMAKE_DL_LIBS})
gaudi_add_executable(checkpoint  main/checkpoint.cpp LINK_LIBRARIES OnlineBase ${CMAKE_DL_LIBS})
gaudi_add_executable(mbm_remove  main/mbm_remove.cpp LINK_LIBRARIES ${CMAKE_DL_LIBS})

# FIXME: In CMT the test application is called 'test', but in CMake it causes a segfault
gaudi_add_executable(onlinekernel_test        main/wt.cpp LINK_LIBRARIES ${CMAKE_DL_LIBS})
# for backward compatibility
gaudi_alias(test.exe onlinekernel_test.exe)

# Disable generation of ConfUserDB (must be done before gaudi_install_python_modules)
set_directory_properties(PROPERTIES CONFIGURABLE_USER_MODULES None)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
##gaudi_env(SET gentest_exe   "`which gentest.exe`")
