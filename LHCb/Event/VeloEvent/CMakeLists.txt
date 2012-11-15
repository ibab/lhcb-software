################################################################################
# Package: VeloEvent
################################################################################
gaudi_subdir(VeloEvent v16r10)

gaudi_depends_on_subdirs(DAQ/Tell1Kernel
                         GaudiObjDesc
                         Kernel/LHCbKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml
                  DESTINATION VeloEvent)

god_build_dictionary(xml/*.xml
                     HEADERS_DESTINATION VeloEvent
                     INCLUDE_DIRS DAQ/Tell1Kernel
                     LINK_LIBRARIES LHCbKernel)

gaudi_install_headers(VeloEvent)

