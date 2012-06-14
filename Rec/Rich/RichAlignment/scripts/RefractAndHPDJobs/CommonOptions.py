from Brunel.Configuration import *
from Configurables import Brunel, LHCbApp, CondDB

# For L0 filter. To be removed when part of Brunel
importOptions("$L0TCK/L0DUConfig.opts")

# For 2009 data
#importOptions("$APPCONFIGOPTS/Brunel/DataType-2009.py")
#LHCbApp().DDDBtag   = "head-20110303"
#LHCbApp().CondDBtag = "head-20110622"

# For 2010 data
#importOptions("$APPCONFIGOPTS/Brunel/DataType-2010.py")
#LHCbApp().DDDBtag   = "head-20110303"
#LHCbApp().CondDBtag = "head-20110622"

# For 2011 data
#importOptions("$APPCONFIGOPTS/Brunel/DataType-2011.py")
#LHCbApp().DDDBtag   = "head-20110914"
#LHCbApp().CondDBtag = "head-20111111"

# For 2012 data
importOptions("$APPCONFIGOPTS/Brunel/DataType-2012.py")
LHCbApp().DDDBtag   = "head-20120413"
LHCbApp().CondDBtag = "head-20120420"

# Aerogel Sub Tiles
#CondDB().LocalTags["LHCBCOND"] = ["rich1-20110624"]
#CondDB().LocalTags["DDDB"]     = ["rich1-20110624"]

# Initial IOV time
# http://www.onlineconversion.com/unix_time.htm
#from Configurables import EventClockSvc
#EventClockSvc( InitialTime = 1306879200000000000 ) # 1st June 2011
#EventClockSvc( InitialTime = 1317460149000000000 ) # 1st Octo 2011
#EventClockSvc( InitialTime = 1319155200000000000 ) # 21st Octo 2011

# No output files
Brunel().OutputType = "None"

# Only tracking and RICH reco
Brunel().RecoSequence = ["Decoding","VELO","TT","IT","OT","Tr","Vertex","RICH"]

from Configurables import RichRecSysConf
rConf = RichRecSysConf("RichOfflineRec")

# Which radiators
#rConf.Radiators = [ "Rich1Gas","Rich2Gas" ]

# Photon selection options
rConf.photonConfig().SelectionMode = "Wide"

# No PID or ring finding, for speed
rConf.TracklessRingAlgs = []
rConf.PidConfig = "None"

# Only RICH Monitoring
from Configurables import RecMoniConf
RecMoniConf().MoniSequence = ["RICH"]

# Tweek the RICH Monitoring histograms
from Configurables import RichRecQCConf
rMoni = RichRecQCConf("OfflineRichMoni")
rMoni.removeMonitors ( ["HPDIFBMonitoring","PidMonitoring","TracklessRingAngles"] )
# Open up the CK res plot range, for the Wide photon selection
#rMoni.CKThetaResRange = [ 0.045, 0.008, 0.004 ]
rMoni.CKThetaResRange = [ 0.05, 0.008, 0.004 ]
# Add HPD occ plots
rMoni.addMonitor("HPDHitPlots")

# Alignment histogram options
from Configurables import RichAlignmentConf
aConf = RichAlignmentConf("OfflineRichMoni_RichAlignmentConf")
aConf.Histograms = "Expert"
aConf.HPDList = [ [], [0], [0] ]

# Save all histograms from image analysis
rMoni.addMonitor("HPDImageShifts")
from Configurables import Rich__HPDImage__Summary
imageSummary = Rich__HPDImage__Summary("RichHPDImageSummary")
imageSummary.Keep2DHistograms = True
imageSummary.FinalHPDFit = False

# Enable extra histos for the Long Track resolution monitors
from Configurables import Rich__Rec__MC__RecoQC
long = Rich__Rec__MC__RecoQC("RiCKResLong")
long.EnablePerPDPlots    = True
long.EnablePerPDColPlots = True
longT = Rich__Rec__MC__RecoQC("RiCKResLongTight")
longT.EnablePerPDPlots    = True
longT.EnablePerPDColPlots = True

# Flip the panels for background shape studies
#rConf.richTools().PixelCreatorType = "RawBufferWithPanelFlip"

# Don't reject HLT error events
#Brunel().VetoHltErrorEvents = False 
