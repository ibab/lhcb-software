################################################################################
# Package: SiTools
################################################################################
gaudi_subdir(SiTools v1r10)

gaudi_depends_on_subdirs(Event/MCEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/MCInterfaces)

gaudi_add_module(SiTools
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES MCEvent GaudiAlgLib GaudiKernel LHCbKernel)

