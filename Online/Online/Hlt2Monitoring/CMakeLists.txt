################################################################################
# Package: Gaucho
################################################################################
gaudi_subdir(Hlt2Monitoring v1r0)

gaudi_depends_on_subdirs(GaudiKernel
                         Online/DIM
                         Online/ZeroMQ
                         Online/OnlineKernel)

find_package(Boost COMPONENTS regex serialization filesystem)
find_package(ROOT COMPONENTS Core RIO Hist)

gaudi_add_module(Hlt2MonitoringLib src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES GaudiKernel Boost ROOT zmq OnlineKernel)

gaudi_install_headers(Hlt2Monitoring)

gaudi_install_python_modules()
