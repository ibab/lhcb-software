################################################################################
# Package: BrunelSys
################################################################################
gaudi_subdir(BrunelSys v48r2)

gaudi_depends_on_subdirs(Rec/Brunel
                         Det/DDDB
                         Tf/FastVelo
                         Muon/MuonID
)

gaudi_add_test(QMTest QMTEST)
