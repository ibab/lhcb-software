################################################################################
# Package: Gaucho
################################################################################
gaudi_subdir(HltMonitoring v1r3)

gaudi_depends_on_subdirs(GaudiKernel
                         Kernel/HltInterfaces
                         Event/HltEvent
                         Det/DetDesc
                         Online/Hlt2Monitoring
                         Online/ZeroMQ)

find_package(Boost COMPONENTS regex serialization)

gaudi_add_module(HltMonitoringLib src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES HltEvent DetDescLib GaudiKernel Boost zmq)

gaudi_install_python_modules()
