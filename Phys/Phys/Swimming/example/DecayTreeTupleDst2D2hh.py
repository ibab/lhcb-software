##
## Static Configuration 
##
from Gaudi.Configuration import *
from Configurables import ( ApplicationMgr,
                            CondDB,
                            HistogramPersistencySvc,
                            DaVinci,
                            MessageSvc,
                            GaudiSequencer,
                            ANNDispatchSvc,
                            HltDecReportsDecoder,
                            HltSelReportsDecoder )

CondDB( UseOracle = False,DisableLFC=True )

## More space for output
MessageSvc().Format = "% F%30W%S%15W%R%T %0W%M"

## mDST paths
locationRoot = '/Event'
selectionPath = 'CharmCompleteEvent/Phys/D2hhPromptDst2D2RSLine'
D0path = 'CharmCompleteEvent/Phys/D2hhPromptD2KPiLine'
particlePath = selectionPath + '/Particles'
#pvLocation = 'Rec/Vertex/Primary'

p2PVLocation = selectionPath +'/Particle2VertexRelations'

mDST = True
if mDST:
    mDSTName = 'SwimmingMDST'
    p2PVLocation = '%s/BestPV_%s_P2PV' % ( selectionPath, mDSTName )
    from MicroDSTConf.TriggerConfUtils import configureL0AndHltDecoding
    locationRoot += '/' + mDSTName
    configureL0AndHltDecoding(locationRoot)

from Gaudi.Configuration import *
from Configurables import ( DaVinci, MessageSvc, FilterDesktop )

# Make the DecayTreeTuple
from Configurables import DecayTreeTuple
dtt = DecayTreeTuple (
    'SwimmingDTT',
    ## print histos 
    HistoPrint = True,
    ## N-tuple LUN 
    NTupleLUN  = "DTT",
    ## input particles from selection:
    Inputs = [ particlePath ],
    ## Primary vertices from mDST
    P2PVInputLocations = [ p2PVLocation ],
    UseP2PVRelations = True,
    WriteP2PVRelations = False,
    )

dtt.Decay = "[D*(2010)+ -> (^D0 -> ^K- ^pi+) ^pi+]cc"
if mDST: dtt.RootInTES = locationRoot

from DecayTreeTuple.Configuration import *
## Add appropriate tools

from Configurables import TupleToolDecay
dtt.addTool(TupleToolDecay, name = 'D0')
dtt.Branches = {
    "D0" : "[D*(2010)+ -> (^D0 -> K- pi+) pi+]cc"
}

ttptime = dtt.D0.addTupleTool('TupleToolPropertime')
ttptime.FitToPV = True
ttsi = dtt.D0.addTupleTool('TupleToolSwimmingInfo/TriggerInfo')
ttsis = dtt.D0.addTupleTool('TupleToolSwimmingInfo/StrippingInfo')
ttsi.ReportsLocation =  D0path + '/P2TPRelations'
ttsis.ReportsLocation =  D0path + '/P2TPRelations'
ttsis.ReportStage = "Stripping"
ttsi.OutputLevel = 1
ttsis.OutputLevel = 1 

tttt = dtt.D0.addTupleTool('TupleToolTISTOS')
tttt.TriggerList = ['Hlt1TrackAllL0Decision', 'Hlt1TrackMuonDecision', 'Hlt1DiMuonHighMassDecision',
                    'Hlt2DiMuonDetachedJpsiDecision', 'Hlt2DiMuonJpsiDecision']
tttt.VerboseHlt1 = True
tttt.VerboseHlt2 = True

dv = DaVinci()
dv.DDDBtag = 'head-20110914'
dv.CondDBtag = 'head-20110914'
dv.DataType = '2011'
dv.Lumi = False
dv.InputType = "MDST" if mDST else "DST"
dv.UserAlgorithms = [ dtt ]
dv.EvtMax = -1

ApplicationMgr().HistogramPersistency = "ROOT"
from Configurables import HistogramPersistencySvc
HistogramPersistencySvc ( OutputFile = 'histos.root' ) 

from Configurables import NTupleSvc
NTupleSvc().Output += [ "DTT DATAFILE='tuples.root' TYPE='ROOT' OPT='NEW'"]
NTupleSvc().OutputLevel = 1

## Point the EventClockSvc to the RootInTES
## from Configurables import EventClockSvc, OdinTimeDecoder, TimeDecoderList
## EventClockSvc().addTool( TimeDecoderList, name = "EventTimeDecoder" )
## EventClockSvc().EventTimeDecoder.RootInTES = locationRoot

from GaudiConf import IOHelper
if mDST:
    ## IOHelper().inputFiles(['/castor/cern.ch/user/r/raaij/test/Swimming.SwimmingMicroDST.mdst'])
    IOHelper().inputFiles(['Swimming.SwimmingMDSTTagged.mdst'])
else:
    IOHelper().inputFiles(['/castor/cern.ch/user/r/raaij/test/SwimTrigDST.dst'])
