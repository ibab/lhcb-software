gaudi_subdir(OnlineAlign v0r9)

gaudi_depends_on_subdirs(Online/GaudiOnline
                         Online/Gaucho)

find_package(ROOT COMPONENTS Minuit)

gaudi_add_module(OnlineAlign src/*.cpp
                 INCLUDE_DIRS ROOT
                 LINK_LIBRARIES GaudiOnline GauchoLib ROOT)

gaudi_install_headers(OnlineAlign)
