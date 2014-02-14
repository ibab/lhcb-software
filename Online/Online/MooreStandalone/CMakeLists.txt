################################################################################
# Package: MooreStandalone
################################################################################
gaudi_subdir(MooreStandalone v0r1)

gaudi_depends_on_subdirs(GaudiKernel
                         Online/UPI
                         Online/OnlineBase
                         Online/OnlineKernel
                         Online/GaudiOnline)

gaudi_install_headers(MooreStandalone)
gaudi_add_library(MooreStandalone src/*.cpp
                 INCLUDE_DIRS 
                 LINK_LIBRARIES GaudiKernel GaudiOnline UPI OnlineKernel OnlineBase)

