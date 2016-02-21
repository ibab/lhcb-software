################################################################################
# Package: BenderSys
################################################################################
gaudi_subdir(BenderSys v28r1)

gaudi_depends_on_subdirs(Ex/BenderExample
                         Phys/Bender
                         Phys/BenderTools
                         Phys/BenderUser
                         Tutorial/BenderTutor)

gaudi_add_test(QMTest QMTEST)
