################################################################################
# Package: EventBase
################################################################################
gaudi_subdir(EventBase v3r1)

gaudi_depends_on_subdirs(GaudiKernel
                         GaudiObjDesc
                         Kernel/LHCbKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

god_build_dictionary(xml/*.xml
                     LINK_LIBRARIES GaudiKernel LHCbKernel)

