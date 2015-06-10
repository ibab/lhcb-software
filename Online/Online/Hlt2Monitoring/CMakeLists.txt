################################################################################
# Package: Gaucho
################################################################################
gaudi_subdir(Hlt2Monitoring v0r1)

gaudi_depends_on_subdirs(GaudiKernel
                         Online/DIM
                         Online/ZeroMQ
                         Online/OnlineKernel)

find_package(Boost COMPONENTS regex serialization)

gaudi_add_module(Hlt2MonitoringLib src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES GaudiKernel Boost zmq OnlineKernel)

gaudi_install_headers(Hlt2Monitoring)