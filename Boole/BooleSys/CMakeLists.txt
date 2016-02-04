################################################################################
# Package: BooleSys
################################################################################
gaudi_subdir(BooleSys v30r0)

gaudi_depends_on_subdirs(Digi/Boole
)

gaudi_add_test(QMTest QMTEST)
