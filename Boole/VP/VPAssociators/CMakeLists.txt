################################################################################
# Package: VPAssociators
################################################################################
gaudi_subdir(VPAssociators v2r4)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

find_package(GSL)

gaudi_add_module(VPAssociators
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Event/DigiEvent 
                 LINK_LIBRARIES GSL VPDetLib LinkerEvent MCEvent GaudiAlgLib LHCbKernel)

