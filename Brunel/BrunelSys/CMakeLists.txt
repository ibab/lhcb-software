################################################################################
# Package: BrunelSys
################################################################################
gaudi_subdir(BrunelSys v48r1)

gaudi_depends_on_subdirs(Rec/Brunel
)

gaudi_add_test(QMTest QMTEST)
