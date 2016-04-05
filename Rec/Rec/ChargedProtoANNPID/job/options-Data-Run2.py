
from Gaudi.Configuration import *
from Configurables import ( DaVinci, GaudiSequencer )

#from Configurables import ( ANNGlobalPID__ChargedProtoANNPIDTrainingTuple,
#                            ANNGlobalPID__ChargedProtoANNPIDTupleTool )
#pidtuple = ANNGlobalPID__ChargedProtoANNPIDTrainingTuple("ANNPID")
#pidtuple.NTupleLUN = "ANNPIDTUPLE"
##pidtuple.addTool( ANNGlobalPID__ChargedProtoANNPIDTupleTool, name = "Tuple" )
##pidtuple.Tuple.NTupleLUN = "ANNPIDTUPLE"
#DaVinci().UserAlgorithms += [ pidtuple ]

from DecayTreeTuple.Configuration import *
from Configurables import ( DecayTreeTuple, LoKi__Hybrid__TupleTool,
                            TupleToolTrigger, TupleToolStripping )
tuple = DecayTreeTuple("ANNPID")
tuple.Decay    = "[pi+]cc"
tuple.NTupleLUN = "ANNPIDTUPLE"
tuple.Inputs = [ 'Phys/StdAllNoPIDsPions/Particles',
                 'Phys/StdNoPIDsUpPions/Particles',
                 'Phys/StdNoPIDsDownPions/Particles' ]
tuple.ToolList = [ "TupleToolANNPID", "TupleToolANNPIDTraining", "TupleToolGeometry" ]
stripTool = tuple.addTupleTool( TupleToolStripping )
stripTool.TriggerList = [
    "StrippingB02DKD2HHHBeauty2CharmLineDecision",   
    "StrippingB02DKD2Pi0HHHMergedBeauty2CharmLineDecision",   
    "StrippingB02DKD2Pi0HHHResolvedBeauty2CharmLineDecision",   
    "StrippingB02DKPiPiD2HHHPIDBeauty2CharmLineDecision",   
    "StrippingB02DPiD2HHHBeauty2CharmLineDecision",   
    "StrippingB02DPiD2Pi0HHHMergedBeauty2CharmLineDecision",   
    "StrippingB02DPiD2Pi0HHHResolvedBeauty2CharmLineDecision",   
    "StrippingB02DPiNoIPD2HHHCFPIDBeauty2CharmLineDecision",   
    "StrippingB02DPiNoIPDs2HHHPIDBeauty2CharmLineDecision",   
    "StrippingB02DPiPiPiD2HHHPIDBeauty2CharmLineDecision",
    "StrippingB02DPiWSD2HHHBeauty2CharmLineDecision",
    "StrippingB0d2DTauNuForB2XTauNuAllLinesDecision",
    "StrippingB0d2DTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingB0d2DTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingB0d2DdoubleStarTauNuForB2XTauNuAllLinesDecision",
    "StrippingB0d2DdoubleStarTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingB0d2DdoubleStarTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuInvVertForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingB0d2DstarTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingB2CharmlessQ2B3piSelectionLineDecision",
    "StrippingB2CharmlessQ2B4piSelectionLineDecision",
    "StrippingB2D0D0KD02HHD02HHBeauty2CharmLineDecision",
    "StrippingB2D0KD2HHBeauty2CharmLineDecision",
    "StrippingB2D0PiD2HHBeauty2CharmLineDecision",
    "StrippingB2D0PiD2HHHHTIGHTBeauty2CharmLineDecision",
    "StrippingB2D0PiNoIPD2KPIPIDBeauty2CharmLineDecision",
    "StrippingB2D0PiPiPiD2HHTIGHTBeauty2CharmLineDecision",
    "StrippingB2DDphi_B2D0D0PhiLineDecision",
    "StrippingBc2JpsiTauNuForB2XTauNuAllLinesDecision",
    "StrippingBc2JpsiTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingBs2DsTauNuForB2XTauNuAllLinesDecision",
    "StrippingBs2DsTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingBs2DsTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingBu2D0TauNuForB2XTauNuAllLinesDecision",
    "StrippingBu2D0TauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingBu2D0TauNuWSForB2XTauNuAllLinesDecision",
    "StrippingD02HHTopoTOSLineDecision",
    "StrippingHb2Charged2BodyB2Charged2BodyLineDecision",
    "StrippingLb2LcPiNoIPLc2PKPiBeauty2CharmLineDecision",
    "StrippingLb2LcTauNuForB2XTauNuAllLinesDecision",
    "StrippingLb2LcTauNuInvVertForB2XTauNuAllLinesDecision",
    "StrippingLb2LcTauNuNonPhysTauForB2XTauNuAllLinesDecision",
    "StrippingLb2LcTauNuWSForB2XTauNuAllLinesDecision",
    "StrippingLb2XicPiNoIPXic2PKPiBeauty2CharmLineDecision",
    "StrippingOmegab2Omegac0PiNoIPOmegac02PKKPiBeauty2CharmLineDecision",
    "StrippingPseudoDoubleTopoLineDecision",
    "StrippingXib2Xic0PiNoIPXic02PKKPiBeauty2CharmLineDecision"
    ]
stripTool.Verbose = True
trigTool = tuple.addTupleTool( TupleToolTrigger )
trigTool.Verbose = True
trigTool.TriggerList = [ 'L0HadronDecision',
                         'Hlt1TrackAllL0Decision',
                         'Hlt2Topo2BodyBBDTDecision',
                         'Hlt2Topo3BodyBBDTDecision',
                         'Hlt2Topo4BodyBBDTDecision'
                         ]
#lokiT = tuple.addTupleTool( LoKi__Hybrid__TupleTool, name = "LokiTool" )
#lokiT.Variables = { "MIPCHI2_PRIMARY" : "MIPCHI2DV(PRIMARY)" }
DaVinci().UserAlgorithms += [ tuple ]

DaVinci().EvtMax     = -1
DaVinci().PrintFreq  = 1000
#DaVinci().SkipEvents = 100000

DaVinci().InputType = 'DST'
DaVinci().Simulation = False

DaVinci().DataType  = "2015"

NTupleSvc().Output += ["ANNPIDTUPLE DATAFILE='ProtoPIDANN.Data.tuples.root' TYP='ROOT' OPT='NEW'"]

from Configurables import LHCbApp
LHCbApp().TimeStamp = True
