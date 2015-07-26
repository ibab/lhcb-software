################################################################################
# Package: HltLine
################################################################################
gaudi_subdir(HltLine v7r12)

gaudi_depends_on_subdirs(DAQ/DAQSys
                         GaudiAlg
                         Hlt/HltBase
                         Hlt/HltCommon
                         Kernel/SelectionLine
                         Phys/SelPy)

find_package(Boost)

gaudi_add_module(HltLine
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost GaudiAlgLib HltBase SelectionLine)

set_property(DIRECTORY PROPERTY CONFIGURABLE_USER_MODULES HltLine.HltLinesConfigurableUser)
gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
