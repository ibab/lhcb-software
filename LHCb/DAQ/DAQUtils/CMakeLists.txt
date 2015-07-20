################################################################################
# Package: DAQUtils
################################################################################
gaudi_subdir(DAQUtils v1r13p1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/RecEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiSvc
			 DAQ/DAQKernel)

find_package(RELAX)

gaudi_add_module(DAQUtils
                 src/*.cpp
                 INCLUDE_DIRS RELAX
                 LINK_LIBRARIES RELAX DAQEventLib DAQKernelLib RecEvent GaudiAlgLib GaudiKernel)

gaudi_install_headers(Event)


gaudi_add_test(QMTest QMTEST)
