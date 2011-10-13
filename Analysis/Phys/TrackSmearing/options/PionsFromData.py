from Gaudi.Configuration import *

from PhysSelPython.Wrappers import Selection , SelectionSequence, DataOnDemand



Data = False

#################################################
from Configurables import CombineParticles, FilterDesktop


#DaVinci v28
#_pions = DataOnDemand(Location='Phys/StdLoosePions/Particles')
#DaVinci v26
#######################################
# pions
_pions = DataOnDemand(Location='Phys/StdLoosePions')
_filter = FilterDesktop("filterPions", Code = "TRCHI2DOF<4")
_selPions = Selection("SelFilterPions",
                            Algorithm = _filter,
                            RequiredSelections = [_pions])
_seqPions = SelectionSequence("SeqFilterPions", TopSelection = _selPions)

#######################################
# kaons
_kaons = DataOnDemand(Location='Phys/StdLooseKaons')
_filterkaons = FilterDesktop("filterKaons", Code = "TRCHI2DOF<4")
_selKaons = Selection("SelFilterKaons",
                            Algorithm = _filterkaons,
                            RequiredSelections = [_kaons])
_seqKaons = SelectionSequence("SeqFilterKaons", TopSelection = _selKaons)

_muons = DataOnDemand(Location='Phys/StdLooseMuons')
_filtermuons = FilterDesktop("filterMuons", Code = "TRCHI2DOF<4")
_selMuons = Selection("SelFilterMuons",
                            Algorithm = _filtermuons,
                            RequiredSelections = [_muons])
_seqMuons = SelectionSequence("SeqFilterMuons", TopSelection = _selMuons)


if not Data:
    #######################################
    # pions
    _pions2 = DataOnDemand(Location='Phys/SmrdLoosePions')
    _filter2 = FilterDesktop("filterPions2", Code = "TRCHI2DOF<4")
    _selPions2 = Selection("SelFilterPions2",
                            Algorithm = _filter2,
                            RequiredSelections = [_pions2])
    _seqPions2 = SelectionSequence("SeqFilterPions2", TopSelection = _selPions2)

    #######################################
    # kaons
    _kaons2 = DataOnDemand(Location='Phys/SmrdLooseKaons')
    _kaonsfilter2 = FilterDesktop("filterKaons2", Code = "TRCHI2DOF<4")
    _selKaons2 = Selection("SelFilterKaons2",
                            Algorithm = _kaonsfilter2,
                            RequiredSelections = [_kaons2])
    _seqKaons2 = SelectionSequence("SeqFilterKaons2", TopSelection = _selKaons2)

    #######################################
    # muons
    _muons2 = DataOnDemand(Location='Phys/SmrdLooseMuons')
    _filtermuons2 = FilterDesktop("filterMuons2", Code = "TRCHI2DOF<4")
    _selMuons2 = Selection("SelFilterMuons2",
                            Algorithm = _filtermuons,
                            RequiredSelections = [_muons])
    _seqMuons2 = SelectionSequence("SeqFilterMuons2", TopSelection = _selMuons2)


#######################################################################
#
# Decay Tree tuple
#

from Configurables import DecayTreeTuple
decaytuple = DecayTreeTuple("OriginalPionsTuple")
decaytuple.InputLocations = [_seqPions.outputLocation() ]
decaytuple.ToolList +=  [
    "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolTrackInfo"
    , "TupleToolPid"
   ]
decaytuple.Decay = "[pi+]cc"
from Configurables import TupleToolTrigger
decaytuple.addTool( TupleToolTrigger, name='TupleToolTrigger' )
decaytuple.TupleToolTrigger.Verbose = True
decaytuple.TupleToolTrigger.UseAutomaticTriggerList = False
decaytuple.TupleToolTrigger.TriggerList =[
"Hlt1MBNoBiasDecision",
"Hlt1MBMicroBiasRZVeloDecision",
"Hlt1MBMicroBiasTStationDecision",
]
decaytuple.ToolList += [ "TupleToolTrigger" ]



decaytuple4 = DecayTreeTuple("OriginalKaonsTuple")
decaytuple4.InputLocations = [_seqKaons.outputLocation() ]
decaytuple4.ToolList +=  [
    "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolTrackInfo"
    , "TupleToolPid"
   ]
