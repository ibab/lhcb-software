'''
B->DPi lifetime-unbiased selection for lifetime resolution calibration
'''

__author__  = ["Anton Poluektov" ]
__date__    = "26/08/2011"
__version__ = ""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder, checkConfig
from StandardParticles import ( StdNoPIDsPions,
                                StdNoPIDsKaons,
                                StdNoPIDsDownPions,
                                StdNoPIDsProtons,
                                StdAllNoPIDsPions,
                                StdLooseMergedPi0,
                                StdLooseResolvedPi0 )

from copy import copy

moduleName = "B2DX"

config = {                                  # Default configuration dictionary

    	    "D2hhhCuts" : {                  # D->hhh cuts
    	      "DauChi2Max"   : 4.,           # maximum track chi2
    	      "DauPtMin"     : 250.,         # track Pt cut (MeV)
    	      "DauPMin"      : 2000.,        # track P cut (MeV)
    	      "DauMIPChi2Min": 4.,           # minimum IP chi2 for a track
    	      "CombDMass"    : 110.,         # mass window (MeV) for a combination
    	      "DMass"        : 100.,         # mass window (MeV) after vertex fit
    	      "VtxChi2Max"   : 9.,           # D vertex Chi2
    	      "VDChi2Min"    : 36.,          # D flight distance chi2
	      "PtMin"        : 1500.,        # Minimum Pt of the D (MeV)
	      "MaxDauMIPChi2": 40.,          # IP chi2 for at least one track
	      "DIRAMin"	     : 0.9,          # minimum D DIRA
	      "DOCAMax"      : 0.6,          # maximum DOCA of track combinations (mm)
	      "IPChi2Min"    : 0.,           # Minimum IP chi2
    	    }, 
	    "UnbiasedBCuts" : {                     # Lifetime-unbiased B->D(hhh)h selection
    	      "BachelorChi2Max"    : 4.,            # maximum track chi2
    	      "BachelorPtMin"      : 500.,          # track Pt cut (MeV)
    	      "BachelorPMin"       : 5000.,         # track P cut (MeV)
    	      "BachelorMIPChi2Min" : 0.,            # minimum IP chi2 for a track
    	      "DMass" 		   : 100.,          # Mass window for a D combination (MeV)
	      "VtxChi2Max"         : 9.,            # B vertex Chi2
	      "IPChi2Max"          : 250.,          # chi2 of B impact parameter to the related PV
	      "LTMin"              : -1000.,        # Chi2 of B vertex separation from the related PV
	      "DIRAMin"            : -1.,           # DIRA of the B to the related PV
	      "CombDMass"          : 500,           # Mass window for a B combination (MeV)
	    }, 
	    "Prescale"         : 0.05,                # Prescales for individual lines
	    "CheckPV"	       : True,              # PV requirement
	    "MaxTracksInEvent" : 500                # GECs for individual lines (max. number of long tracks)
}


class UnbiasedB2DPiConf(LineBuilder) :
    __configuration_keys__ = (
                              "D2hhhCuts",
                              "UnbiasedBCuts",
                              "Prescale",
                              "CheckPV",
                              "MaxTracksInEvent")

    def __init__(self, moduleName, config) :

        LineBuilder.__init__(self, moduleName, config)

        from Configurables import LoKi__VoidFilter as VoidFilter

        D2hhh = makeD2hhh(moduleName, config["D2hhhCuts"])

        unbiasedSelection = makeUnbiasedB2DPi("Unbiased" + moduleName, "D2hhh", D2hhh, config["UnbiasedBCuts"]) #NeedsAdditionalString?

        HLT1TIS = makeTISTOSSel("HLT1TISSelFor" + moduleName + "WithUnbiasedBs2DsPi", unbiasedSelection, "Hlt1Global%TIS")
        HLT2TIS = makeTISTOSSel("HLT2TISSelFor" + moduleName + "WithUnbiasedBs2DsPi", HLT1TIS, "Hlt2Global%TIS")

        line = StrippingLine(moduleName + "WithUnbiasedB2DPiLine", prescale = config["Prescale"] ,
                             selection = HLT2TIS,
                             checkPV = config["CheckPV"],
                             FILTER = {
                                 "Code" : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )" % config["MaxTracksInEvent"], 
                                 "Preambulo" : [ "from LoKiTracks.decorators import *" ]
                             } ) 
        self.registerLine( line )


