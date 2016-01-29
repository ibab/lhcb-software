################################################################################
# Package: RichReadout
################################################################################
gaudi_subdir(RichReadout v2r17)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/MCEvent
                         GaudiObjDesc
                         Kernel/MCInterfaces
                         Kernel/PartProp
                         Rich/RichKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_module(RichReadout
                 src/component/*.cpp
                 INCLUDE_DIRS AIDA Kernel/MCInterfaces
                 LINK_LIBRARIES RichDetLib MCEvent PartPropLib RichKernelLib)

gaudi_env(SET RICHREADOUTOPTS \${RICHREADOUTROOT}/options)