decaytuple4.Decay = "[K-]cc"
from Configurables import TupleToolTrigger
decaytuple4.addTool( TupleToolTrigger, name='TupleToolTrigger' )
decaytuple4.TupleToolTrigger.Verbose = True
decaytuple4.TupleToolTrigger.UseAutomaticTriggerList = False
decaytuple4.TupleToolTrigger.TriggerList =[
"Hlt1MBNoBiasDecision",
"Hlt1MBMicroBiasRZVeloDecision",
"Hlt1MBMicroBiasTStationDecision",
]
decaytuple4.ToolList += [ "TupleToolTrigger" ]

decaytuple5 = DecayTreeTuple("OriginalMuonsTuple")
decaytuple5.InputLocations = [_seqMuons.outputLocation() ]
decaytuple5.ToolList +=  [
    "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolTrackInfo"
    , "TupleToolPid"
   ]
decaytuple5.Decay = "[mu-]cc"
from Configurables import TupleToolTrigger
decaytuple5.addTool( TupleToolTrigger, name='TupleToolTrigger' )
decaytuple5.TupleToolTrigger.Verbose = True
decaytuple5.TupleToolTrigger.UseAutomaticTriggerList = False
decaytuple5.TupleToolTrigger.TriggerList =[
"Hlt1MBNoBiasDecision",
"Hlt1MBMicroBiasRZVeloDecision",
"Hlt1MBMicroBiasTStationDecision",
]
decaytuple5.ToolList += [ "TupleToolTrigger" ]

if not Data:
    decaytuple2 = DecayTreeTuple("SmearedPionsTuple")
    decaytuple2.InputLocations = [_seqPions2.outputLocation() ]
    decaytuple2.ToolList +=  [
        "TupleToolGeometry"
        , "TupleToolKinematic"
        , "TupleToolTrackInfo"
        , "TupleToolPid"
       ]
    decaytuple2.Decay = "[pi+]cc"
    decaytuple2.addTool( TupleToolTrigger, name='TupleToolTrigger' )
    decaytuple2.TupleToolTrigger.Verbose = True
    decaytuple2.TupleToolTrigger.UseAutomaticTriggerList = False
    decaytuple2.TupleToolTrigger.TriggerList =[
        "Hlt1MBNoBiasDecision",
        "Hlt1MBMicroBiasRZVeloDecision",
        "Hlt1MBMicroBiasTStationDecision",
        ]
    decaytuple2.ToolList += [ "TupleToolTrigger" ]
    #decaytuple2.Branches = { "piplus2" : "[pi+]cc" }


    decaytuple3 = DecayTreeTuple("SmearedKaonsTuple")
    decaytuple3.InputLocations = [_seqKaons2.outputLocation() ]
    decaytuple3.ToolList +=  [
        "TupleToolGeometry"
        , "TupleToolKinematic"
        , "TupleToolTrackInfo"
        , "TupleToolPid"
       ]
    decaytuple3.Decay = "[K-]cc"
    decaytuple3.addTool( TupleToolTrigger, name='TupleToolTrigger' )
    decaytuple3.TupleToolTrigger.Verbose = True
    decaytuple3.TupleToolTrigger.UseAutomaticTriggerList = False
    decaytuple3.TupleToolTrigger.TriggerList =[
        "Hlt1MBNoBiasDecision",
        "Hlt1MBMicroBiasRZVeloDecision",
        "Hlt1MBMicroBiasTStationDecision",
        ]
    decaytuple3.ToolList += [ "TupleToolTrigger" ]

#from Configurables import TupleToolSmearedIP
#decaytuple.addTool( TupleToolSmearedIP, name = "TupleToolSmearedIP" )
#decaytuple.TupleToolSmearedIP.runOnData = Data
#decaytuple.TupleToolSmearedIP.myMessages = False
#decaytuple.TupleToolSmearedIP.veloResolution = 0.05
#decaytuple.ToolList += [ "TupleToolSmearedIP" ]


#######################################################################
#
# Configure the application
#

