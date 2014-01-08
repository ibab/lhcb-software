"""
Stripping lines for B+ -> rho0 rho+
lines for both resolved and merged pi0's
"""

__author__  = "Jason E Andrews"
__date__    = "09/12/2013"
__version__ = "1.0"
__all__     = ( "StrippingBu2rho0rhoPlusConf",
                "makeRhoP",
                "makeBu2rho0rhoPlus"
                )

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
#from GaudiConfUtils.ConfigurableGenerators import CombineParticles
#from Configurables import FilterDesktop
from StandardParticles import StdLoosePions, StdLooseMergedPi0, StdLooseResolvedPi0

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder

default_config = {
    ### isMC = True: do not apply hlt filter and do not use TisTosParticleTagger
    "isMC"                : False,
    "trMinIPChi2PV"       : 16,
    "trMinProbNNpi"       : 0.5,
    "trMaxProbNNK"        : 0.5,
    "trMaxChi2Dof"        : 3.0,
    "trMaxGhostProb"      : 0.5,
    "rhoCombMassMax"      : 1200,
    "rhoCombMassMin"      : 200,
    "rhoMothMassMax"      : 1100,
    "rhoMothMassMin"      : 300,
    "rho0DauMaxDocaChi2"  : 15,
    "rho0MinDeltaZ"       : 0.0,
    "rho0MinVChi2Dof"     : 9,
    "rho0MinDira"         : 0.0,
    "rho0MinVDisplChi2"   : 25,
    
    "pi0ResMinP"          : 4000,
    "pi0ResMinPT"         : 600,
    "pi0ResDauMinCL"      : 0.2,
    "pi0ResDauMinPT"      : 250,
    "rhoPResMinPT"        : 1200,
    "BuResCombMassWindow" : 750,
    "BuResMinVChi2Dof"    : 9,
    "BuResMothMassWindow" : 600,
    "BuResMinDira"        : 0.99995,
    "BuResMinPT"          : 3000,
    
    "pi0MgdMinPT"         : 2200,
    "BuMgdCombMassMin"    : 3900,
    "BuMgdCombMassMax"    : 7150,
    "BuMgdMinVChi2Dof"    : 9,
    "BuMgdMothMassMin"    : 4000,
    "BuMgdMothMassMax"    : 7000,
    "BuMgdMinDira"        : 0.99994,
    "PrescaleBu2rho0rhoPlusResolved" : 1.0,
    "PrescaleBu2rho0rhoPlusMerged"   : 1.0
    }

