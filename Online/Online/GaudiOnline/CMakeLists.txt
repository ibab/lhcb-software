################################################################################
# Package: GaudiOnline
################################################################################
gaudi_subdir(GaudiOnline v2r68)

gaudi_depends_on_subdirs(GaudiKernel
                         DAQ/MDF
                         Event/DAQEvent
                         Online/DIM
                         Online/OnlineBase
                         Online/OnlineKernel
                         Online/RawBankSizes)

find_package(PythonLibs)
find_package(tcmalloc)
find_package(AIDA)

gaudi_install_headers(GaudiOnline)
gaudi_add_module(GaudiOnline src/*.cpp
                 INCLUDE_DIRS PythonLibs AIDA
                 LINK_LIBRARIES PythonLibs GaudiKernel dim DAQEventLib MDFLib OnlineKernel)

gaudi_install_python_modules()

gaudi_add_executable(GaudiOnlineExe  main/main.cpp       LINK_LIBRARIES GaudiKernel tcmalloc ${CMAKE_DL_LIBS})
gaudi_add_executable(GaudiCheckpoint main/checkpoint.cpp LINK_LIBRARIES GaudiKernel OnlineBase tcmalloc ${CMAKE_DL_LIBS})