#remove the welcome message
from Configurables import LoKiSvc
LoKiSvc().Welcome = False

from Configurables import EventSelector
EventSelector().PrintFreq = 500


from Configurables import DaVinci

if Data: DaVinci().TupleFile = "DataPions.root"
else: DaVinci().TupleFile = "MCPions.root"
DaVinci().EvtMax = 1000
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().DataType = '2010'
DaVinci().Simulation = True
if Data: DaVinci().Simulation = False

# MC Options
if not Data:
    DaVinci().DDDBtag = 'head-20101206'
    DaVinci().CondDBtag = "sim-20101210-vc-md100"


if not Data:
    #importOptions("$User_release_area/DaVinci_v26r3p2/TrackSmearing/TrackSmearing/options/MakeMyParticles.py")
    #from MakeMyParticles import *
    #makeMyProtoP("Best")
    #makeMyParticles("SmrdLoosePions", "Best", "pion")
    #makeMyKaons("SmrdLooseKaons", "Best", "kaon")
    #makeAllParticles("SmrdNoPIDPions", "Best", "pion")
    #makeAllParticles("SmrdNoPIDKaons", "Best", "kaon")
    from Configurables import TrackSmeared
    TrackSmeared("TrackSmearing").InputLocation = "Rec/Track/Best"
    #TrackSmeared("TrackSmearing").OutputLocation = "Best"
    TrackSmeared("TrackSmearing").smear = True
    TrackSmeared("TrackSmearing").makePlots = True
    TrackSmeared("TrackSmearing").OutputLevel = 3
    TrackSmeared("TrackSmearing").smearBest = True
    TrackSmeared("TrackSmearing").smearProto = True
    TrackSmearingSeq = GaudiSequencer("TrackSmearingSeq")
    TrackSmearingSeq.Members = [ TrackSmeared("TrackSmearing") ]
    




DaVinci().UserAlgorithms += [ TrackSmearingSeq,
    _seqPions.sequence()
    ,decaytuple
    ,_seqKaons.sequence()
    ,decaytuple4
    ,_seqMuons.sequence()
    ,decaytuple5
    ]

#if not Data:
#    DaVinci().UserAlgorithms += [ 
#        _seqPions2.sequence()
#        ,decaytuple2
#        ,_seqKaons2.sequence()
#        ,decaytuple3
#        ,_seqMuons2.sequence()
#        ]
    

DaVinci().MainOptions  = ""




if Data: #MiniBias DATA
    EventSelector().Input = [
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000094_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000697_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'"
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000528_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000390_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000005_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000664_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000084_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000067_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000535_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000390_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000546_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000382_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000613_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000418_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000026_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000029_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000597_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000606_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000105_1.minibias.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
] 


else: #MiniBias MC
    EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000025_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000471_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000013_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000402_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000002_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000009_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000312_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000353_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000029_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000217_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
] 

# MC MAG DOWN
# '/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000025_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000471_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000013_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000402_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000002_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000009_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000312_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000353_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00008895/0000/00008895_00000029_1.allstreams.dst' 
#, '/lhcb/MC/MC10/ALLSTREAMS.DST/00009368/0000/00009368_00000217_1.allstreams.ds

# DATA MAG DOWN
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000094_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000697_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000528_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000259_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000005_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000664_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000220_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000084_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000067_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000535_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000390_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000546_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000382_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000613_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000418_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000026_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000029_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000597_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000606_1.minibias.dst
#/lhcb/data/2010/MINIBIAS.DST/00008378/0000/00008378_00000105_1.minibias.dst


#DATA MAG UP
#['/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000747_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000704_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000484_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000140_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000089_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000701_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000614_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000202_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000925_1.minibias.dst' , '/lhcb/data/2010/MINIBIAS.DST/00008392/0000/00008392_00000568_1.minibias.dst' ]

#MC MAG UP
#['/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000418_1.allstreams.dst', '/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000243_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000146_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000138_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00008545/0000/00008545_00000023_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00008545/0000/00008545_00000029_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000235_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000364_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000129_1.allstreams.dst' ,'/lhcb/MC/MC10/ALLSTREAMS.DST/00009365/0000/00009365_00000414_1.allstreams.dst' ]


