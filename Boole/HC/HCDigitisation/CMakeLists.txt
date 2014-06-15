################################################################################
# Package: HCDigitisation
################################################################################
gaudi_subdir(HCDigitisation v1r0)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/MCEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

gaudi_add_module(HCDigitisation
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent 
                 LINK_LIBRARIES DAQEventLib MCEvent GaudiAlgLib LHCbKernel)

