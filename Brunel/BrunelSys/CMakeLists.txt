################################################################################
# Package: BrunelSys
################################################################################
gaudi_subdir(BrunelSys v47r8)

gaudi_depends_on_subdirs(Rec/Brunel
				Pr/PrMCTools
				Tr/TrackFitter
				Tr/TrackMonitors
)

gaudi_add_test(QMTest QMTEST)