def makeD2hhh(moduleName, config) :

    __configuration_keys__ = ("DauChi2Max", "DauPtMin", "DauPMin", "DauMIPChi2Min",
                              "PtMin", "CombDMass", "DMass", "VtxChi2Max", "VDChi2Min",
                              "MaxDauMIPChi2", "DIRAMin", "DOCAMax", "IPChi2Min")

    checkConfig(__configuration_keys__, config)

    StdPi = StdNoPIDsPions
    StdK = StdNoPIDsKaons

    Daughtercut = "((TRCHI2DOF<%(DauChi2Max)s) & " \
    "(PT > %(DauPtMin)s*MeV) & (P > %(DauPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(DauMIPChi2Min)s))" % config

    D2Kpipi = CombineParticles()
    D2Kpipi.DecayDescriptors = [ "[D+ -> K- pi+ pi+]cc" ]
    D2Kpipi.DaughtersCuts =  { "pi+"     : Daughtercut,
                                "K+"      : Daughtercut }

    D2Kpipi.CombinationCut = \
    "((ADAMASS('D+') < %(CombDMass)s*MeV) | (ADAMASS('D_s+') < %(CombDMass)s*MeV)) & " \
    "(APT>%(PtMin)s*MeV) & " \
    "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(DOCAMax)s) & " \
    "AHASCHILD(((ABSID == 'K+') | (ABSID == 'pi+')) & (MIPCHI2DV(PRIMARY) > %(MaxDauMIPChi2)s))" \
    % config

    D2Kpipi.MotherCut = "((ADMASS('D+') < %(DMass)s*MeV) | (ADMASS('D_s+') < %(DMass)s*MeV))" \
    "& (VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & (BPVVDCHI2 > %(VDChi2Min)s) & (BPVDIRA > %(DIRAMin)s) & (BPVIPCHI2() > %(IPChi2Min)s)" % config

    D2KpipiSel = Selection("D2KpipiFor" + moduleName, Algorithm = D2Kpipi, RequiredSelections = [ StdPi, StdK ] )

    D2KKpi = copy(D2Kpipi)
    D2KKpi.DecayDescriptors = [ "[D+ -> K- K+ pi+]cc" ]
    D2KKpiSel = Selection("D2KKpiFor" + moduleName, Algorithm = D2KKpi, RequiredSelections = [ StdPi, StdK ] )

    DSel = MergedSelection("D2hhhFor" + moduleName,
                           RequiredSelections = [ D2KpipiSel, D2KKpiSel ] )

    return DSel


def makeUnbiasedB2DPi(moduleName, DName, DSel, config ) :

    StdPi  = StdAllNoPIDsPions  # Because of no IPchi2 cut

    Bachelorcut = "((TRCHI2DOF<%(BachelorChi2Max)s) & " \
    "(PT > %(BachelorPtMin)s*MeV) & (P > %(BachelorPMin)s*MeV) & " \
    "(MIPCHI2DV(PRIMARY) > %(BachelorMIPChi2Min)s))" % config

    Dcut = "((MINTREE(ABSID=='K+',PIDK)>0) & (MAXTREE(ABSID=='pi+',PIDK)<5))"

    B2DPi = CombineParticles()
    B2DPi.DecayDescriptors = [ "[B_s0 -> D- pi+]cc" ]
    B2DPi.DaughtersCuts = { "pi+" : Bachelorcut, "D-" : Dcut }
    B2DPi.CombinationCut = "((ADAMASS('B0') < %(CombDMass)s *MeV) | (ADAMASS('B_s0') < %(CombDMass)s *MeV))" % config

    B2DPi.MotherCut = "((VFASPF(VCHI2/VDOF)<%(VtxChi2Max)s) & " \
    "(BPVDIRA > %(DIRAMin)s))" % config

    return Selection("Bs2DsPiWith" + DName + "For" + moduleName, Algorithm = B2DPi,
                        RequiredSelections = [ DSel, StdPi ] )

def makeTISTOSSel(name, sel, trigger ) :
    from Configurables import TisTosParticleTagger

    tisTosFilter = TisTosParticleTagger(name + "Filter")
    tisTosFilter.TisTosSpecs = { trigger : 0}
    # the rest ist only to speed it up... (TISTOSsing only done with tracking system)
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }

    return Selection(name, Algorithm = tisTosFilter, RequiredSelections = [ sel ] )
