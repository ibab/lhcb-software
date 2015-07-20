################################################################################
# Package: LumiAlgs
################################################################################
gaudi_subdir(LumiAlgs v9r9)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/DAQEvent
                         Event/EventBase
                         Event/LumiEvent
                         Event/RecEvent
                         GaudiAlg
                         GaudiConf
                         Kernel/FSRAlgs
                         Kernel/HltInterfaces
                         Kernel/LHCbAlgs
                         Kernel/LHCbKernel
                         DAQ/DAQSys)

find_package(Boost COMPONENTS iostreams)
find_package(RELAX)

gaudi_add_module(LumiAlgs
                 src/*.cpp
                 INCLUDE_DIRS Boost RELAX Event/EventBase Event/LumiEvent Kernel/FSRAlgs
                 LINK_LIBRARIES Boost RELAX DetDescLib DAQEventLib RecEvent GaudiAlgLib HltInterfaces LHCbKernel)

gaudi_install_python_modules()

gaudi_add_dictionary(LumiAlgs
                     dict/LumiAlgsDict.h
                     dict/LumiAlgsDict.xml
                     LINK_LIBRARIES Boost GaudiKernel
                     OPTIONS "-U__MINGW32__")

gaudi_install_headers(LumiAlgs)

gaudi_add_test(QMTest QMTEST)

# FIXME: workaround for bug GAUDI-1007
set(test_names
    lumi0conddbchecker
    lumi0writer
    lumi1writer
    lumi2reader
    lumi3conftest
    lumi40fsrwriter
    lumi4fsrwriter
    lumi5fsrreader
    lumi60fsrintegrator
    lumi6fsrintegrator
    lumi7fsr2file
    lumi8merger
    lumi9fsrintegrator
    lumi9mergesmallfiles)

list(GET test_names 0 prereq)
list(REMOVE_AT test_names 0)

while(test_names)
  list(GET test_names 0 test_name)
  list(REMOVE_AT test_names 0)
  set_property(TEST LumiAlgs.${test_name} APPEND PROPERTY DEPENDS LumiAlgs.${prereq})
  set(prereq ${test_name})
endwhile()
