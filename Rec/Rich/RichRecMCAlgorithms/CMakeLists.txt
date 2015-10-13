################################################################################
# Package: RichRecMCAlgorithms
################################################################################
gaudi_subdir(RichRecMCAlgorithms v1r6p1)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Event/MCEvent
                         GaudiKernel
                         Kernel/MCInterfaces
                         Rich/RichKernel
                         Rich/RichRecBase)

gaudi_add_module(RichRecMCAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiKernel RichKernelLib RichRecBase)

