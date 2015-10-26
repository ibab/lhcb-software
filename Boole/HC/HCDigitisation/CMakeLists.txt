################################################################################
# Package: HCDigitisation
################################################################################
gaudi_subdir(HCDigitisation v1r1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/MCEvent
                         Event/GenEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

gaudi_add_module(HCDigitisation
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent 
                 LINK_LIBRARIES DAQEventLib GenEvent MCEvent GaudiAlgLib LHCbKernel)

