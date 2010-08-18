
from Brunel.Configuration import *
from Configurables import Brunel, LHCbApp

importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")
importOptions("$APPCONFIGOPTS/Brunel/DataType-2010.py")
importOptions("$APPCONFIGOPTS/UseOracle.py")

LHCbApp().DDDBtag   = "head-20100518"
LHCbApp().CondDBtag = "head-20100730"

Brunel().OutputType = "None"

Brunel().PrintFreq = 100

# Only tracking and RICH reco
Brunel().RecoSequence = ["Decoding","VELO","TT","IT","OT","Tr","Vertex","RICH"]

from Configurables import RichRecSysConf
rConf = RichRecSysConf("RichOfflineRec")

# Disable Aerogel (Not being calibrated)
rConf.Radiators = [ "Rich1Gas","Rich2Gas" ]

# No PID or ring finding
rConf.TracklessRingAlgs = []
rConf.PidConfig = "None"

# Only RICH Monitoring
from Configurables import RecMoniConf
RecMoniConf().MoniSequence = ["RICH"]

# Tweek the RICH Monitoring
from Configurables import RichRecQCConf
rMoni = RichRecQCConf("OfflineRichMoni")
rMoni.removeMonitors ( ["HPDIFBMonitoring","PidMonitoring","TracklessRingAngles"] )
from Configurables import RichAlignmentConf
conf = RichAlignmentConf("OfflineRichMoni_RichAlignmentConf")
conf.Histograms = "Expert"
