################################################################################
# Package: RawBankSizes
################################################################################
gaudi_subdir(RawBankSizes v2r22)

gaudi_depends_on_subdirs(GaudiAlg
                         Event/DAQEvent
                         DAQ/MDF
                         Online/OnlineHistDB)

find_package(AIDA)

# Only for testing:
# gau di_depends_on_subdirs(GaudiSvc Online/OnlineKernel Online/Gaucho Online/GaudiOnline)

gaudi_add_module(RawBankSizes RawBankSizes.cpp Structure.cpp
                 INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/RawBankSizes
                 LINK_LIBRARIES GaudiAlgLib DAQEventLib)

gaudi_add_executable(MakePages MakePages.cpp LINK_LIBRARIES OnlineHistDB)
gaudi_add_executable(MakeOptions MakeOptions.cpp LINK_LIBRARIES DAQEventLib)

if(GAUDI_USE_EXE_SUFFIX)
  set(exe_suff .exe)
else()
  set(exe_suff)
endif()

add_custom_target(runMakeOptions ALL
                  COMMAND ${env_cmd} --xml ${env_xml}
                    MakeOptions${exe_suff}
                  DEPENDS MakeOptions)

add_custom_target(WriteOptions ALL
                  COMMAND ${env_cmd} --xml ${env_xml}
                    ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/src/WriteOptions.py
                  DEPENDS runMakeOptions)
