################################################################################
# Package: TrackSys
################################################################################
gaudi_subdir(TrackSys v9r10)

gaudi_depends_on_subdirs(Event/TrackEvent
                         Tf/FastVelo
                         Tf/PatAlgorithms
                         Tf/PatVelo
                         Tf/PatVeloTT
                         Tf/TsaAlgorithms
                         Tf/TsaKernel
                         Tr/TrackAssociators
                         Tr/TrackCheckers
                         Tr/TrackExtrapolators
                         Tr/TrackFitEvent
                         Tr/TrackFitter
                         Tr/TrackIdealPR
                         Tr/TrackInterfaces
                         Tr/TrackMCTools
                         Tr/TrackMonitors
                         Tr/TrackProjectors
                         Tr/TrackTools
                         Tr/TrackUtils
			 DAQ/DAQSys)

gaudi_install_python_modules()

