################################################################################
# Package: BrunelSys
################################################################################
gaudi_subdir(BrunelSys v47r9)

gaudi_depends_on_subdirs(Rec/Brunel
				Pr/PrMCTools
				Tr/TrackFitter
				Tr/TrackMonitors
				Det/DDDB
				Det/DetCond
				Event/EventPacker
				Tf/TrackSys
				Tr/TrackTools
)

gaudi_add_test(QMTest QMTEST)
