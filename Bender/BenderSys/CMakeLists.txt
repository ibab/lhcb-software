################################################################################
# Package: BenderSys
################################################################################
gaudi_subdir(BenderSys v25r7p2)

gaudi_depends_on_subdirs(Ex/BenderExample
                         Phys/Bender
                         Phys/BenderTools
                         Phys/BenderUser
                         Tutorial/BenderTutor)

gaudi_add_test(QMTest QMTEST)
