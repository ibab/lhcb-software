################################################################################
# Package: XMLSummaryKernel
################################################################################
gaudi_subdir(XMLSummaryKernel v1r13p1)

gaudi_depends_on_subdirs(GaudiConf
                         GaudiKernel
                         GaudiUtils
                         Kernel/XMLSummaryBase)

find_package(Boost COMPONENTS regex)
find_package(PythonLibs)
find_package(Xrootd) # runtime, only for the tests
# FIXME: workaround for bug https://sft.its.cern.ch/jira/browse/ROOT-6516
set_property(GLOBAL APPEND PROPERTY INCLUDE_PATHS ${XROOTD_INCLUDE_DIRS})

gaudi_add_module(XMLSummaryKernel
                 src/*.cpp
                 INCLUDE_DIRS Boost PythonLibs
                 LINK_LIBRARIES Boost PythonLibs GaudiKernel GaudiUtilsLib)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
