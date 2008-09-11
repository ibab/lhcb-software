from RichRecSys.Configuration import *
from GaudiKernel.SystemOfUnits import GeV, mm

seq = GaudiSequencer("HltRICHReco")
seq.MeasureTime = True
GaudiSequencer("SeqHlt2Charged").Members += [seq]

richConf = RichRecSysConf()

richConf.context   = "HLT"
richConf.pidConfig = "FastGlobal"

richConf.initPixels  = True
richConf.initTracks  = True
richConf.initPhotons = True

RichPixelCreatorConfig().bookKeeping  = False
RichPixelCreatorConfig().findClusters = False

#RichTrackCreatorConfig().minPtot = 2
#RichTrackCreatorConfig().zTolerances = [ 3000*mm,   3000*mm,  4000*mm ]
#RichTrackCreatorConfig().primaryExtrapolator = "TrackFastParabolicExtrapolator"
#RichTrackCreatorConfig().backupExtrapolator  = "TrackLinearExtrapolator"
#RichTrackCreatorConfig().trackAlgs = [ "Forward" ]

#RichPhotonCreatorConfig().photonPredictor = "CKthetaBands"
#CKthetaBandsPhotonPredictor().nSigma = [ 4.5, 4.5, 10.5 ]
#RichPhotonCreatorConfig().photonReco = "EstiFromRadius"

#PhotonCreator().nSigma = [ 3.5, 2.8, 3.0 ]

richConf.applyConf(seq)

del richConf
