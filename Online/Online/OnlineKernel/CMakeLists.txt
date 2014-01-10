################################################################################
# Package: OnlineKernel
################################################################################
gaudi_subdir(OnlineKernel v2r13)

gaudi_depends_on_subdirs(Online/OnlineBase)

find_package(PythonLibs)

gaudi_add_library(OnlineKernel
                    src/RTL/*.cpp
                    src/MBM/*.cpp
                  PUBLIC_HEADERS OnlineKernel
                  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/OnlineKernel
                  LINK_LIBRARIES OnlineBase)

gaudi_add_dictionary(OnlineKernel dict/dictionary.h  dict/dictionary.xml
                     INCLUDE_DIRS PythonLibs
                     LINK_LIBRARIES OnlineKernel PythonLibs)


gaudi_add_executable(gentest     main/gentest.cpp LINK_LIBRARIES ${CMAKE_DL_LIBS})
gaudi_add_executable(mbm_remove  main/mbm_remove.cpp LINK_LIBRARIES ${CMAKE_DL_LIBS})

# FIXME: In CMT the test application is called 'test', but in CMake it causes a segfault
gaudi_add_executable(onlinekernel_test        main/wt.cpp LINK_LIBRARIES ${CMAKE_DL_LIBS})
# for backward compatibility
gaudi_alias(test.exe onlinekernel_test.exe)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
