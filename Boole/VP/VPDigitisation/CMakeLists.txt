################################################################################
# Package: VPDigitisation
################################################################################
gaudi_subdir(VPDigitisation v1r7)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

find_package(GSL)

gaudi_add_module(VPDigitisation
                 src/*.cpp
                 INCLUDE_DIRS GSL AIDA Event/DigiEvent 
                 LINK_LIBRARIES GSL VPDetLib LinkerEvent MCEvent GaudiAlgLib LHCbKernel)

