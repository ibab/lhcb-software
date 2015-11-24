################################################################################
# Package: BooleSys
################################################################################
gaudi_subdir(BooleSys v29r12)

gaudi_depends_on_subdirs(Digi/Boole
)

gaudi_add_test(QMTest QMTEST)