class StrippingBu2rho0rhoPlusConf(LineBuilder) :

    __configuration_keys__ = ( "isMC",
                               "trMinIPChi2PV",
                               "trMinProbNNpi",
                               "trMaxProbNNK",
                               "trMaxChi2Dof",
                               "trMaxGhostProb",
                               "rhoCombMassMax",
                               "rhoCombMassMin",
                               "rhoMothMassMax",
                               "rhoMothMassMin",
                               "rho0DauMaxDocaChi2",
                               "rho0MinDeltaZ",
                               "rho0MinVChi2Dof",
                               "rho0MinDira",
                               "rho0MinVDisplChi2",
                               "pi0ResMinP",
                               "pi0ResMinPT",
                               "pi0ResDauMinCL",
                               "pi0ResDauMinPT",
                               "rhoPResMinPT",
                               "BuResCombMassWindow",
                               "BuResMinVChi2Dof",
                               "BuResMothMassWindow",
                               "BuResMinDira",
                               "BuResMinPT",
                               "pi0MgdMinPT",
                               "BuMgdCombMassMin",
                               "BuMgdCombMassMax",
                               "BuMgdMinVChi2Dof",
                               "BuMgdMothMassMin",
                               "BuMgdMothMassMax",
                               "BuMgdMinDira",
                               "PrescaleBu2rho0rhoPlusResolved",
                               "PrescaleBu2rho0rhoPlusMerged"
                               )

    def __init__(self, name, config) :
        self.name = name
        LineBuilder.__init__(self, name, config)
        modeName = "Bu2rho0rhoPlus"
        ### filters ----------------------------------------------------------------------------------------
        _resVoidFilter = "((CONTAINS('Phys/StdLoosePions/Particles')>0) & (CONTAINS('Phys/StdLooseResolvedPi0/Particles')>0))"
        _mgdVoidFilter = "((CONTAINS('Phys/StdLoosePions/Particles')>0) & (CONTAINS('Phys/StdLooseMergedPi0/Particles')>0))"
        _hltFilter = "( HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[23].*Decision') )"
        _TisTosTaggerSpecs = { 'Hlt2Topo[23].*Decision%TOS' : 0 }
        if config["isMC"]:
            _hltFilter = None
        ### build cuts -------------------------------------------------------------------------------------
        _trackCuts = ("(MIPCHI2DV(PRIMARY) > %(trMinIPChi2PV)s) & (PROBNNpi > %(trMinProbNNpi)s) & (PROBNNK < %(trMaxProbNNK)s)"
                      " & (TRCHI2DOF < %(trMaxChi2Dof)s) & (TRGHOSTPROB < %(trMaxGhostProb)s)") % locals()["config"]
        
        _rhoCombMassWindow = "(AM > %(rhoCombMassMin)s) & (AM < %(rhoCombMassMax)s)" % locals()["config"]
        
        _rho0CombCuts = _rhoCombMassWindow + " & (ADOCACHI2CUT(%(rho0DauMaxDocaChi2)s,''))" % locals()["config"]
        _rho0MotherCuts = ("(M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s) & (BPVVDZ > %(rho0MinDeltaZ)s) & (VFASPF(VCHI2/VDOF) < %(rho0MinVChi2Dof)s)"
                           " & (BPVDIRA > %(rho0MinDira)s) & (BPVVDCHI2 > %(rho0MinVDisplChi2)s)") % locals()["config"]
        
        _rhoPResDaughtersCuts = {
            "pi+" : "ALL",
            "pi0" : ("(P > %(pi0ResMinP)s) & (PT > %(pi0ResMinPT)s) & (CHILD(CL,1) > %(pi0ResDauMinCL)s) & (CHILD(CL,2) > %(pi0ResDauMinCL)s)"
                     " & (CHILD(PT,1) > %(pi0ResDauMinPT)s) & (CHILD(PT,2) > %(pi0ResDauMinPT)s)") % locals()["config"]
            }
        _rhoPResMotherCuts = "(M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s) & (PT > %(rhoPResMinPT)s)" % locals()["config"]
        
        _BuResCombCuts = "(ADAMASS('B0') < %(BuResCombMassWindow)s)" % locals()["config"]
        _BuResMotherCuts = ("(VFASPF(VCHI2/VDOF) < %(BuResMinVChi2Dof)s) & (ADMASS('B+') < %(BuResMothMassWindow)s)"
                            " & (BPVDIRA > %(BuResMinDira)s) & (PT > %(BuResMinPT)s)") % locals()["config"]

        _rhoPMgdDaughtersCuts = {
            "pi+" : "ALL",
            "pi0" : "(PT > %(pi0MgdMinPT)s)" % locals()["config"]
            }
        _rhoPMgdMotherCuts = "ALL"
        
        _BuMgdCombCuts = "(AM > %(BuMgdCombMassMin)s) & (AM < %(BuMgdCombMassMax)s)" % locals()["config"]
        _BuMgdMotherCuts = ("(M > %(BuMgdMothMassMin)s) & (M < %(BuMgdMothMassMax)s) & (VFASPF(VCHI2/VDOF) < %(BuMgdMinVChi2Dof)s)"
                            " & (BPVDIRA > %(BuMgdMinDira)s)") % locals()["config"]
        
        ### shared particles -------------------------------------------------------------------------------
        _trackFilter = FilterDesktop( Code = _trackCuts )
        _tracks = Selection( "tracks" + self.name, Algorithm = _trackFilter, RequiredSelections = [ StdLoosePions ] )
        
        _rho0 = CombineParticles(
                                  DecayDescriptor = "rho(770)0 -> pi+ pi-",
                                  CombinationCut = _rho0CombCuts,
                                  MotherCut = _rho0MotherCuts
                                  )
        _selRho0 = Selection( "selRho0",
                              Algorithm = _rho0,
                              RequiredSelections = [ _tracks ]
                              )
        
        ### resolved line ----------------------------------------------------------------------------------
        _selRhoPResolved = makeRhoP( "RhoPResolved",
                                     DauCuts = _rhoPResDaughtersCuts,
                                     combCuts = _rhoCombMassWindow,
                                     mothCuts = _rhoPResMotherCuts,
                                     inputSel = [ _tracks, StdLooseResolvedPi0 ]
                                     )

	_selBuResolved = makeBu2rho0rhoPlus( modeName + "Resolved",
                                             combCuts = _BuResCombCuts,
                                             mothCuts = _BuResMotherCuts,
                                             inputSel = [ _selRho0, _selRhoPResolved ]
                                             )
        
        if config["isMC"]:
            self.stripCandResolved = _selBuResolved
        else:
            # self.stripCandResolved = makeTisTosSelection( _selBuResolved, _TisTosTaggerSpecs )
            self.stripCandResolved = _selBuResolved
            
        self.Bu2rho0rhoPlusResolvedLine = StrippingLine( modeName + "ResolvedLine",
                                                         prescale = config["PrescaleBu2rho0rhoPlusResolved"],
                                                         HLT = _hltFilter,
                                                         FILTER = _resVoidFilter,
                                                         checkPV = True,
                                                         selection = self.stripCandResolved
                                                         )
            
        self.registerLine( self.Bu2rho0rhoPlusResolvedLine )

        ### merged line ------------------------------------------------------------------------------------
        _selRhoPMerged = makeRhoP( "RhoPMerged",
                                   DauCuts = _rhoPMgdDaughtersCuts,
                                   combCuts = _rhoCombMassWindow,
                                   mothCuts = _rhoPMgdMotherCuts,
                                   inputSel = [ _tracks, StdLooseMergedPi0 ]
                                   )
        
	_selBuMerged = makeBu2rho0rhoPlus( modeName + "Merged",
                                           combCuts = _BuMgdCombCuts,
                                           mothCuts = _BuMgdMotherCuts,
                                           inputSel = [ _selRho0, _selRhoPMerged ]
                                           )
        
        if config["isMC"]:
            self.stripCandMerged = _selBuMerged
        else:
            # self.stripCandMerged = makeTisTosSelection( _selBuMerged, _TisTosTaggerSpecs )
            self.stripCandMerged = _selBuMerged
        
        self.Bu2rho0rhoPlusMergedLine = StrippingLine( modeName + "MergedLine",
                                                       prescale = config["PrescaleBu2rho0rhoPlusMerged"],
                                                       HLT = _hltFilter,
                                                       FILTER = _mgdVoidFilter,
                                                       checkPV = True,
                                                       selection = self.stripCandMerged
                                                       )
        
        self.registerLine( self.Bu2rho0rhoPlusMergedLine )

