################################################################################
# Package: BrunelSys
################################################################################
gaudi_subdir(BrunelSys v47r9)

gaudi_depends_on_subdirs(Rec/Brunel
)

gaudi_add_test(QMTest QMTEST)
