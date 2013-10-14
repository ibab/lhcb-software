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

from MicroDSTConf.TriggerConfUtils import configureL0AndHltDecoding
configureL0AndHltDecoding('/Event/SwimStrippingD2hhMDST_v14r8/CharmToBeSwum')

## More space for output
MessageSvc().Format = "% F%30W%S%15W%R%T %0W%M"

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
    Inputs = [ '/Event/SwimStrippingD2hhMDST_v14r8/CharmToBeSwum/Phys/D2hhPromptD2KPiLine/Particles' ],
    ## Primary vertices from mDST
    P2PVInputLocations = [ '/Event/SwimStrippingD2hhMDST_v14r8/CharmToBeSwum/Phys/D2hhPromptD2KPiLine/Particle2VertexRelations'],
    UseP2PVRelations = True,
    WriteP2PVRelations = False,
    Decay = "[[D0]cc -> ^K- ^pi+]cc"
    )

from DecayTreeTuple.Configuration import *
## Add appropriate tools
from Configurables import EventNodeKiller
killer=EventNodeKiller( Nodes = ["/Event/DAQ","/Event/pRec"] ) 

dtt.addBranches({
    "D" : "[D0]cc : [[D0]cc -> K- pi+]cc"
})

dtt.D.addTupleTool('TupleToolPropertime')
ttsi = dtt.D.addTupleTool('TupleToolSwimmingInfo/TriggerInfo')
ttsis = dtt.D.addTupleTool('TupleToolSwimmingInfo/StrippingInfo')
ttsi.ReportsLocation = '/Event/SwimStrippingD2hhMDST_v14r8/CharmToBeSwum/Phys/D2hhPromptD2KPiLine/P2TPRelations'
ttsis.ReportsLocation = '/Event/SwimStrippingD2hhMDST_v14r8/CharmToBeSwum/Phys/D2hhPromptD2KPiLine/P2TPRelations'
ttsis.ReportStage = "Stripping"

tttt = dtt.D.addTupleTool('TupleToolTISTOS')
tttt.TriggerList = ['Hlt1TrackAllL0Decision', 'Hlt2CharmHadD02HH_D02KPiDecision']
tttt.VerboseHlt1 = True
tttt.VerboseHlt2 = True

dv = DaVinci()
dv.DDDBtag = 'dddb-20120831'
dv.CondDBtag = 'cond-20120831'
dv.DataType = '2012'
dv.Lumi = False
dv.InputType = "MDST" 
dv.UserAlgorithms = [ killer,dtt ]
dv.EvtMax = -1
dv.EnableUnpack = ['Stripping','Reconstruction']
# Where to save the output:
dv.HistogramFile = "histos.root"
dv.Simulation = False

from Configurables import NTupleSvc
NTupleSvc().Output += [ "DTT DATAFILE='SwimD2hhTuple_v14r8.root' TYPE='ROOT' OPT='NEW'"]
NTupleSvc().OutputLevel = 1 

from GaudiConf import IOHelper
IOHelper().inputFiles(['Swimming.SwimStrippingD2hhMDST_v14r8.mdst'])

# Use the CondDB to get the right database tags 
from Configurables import CondDB
CondDB(UseOracle = False, DisableLFC = True)