def makeRhoP( name,
              DauCuts,
              combCuts,
              mothCuts,
              inputSel
              ) :

    _rhoP = CombineParticles( DecayDescriptor = "[rho(770)+ -> pi+ pi0]cc",
                              DaughtersCuts = DauCuts,
                              CombinationCut = combCuts,
                              MotherCut = mothCuts,
                              ParticleCombiners = { "" : "ParticleAdder" }
                              )

    return Selection( name + "Sel",
                      Algorithm = _rhoP,
                      RequiredSelections = inputSel
                      )

def makeBu2rho0rhoPlus( name,
                        combCuts,
                        mothCuts,
                        inputSel
                        ) :

    _Bu = CombineParticles( DecayDescriptor = "[B+ -> rho(770)0 rho(770)+]cc",
                            CombinationCut = combCuts,
                            MotherCut = mothCuts,
                            )

    return Selection( name + "Sel",
                      Algorithm = _Bu,
                      RequiredSelections = inputSel
                      )

def makeTisTosSelection( selection,
                         Specs
                         ) :

    from Configurables import TisTosParticleTagger
    myTagger = TisTosParticleTagger('myTagger')
    myTagger.TisTosSpecs = Specs
    ### to speed things up:
    # myTagger.ProjectTracksToCalo = False
    # myTagger.CaloClustForCharged = False
    # myTagger.CaloClustForNeutral = False
    # myTagger.TOSFrac = {4:0.0, 5:0.0}
    
    return Selection( selection.name() + "TISTOS",
                      Algorithm = myTagger,
                      RequiredSelections = [ selection ]
                      )
