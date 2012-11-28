################################################################################
# Package: RichTools
################################################################################
gaudi_subdir(RichTools v2r41)

gaudi_depends_on_subdirs(Det/RichDet
                         Kernel/PartProp
                         Rich/RichKernel)

gaudi_add_module(RichTools
                 src/*.cpp
                 LINK_LIBRARIES RichDetLib PartPropLib RichKernelLib)

