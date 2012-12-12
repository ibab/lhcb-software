__author__ = ['Marco Gersabeck']
__date__ = '03/11/2009'
__version__ = '$Revision: 1.1 $'

__all__ = ('name', 'D0', 'sequence')

################
# Configure the selection
################
from Gaudi.Configuration import *

from Configurables import FilterDesktop
from Configurables import CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

## More space for output
MessageSvc().Format = "% F%30W%S%15W%R%T %0W%M"

## mDST paths
locationRoot1 = '/Event'
D0path = 'CharmCompleteEvent/Phys/D2KSPPLLForDstarD2KShh'
D0particlePath = D0path + '/Particles'

Dstpath = 'CharmCompleteEvent/Phys/DstarD2KShhPPLLLine'
DstparticlePath = Dstpath + '/Particles'

p2PVLocation = D0path + '/Particle2VertexRelations'

mDST = True
if mDST:
    mDSTName = 'testSwimStrip'
    p2PVLocation = '%s/BestPV_%s_P2PV' % ( D0path, mDSTName )
    from MicroDSTConf.TriggerConfUtils import configureL0AndHltDecoding
    locationRoot1 += '/' + mDSTName
    configureL0AndHltDecoding(locationRoot1)
    
################
# Configure DecayTreeTuple
################
    
l0List = ['Muon', 'Hadron', 'Electron', 'Photon']
hlt1List = ['Hlt1TrackAllL0',]
hlt2List = ['Hlt2CharmHadD02HHKsLL' ]#, 'Hlt2CharmHadD02HHKsDD']

triggerList = []
for trigger in l0List:
    triggerList.append( 'L0' + trigger + 'Decision')
    for trigger in hlt1List:
        triggerList.append( trigger + 'Decision')
        for trigger in hlt2List:
            triggerList.append( trigger + 'Decision')
            
from Configurables import DecayTreeTuple, TupleToolMCTruth, TupleToolTrigger, TupleToolTISTOS, TupleToolPropertime, PropertimeFitter, TupleToolSwimmingInfo
            
