# Example 2010 collisions options for RichParticleSearch

# Syntax is:
#   gaudirun.py myOptions.py
#
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/DataType-2011.py")


from Gaudi.Configuration import *
from Configurables import Brunel, LHCbApp, CondDB, CondDBAccessSvc, ApplicationMgr

# Run faster over dsts
from Configurables import DstConf
ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]
DstConf().EnableUnpack = ["Reconstruction"]

from Configurables import RichRecSysConf
rConf = RichRecSysConf("RichOfflineRec")
rConf.richTools().PixelCreatorType = "RawBufferWithPanelFlip" #background

##faster photon reconstruction
rConf.richTools().photonReco().FindUnambiguousPhotons = [False,False,False]
#rConf.richTools().PhotonRecoType = "EstiFromRadius"#use for faster photon reconstruction!!

#Brunel options
Brunel().DataType = "2011"
Brunel().EvtMax = -1
Brunel().PrintFreq = 1
Brunel().OutputType = "None"
Brunel().InitSequence      = ["Brunel"]
Brunel().RecoSequence      = ["RICH"]
Brunel().Histograms = "OfflineExpress"
#tags
LHCbApp().DDDBtag = "head-20110914"
LHCbApp().CondDBtag= "head-20111111"


from Configurables import RecMoniConf
RecMoniConf ().MoniSequence = ["RICH"]


from Configurables import HistogramPersistencySvc
HistogramPersistencySvc().OutputFile = "Histos-2011-Collisions.root"

from Configurables import RichRecSysConf
# disable PID
RichRecSysConf ("RichOfflineRec").photonConfig().SelectionMode = "ParticleSearch"
#RichRecSysConf ("RichOfflineRec").setProp("PidConfig", "None")

from Configurables import RichParticleSearchConf
PtConf = RichParticleSearchConf
PtConf.R2NTupleProduce = True
#PtConf.AerogelNTupleProduce = True
PtConf.NTupleProduce = True
PtConf.R1NTupleProduce = True
PtConf.Radiators = ["Aerogel","Rich1Gas","Rich2Gas"]
#PtConf.Histograms = "Online"
PtConf.MinIsolationCut = [100,50,50]
#PtConf.MinIsolationCut = [150,150,150]

#PtConf.MaxCK_Sig = [3.0,3.0,3.0]
PtConf.MaxCK_Sig = [3.0,3.0,3.0]

PtConf.CKDevCut = [0.2,0.1,0.5]
PtConf.MaxRichRecTracks = 200
PtConf.UseMuonInfo = True
PtConf.PlotPhotons = True


from Configurables import RichRecQCConf

#RichRecSysConf("RichOfflineRec").TracklessRingAlgs = ["Template"]
# disable monitors that are enabled by default in "OfflineFull" mode
# except "AlignmentMonitoring"
RichRecQCConf ("OfflineRichMoni").Histograms = "OfflineExpress"
RichRecQCConf ("OfflineRichMoni").removeMonitor("L1SizeMonitoring"       )
RichRecQCConf ("OfflineRichMoni").removeMonitor("DBConsistencyCheck"     )
RichRecQCConf ("OfflineRichMoni").removeMonitor("HotPixelFinder"         )
RichRecQCConf ("OfflineRichMoni").removeMonitor("PidMonitoring"          )
RichRecQCConf ("OfflineRichMoni").removeMonitor("PixelMonitoring"        )
RichRecQCConf ("OfflineRichMoni").removeMonitor("TrackMonitoring"        )
RichRecQCConf ("OfflineRichMoni").removeMonitor("PhotonMonitoring"       )
RichRecQCConf ("OfflineRichMoni").removeMonitor("TracklessRingAngles"    )
RichRecQCConf ("OfflineRichMoni").removeMonitor("TracklessRingPeakSearch")
RichRecQCConf ("OfflineRichMoni").removeMonitor("AlignmentMonitoring"    )
RichRecQCConf ("OfflineRichMoni").removeMonitor("HPDIFBMonitoring"       )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichPixelPositions"     )
RichRecQCConf ("OfflineRichMoni").removeMonitor("HPDHitPlots"            )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichTrackGeometry"      )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichGhostTracks"        )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichCKThetaResolution"  )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichTrackResolution"    )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichPhotonSignal"       )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichTrackCKResolutions" )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichPhotonGeometry"     )
RichRecQCConf ("OfflineRichMoni").removeMonitor("PhotonRecoEfficiency"   )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichPhotonTrajectory"   )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichStereoFitterTests"  )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichRayTracingTests"    )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichDataObjectChecks"   )
RichRecQCConf ("OfflineRichMoni").removeMonitor("RichRecoTiming"         )
RichRecQCConf ("OfflineRichMoni").removeMonitor("DataDecodingErrors"     )
RichRecQCConf ("OfflineRichMoni").removeMonitor("HPDImageShifts"         )
#RichRecQCConf ("OfflineRichMoni").removeMonitor("ParticleSearch"         )
#RichRecQCConf ("OfflineRichMoni").removeMonitor("HPDImageShifts"         )


NTupleSvc().Output = ["RICHTUPLE1 DATAFILE='Ntuple_2010-Collisions.root' TYP='ROOT' OPT='NEW'"]



