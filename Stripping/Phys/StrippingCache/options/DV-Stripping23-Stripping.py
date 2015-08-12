# $Id: $
# Test your line(s) of the stripping by taking the dictionaries from StrippingSettings
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
#Raw event juggler to split Other/RawEvent into Velo/RawEvent and Tracker/RawEvent
#
from Configurables import RawEventJuggler
juggler = RawEventJuggler( DataOnDemand=True, Input=0.3, Output=4.1 )

#
#Fix for TrackEff lines
#
#from Configurables import DecodeRawEvent
#DecodeRawEvent().setProp("OverrideInputs",4.1)

#from Configurables import ConfigCDBAccessSvc
#confDBAcc = ConfigCDBAccessSvc()
#confDBAcc.File = '/afs/cern.ch/user/s/sperazzi/public/Stripping/S23/DaVinciDev_v37r0/Phys/StrippingSelections/tests/data/config.cdb'

# Specify the name of your configuration
my_wg='Charm' #FOR LIAISONS
stripping='stripping23'


# NOTE: this will work only if you inserted correctly the 
# default_config dictionary in the code where your LineBuilder 
# is defined.
from StrippingSettings.Utils import strippingConfiguration
from StrippingSelections import buildersConf
conf=strippingConfiguration(stripping)
from StrippingSelections.Utils import lineBuilder, buildStreams
streams = buildStreams( conf )

leptonicMicroDSTname   = 'Leptonic'
charmMicroDSTname      = 'Charm'
pidMicroDSTname        = 'PID'
bhadronMicroDSTname    = 'Bhadron'
mdstStreams = [ leptonicMicroDSTname,charmMicroDSTname,pidMicroDSTname,bhadronMicroDSTname ]
dstStreams  = [ "BhadronCompleteEvent", "CharmCompleteEvent", "CharmToBeSwum", "Dimuon",
                "EW", "Semileptonic", "Calibration", "MiniBias", "Radiative" ]

stripTESPrefix = 'Strip'

from Configurables import ProcStatusCheck

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = ProcStatusCheck(),
                    TESPrefix = stripTESPrefix,
                    ActiveMDSTStream = True,
                    Verbose = True,
                    DSTStreams = dstStreams,
                    MicroDSTStreams = mdstStreams )

## Configure PV refitter
from GaudiKernel.SystemOfUnits import micrometer
from Configurables import LoKi__PVReFitter
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").CheckTracksByLHCbIDs = True
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").DeltaChi2 = 0.01
LoKi__PVReFitter("ToolSvc.LoKi::PVReFitter").DeltaDistance = 5*micrometer

## Configure the VeloTrack unpacker
from Configurables import UnpackTrack
unpackIt = UnpackTrack("unpackIt")
unpackIt.InputName = "pRec/Track/FittedHLT1VeloTracks"
unpackIt.OutputName = "Rec/Track/FittedHLT1VeloTracks"

from Configurables import ApplicationMgr, AuditorSvc, SequencerTimerTool

#from Configurables import COOLConfSvc, MagneticFieldSvc, ToolSvc, AuditorSvc, CondDBCnvSvc
#coolConfSvc = COOLConfSvc()
#magneticSvc = MagneticFieldSvc()
#toolSvc = ToolSvc()
#auditor = AuditorSvc()

# Initial IOV time
# http://www.onlineconversion.com/unix_time.htm
# values in ns (so multiply values from above link by 1e9)
#from Configurables import EventClockSvc
#EventClockSvc( InitialTime = 1433635200000000000 ) # 7th June 2015

appMgr = ApplicationMgr()
appMgr.OutputLevel = 3
#appMgr.ExtSvc += [ coolConfSvc, magneticSvc, confDBAcc, toolSvc, auditor ]
#appMgr.ExtSvc += [ CondDBCnvSvc(), confDBAcc, magneticSvc, toolSvc, auditor ]
#appMgr.ExtSvc += [ confDBAcc ]

appMgr.HistogramPersistency = "ROOT"
ntSvc = NTupleSvc()
appMgr.ExtSvc +=  [ ntSvc ]

from Configurables import LHCbApp, PhysConf

datatype =  "2015"
LHCbApp().DataType = datatype
PhysConf().DataType = datatype

LHCbApp().DDDBtag  = "dddb-20150526"
LHCbApp().CondDBtag = "cond-20150625"

from Configurables import DaVinci
DaVinci().ProductionType = "Stripping"
DaVinci().DataType   = datatype
DaVinci().DDDBtag    = LHCbApp().DDDBtag
DaVinci().CondDBtag  = LHCbApp().CondDBtag

#appMgr.TopAlg += [ unpackIt, sc.sequence() ]
DaVinci().appendToMainSequence( [ unpackIt, sc.sequence() ] )
