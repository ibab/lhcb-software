################################################################################
# Package: RichMCTools
################################################################################
gaudi_subdir(RichMCTools v3r19)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Kernel/MCInterfaces
                         Kernel/PartProp
                         Rich/RichKernel)

gaudi_add_module(RichMCTools
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES RichDetLib LinkerEvent MCEvent PartPropLib RichKernelLib)

