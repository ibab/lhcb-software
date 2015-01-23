################################################################################
# Package: BenderSys
################################################################################
gaudi_subdir(BenderSys v25r4)

gaudi_depends_on_subdirs(Ex/BenderExample
                         Phys/Bender
                         Phys/BenderTools
                         Phys/BenderUser
                         GaudiObjDesc #TEMP v23r4
                         Phys/LoKiPhys #TEMP v23r4
                         Event/RecEvent #TEMP v23r4
                         Tutorial/BenderTutor)