def makeDecayTreeTuple( DstparticlePath, Dstpath, particlePath, D0path, p2PVLocation ):
    print particlePath, D0path, p2PVLocation
    tuple = DecayTreeTuple (
        'SwimmingDTT',
        ## print histos
        HistoPrint = True,
        ## N-tuple LUN
        ## input particles from selection:
        Inputs = [ DstparticlePath ],
        ## Primary vertices from mDST
        P2PVInputLocations = [ p2PVLocation ],
        UseP2PVRelations = True,
        WriteP2PVRelations = False,
        )
    
    tuple.Decay = "[D*(2010)+ -> (^D0 -> (^KS0 => ^pi+ ^pi-) ^pi- ^pi+) ^pi+]cc"
    if mDST: tuple.RootInTES = locationRoot1
    
    tuple.ToolList = [ "TupleToolKinematic",
                       "TupleToolGeometry", "TupleToolEventInfo",
                       "TupleToolPrimaries", "TupleToolTISTOS",
                       "TupleToolPid", "TupleToolTrackInfo",
                       'TupleToolPropertime', 'TupleToolTrigger']
    
    from Configurables import TupleToolDecay
    tuple.addTool(TupleToolDecay, name = 'D0')
    tuple.addTool(TupleToolDecay, name = 'D0dp')
    tuple.addTool(TupleToolDecay, name = 'D0dm')
    tuple.addTool(TupleToolDecay, name = 'KS0dp')
    tuple.addTool(TupleToolDecay, name = 'KS0dm')

    tuple.Branches = {"D0"    : "[D*(2010)+ -> (^D0 -> (KS0 => pi+ pi-) pi+ pi-) pi+]cc",
                      "KS0dp"  : "[D*(2010)+ -> (D0 -> (KS0 => ^pi+ pi-) pi+ pi-) pi+]cc",
                      "KS0dm"  : "[D*(2010)+ -> (D0 -> (KS0 => pi+ ^pi-) pi+ pi-) pi+]cc",
                      "D0dp" : "[D*(2010)+ -> (D0 -> (KS0 => pi+ pi-) ^pi+ pi-) pi+]cc",
                      "D0dm" : "[D*(2010)+ -> (D0 -> (KS0 => pi+ pi-) pi+ ^pi-) pi+]cc",
                      }
    tuple.D0.ToolList = ['TupleToolSwimmingInfo/TriggerInfo', 'TupleToolSwimmingInfo/StrippingInfo']
    tuple.D0dm.ToolList = ['TupleToolSwimmingInfo/TriggerInfo', 'TupleToolSwimmingInfo/StrippingInfo']
    tuple.D0dp.ToolList = ['TupleToolSwimmingInfo/TriggerInfo', 'TupleToolSwimmingInfo/StrippingInfo']
    tuple.KS0dm.ToolList = ['TupleToolSwimmingInfo/TriggerInfo', 'TupleToolSwimmingInfo/StrippingInfo']
    tuple.KS0dp.ToolList = ['TupleToolSwimmingInfo/TriggerInfo', 'TupleToolSwimmingInfo/StrippingInfo']
    
    tuple.D0dp.addTool(TupleToolSwimmingInfo('TriggerInfo'))
    tuple.D0dp.addTool(TupleToolSwimmingInfo('StrippingInfo'))
    tuple.D0dp.TriggerInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.D0dp.StrippingInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.D0dp.StrippingInfo.ReportStage = "Stripping"

    tuple.D0dm.addTool(TupleToolSwimmingInfo('TriggerInfo'))
    tuple.D0dm.addTool(TupleToolSwimmingInfo('StrippingInfo'))
    tuple.D0dm.TriggerInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.D0dm.StrippingInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.D0dm.StrippingInfo.ReportStage = "Stripping"
    
    tuple.KS0dp.addTool(TupleToolSwimmingInfo('TriggerInfo'))
    tuple.KS0dp.addTool(TupleToolSwimmingInfo('StrippingInfo'))
    tuple.KS0dp.TriggerInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.KS0dp.StrippingInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.KS0dp.StrippingInfo.ReportStage = "Stripping"
    
    tuple.KS0dm.addTool(TupleToolSwimmingInfo('TriggerInfo'))
    tuple.KS0dm.addTool(TupleToolSwimmingInfo('StrippingInfo'))
    tuple.KS0dm.TriggerInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.KS0dm.StrippingInfo.ReportsLocation = D0path + '/P2TPRelations'
    tuple.KS0dm.StrippingInfo.ReportStage = "Stripping"   

    tuple.D0.addTool(TupleToolSwimmingInfo('TriggerInfo'))
    tuple.D0.TriggerInfo.ReportsLocation =  D0path + '/P2TPRelations'
    tuple.D0.TriggerInfo.UseExtraLocation = True
    tuple.D0.TriggerInfo.ExtraLocation = particlePath
    tuple.D0.addTool(TupleToolSwimmingInfo('StrippingInfo'))
    tuple.D0.StrippingInfo.ReportsLocation =  D0path + '/P2TPRelations'
    tuple.D0.StrippingInfo.UseExtraLocation = True
    tuple.D0.StrippingInfo.ExtraLocation = particlePath
    tuple.D0.StrippingInfo.ReportStage = "Stripping"
    
    tuple.TupleName = "DecayTree"
    
    tuple.addTool( TupleToolPropertime() )
    tuple.TupleToolPropertime.FitToPV = True
    
    #from Configurables import L0TriggerTisTos, TriggerTisTos
    tuple.addTool( TupleToolTISTOS() )
    tuple.TupleToolTISTOS.VerboseL0 = True
    tuple.TupleToolTISTOS.VerboseHlt1 = True
    tuple.TupleToolTISTOS.VerboseHlt2 = True
    tuple.TupleToolTISTOS.Verbose = True
    tuple.TupleToolTISTOS.TriggerList = triggerList
    
    return tuple

from Configurables import DaVinci, L0Conf

dv = DaVinci()
dv.DataType = '2011'
dv.EvtMax = -1
tuple = makeDecayTreeTuple( DstparticlePath, Dstpath, D0particlePath, D0path, p2PVLocation )
dv.UserAlgorithms += [ tuple ]
dv.Simulation   = False
DaVinci().DDDBtag = "head-20110914"
DaVinci().CondDBtag = "head-20111111"
dv.Lumi = False
dv.HistogramFile = "postfix_histos.root"    # Histogram file
dv.TupleFile = "postfix_ntuple.root"       # Ntuple

#dv.EnableUnpack = ['Reconstruction', 'Stripping']

from Configurables import Gaudi__IODataManager as IODataManager
IODataManager().UseGFAL=False

from Configurables import EventClockSvc
EventClockSvc( InitialTime = 1322701200000000000 )

from GaudiConf import IOHelper
IOHelper().inputFiles(['/afs/cern.ch/work/n/ntorr/public/testDaVinci/Swimming.testSwimStrip.mdst']
                      , clear=True)


