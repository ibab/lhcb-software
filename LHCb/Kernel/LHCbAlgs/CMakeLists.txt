################################################################################
# Package: LHCbAlgs
################################################################################
gaudi_subdir(LHCbAlgs v3r8)

gaudi_depends_on_subdirs(Det/DDDB
                         Det/DetDesc
                         Event/DAQEvent
			 DAQ/DAQKernel
                         Event/HltEvent
                         Event/L0Event
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/HltInterfaces
                         Kernel/LHCbKernel
                         Kernel/XMLSummaryKernel)

find_package(Boost COMPONENTS filesystem)

find_package(CASTOR) # runtime, only for the tests

gaudi_add_module(LHCbAlgs
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost DetDescLib DAQEventLib DAQKernelLib HltEvent L0Event RecEvent TrackEvent GaudiAlgLib LHCbKernel HltInterfaces HltEvent)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
