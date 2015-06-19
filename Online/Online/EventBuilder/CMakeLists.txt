################################################################################
# Package: EventBuilder
################################################################################
gaudi_subdir(EventBuilder v0r12)

gaudi_depends_on_subdirs(Event/DAQEvent
                         DAQ/MDF
                         Online/DIM
                         Online/OnlineKernel
                         Online/Gaucho
                         GaudiKernel)

find_package(AIDA)

gaudi_add_module(EventBuilder src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES GaudiKernel MDFLib OnlineKernel dim)

gaudi_install_headers(EventBuilder)
