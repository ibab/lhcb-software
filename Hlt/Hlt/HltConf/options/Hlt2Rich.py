from RichRecSys.Configuration import *
from GaudiKernel.SystemOfUnits import GeV, mm

seq = GaudiSequencer("HltRICHReco")
seq.MeasureTime = True
#GaudiSequencer("SeqHlt2Charged").Members += [seq]

richConf = RichRecSysConf()

richConf.setProp("RecoSequencer",seq)

richConf.Context   = "HLT"
richConf.PidConfig = "FastGlobal"

richConf.InitPixels  = True
richConf.InitTracks  = True
richConf.InitPhotons = True

RichPixelCreatorConfig().BookKeeping  = False
RichPixelCreatorConfig().FindClusters = False

#richConf.applyConf()
