from RichRecSys.Configuration import *
from GaudiKernel.SystemOfUnits import GeV, mm

richseq = GaudiSequencer("HltRICHReco")
richseq.MeasureTime = True

# The RICH COnfigurable
richConf = RichRecSysConf()

# Set the sequence to run the RICH PID in
richConf.setProp("RecoSequencer",richseq)

# Configure for Fast HLT Global PID
richConf.Context   = "HLT"
richConf.PidConfig = "FastGlobal"

# Initial pixels tracks and photons
richConf.InitPixels  = True
richConf.InitTracks  = True
richConf.InitPhotons = True

# Turn off ring finding
richConf.TracklessRingAlgs = []

# Turn off book keeping for pixels, for speed
RichPixelCreatorConfig().BookKeeping  = False
RichPixelCreatorConfig().FindClusters = False
