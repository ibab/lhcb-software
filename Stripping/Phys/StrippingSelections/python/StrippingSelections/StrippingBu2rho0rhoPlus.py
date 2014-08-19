
"""
Stripping lines for B+ -> rho0 rho+
rewritten for Stripping21
lines for both resolved and merged pi0's
"""

__author__  = "Jason E Andrews"
__date__    = "10/06/2014"
__version__ = "2.0"
__all__     = ( "default_config",
                "StrippingBu2rho0rhoPlusConf" )

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
# from StandardParticles import StdLoosePions, StdLooseMergedPi0, StdLooseResolvedPi0
from StandardParticles import StdAllNoPIDsPions, StdLooseMergedPi0, StdLooseResolvedPi0

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
default_config = {
    "NAME"        : "Bu2rho0rhoPlus",
    "WGs"         : [ "Charmless" ],
    "BUILDERTYPE" : "StrippingBu2rho0rhoPlusConf",
    "STREAMS"     : { "Bhadron" : [ "StrippingBu2rho0rhoPlusMergedLine",
                                    "StrippingBu2rho0rhoPlusResolvedLine" ] },
    "CONFIG"      : { "isMC"                  : False,  # True = no Hlt filter
                      "refitPVs"              : True,
                      
                      "vetoTrISMUON"          : True,   # ~ISMUON causes errors?
                      "trMinIPChi2"           : 4,
                      "trMinProbNNpi"         : 0.0,
                      "trMaxChi2Dof"          : 3.0,
                      "trMaxGhostProb"        : 0.5,
                                              
                      "rhoCombMassMax"        : 1300,   # MeV
                      "rhoCombMassMin"        : 100,    # MeV
                      "rhoMothMassMax"        : 1200,   # MeV
                      "rhoMothMassMin"        : 200,    # MeV
                      "rho0MinVChi2Dof"       : 14,
                                              
                      "pi0ResMinCL"           : 0.0009,
                      "pi0ResMinP"            : 3500,   # MeV
                      "pi0ResMinPT"           : 400,    # MeV
                      "piPRhoPResMinIPChi2"   : 20,
                      "rho0ResMinIPChi2"      : 20,
                      "rho0ResMinFDChi2"      : 27,
                      "rhoPResMinP"           : 7000,   # MeV
                      "rhoPResMinPT"          : 1000,   # MeV
                                              
                      "BuResCombMassWindow"   : 650,    # MeV
                      "BuResMaxTrIPChi2Min"   : 40,
                      "BuResMaxTrPTMin"       : 1900,   # MeV
                      "BuResSumTrPTMin"       : 3800,   # MeV
                      "BuResMinVChi2Dof"      : 8,
                      "BuResMothMassWindow"   : 600,    # MeV
                      "BuResMinFDChi2"        : 120,
                      "BuResMaxIPChi2"        : 30,
                      "BuResMinDira"          : 0.9998,
                      "BuResMinPT"            : 1000,   # MeV
                                              
                      "pi0MgdMinPT"           : 1900,   # MeV
                      "piPRhoPMgdMinPT"       : 960,    # MeV
                      "rho0MgdDauMinPT"       : 100,    # MeV
                      "rho0MgdMinIPChi2"      : 20,
                      "rho0MgdMinFDChi2"      : 25,
                      "rho0MgdMaxTrIPChi2Min" : 20,
                                              
                      "BuMgdCombMassMin"      : 3900,   # MeV
                      "BuMgdCombMassMax"      : 7150,   # MeV
                      "BuMgdMaxTrIPChi2Min"   : 20,
                      "BuMgdMinFDChi2"        : 120,
                      "BuMgdMinVChi2Dof"      : 8,
                      "BuMgdMothMassMin"      : 4000,   # MeV
                      "BuMgdMothMassMax"      : 7000,   # MeV
                      "BuMgdMinDira"          : 0.9995,
                      "BuMgdMinPT"            : 4000,   # MeV
                      
                      "PrescaleBu2rho0rhoPlusResolved" : 1.0,
                      "PrescaleBu2rho0rhoPlusMerged"   : 1.0
                      }
    }

### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class StrippingBu2rho0rhoPlusConf(LineBuilder) :

    __configuration_keys__ = (
        "isMC",
        "refitPVs",
        
        "vetoTrISMUON",       
        "trMinIPChi2",      
        "trMinProbNNpi",      
        "trMaxChi2Dof",       
        "trMaxGhostProb",     
                              
        "rhoCombMassMax",     
        "rhoCombMassMin",     
        "rhoMothMassMax",     
        "rhoMothMassMin",     
        "rho0MinVChi2Dof",    
                              
        "pi0ResMinCL",
        "pi0ResMinP",
        "pi0ResMinPT",
        "piPRhoPResMinIPChi2",
        "rho0ResMinIPChi2", 
        "rho0ResMinFDChi2",
        "rhoPResMinP",
        "rhoPResMinPT",
                              
        "BuResCombMassWindow",
        "BuResMaxTrIPChi2Min",
        "BuResMaxTrPTMin",
        "BuResSumTrPTMin",    
        "BuResMinVChi2Dof",   
        "BuResMothMassWindow",
        "BuResMinFDChi2",
        "BuResMaxIPChi2",
        "BuResMinDira",
        "BuResMinPT",
                              
        "pi0MgdMinPT",
        "piPRhoPMgdMinPT",
        "rho0MgdDauMinPT",    
        "rho0MgdMinIPChi2", 
        "rho0MgdMinFDChi2",
        "rho0MgdMaxTrIPChi2Min",

        "BuMgdCombMassMin",   
        "BuMgdCombMassMax",   
        "BuMgdMaxTrIPChi2Min",
        "BuMgdMinFDChi2",
        "BuMgdMinVChi2Dof",   
        "BuMgdMothMassMin",   
        "BuMgdMothMassMax",   
        "BuMgdMinDira",
        "BuMgdMinPT",
        
        "PrescaleBu2rho0rhoPlusResolved",
        "PrescaleBu2rho0rhoPlusMerged"
        )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::    
    def __init__(self, name, config) :
        self.name = name
        LineBuilder.__init__(self, name, config)
        modeName = "Bu2rho0rhoPlus"
        ### filters :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        mgdVoidFilter = "(CONTAINS('Phys/StdLooseMergedPi0/Particles')>0)"
        resVoidFilter = "(CONTAINS('Phys/StdLooseResolvedPi0/Particles')>0)"

        mgdHltFilter = None
        resHltFilter = None
        if not config["isMC"]:
            hlt1Filter = "HLT_PASS_RE('Hlt1Track(AllL0|Photon)Decision')"
            mgdHltFilter = hlt1Filter + " & " \
            "HLT_PASS_RE('Hlt2(Topo[23]BodyBBDT|B2HHPi0_Merged)Decision')"
            resHltFilter = hlt1Filter + " & " \
            "HLT_PASS_RE('Hlt2Topo[23]BodyBBDTDecision')"
        ### build cuts ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        trackCuts = ""
        if config["vetoTrISMUON"]:
            trackCuts += "(~ISMUON) &"
        trackCuts += "(MIPCHI2DV(PRIMARY) > %(trMinIPChi2)s)"\
                     " & (PROBNNpi > %(trMinProbNNpi)s)"\
                     " & (TRCHI2DOF < %(trMaxChi2Dof)s)"\
                     " & (TRGHOSTPROB < %(trMaxGhostProb)s)" % locals()["config"]
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        rhoCombMassWindow = "(AM > %(rhoCombMassMin)s)"\
                            " & (AM < %(rhoCombMassMax)s)" % locals()["config"]
        
        rho0CombCuts = rhoCombMassWindow
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        rho0ResMotherCuts = "(MIPCHI2DV(PRIMARY) > %(rho0ResMinIPChi2)s)"\
                            " & (BPVVDCHI2 > %(rho0ResMinFDChi2)s)"\
                            " & (M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s)"\
                            " & (VFASPF(VCHI2/VDOF) < %(rho0MinVChi2Dof)s)"\
                            % locals()["config"]
        
        rhoPResDaughtersCuts = {
            "pi+" : "(MIPCHI2DV(PRIMARY) > %(piPRhoPResMinIPChi2)s)"\
            % locals()["config"],
            "pi0" : "(CL > %(pi0ResMinCL)s) & (P > %(pi0ResMinP)s)"\
            " & (PT > %(pi0ResMinPT)s)" % locals()["config"]
            }
        
        rhoPResMotherCuts = "(M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s)"\
                            " & (P > %(rhoPResMinP)s) & (PT > %(rhoPResMinPT)s)"\
                            % locals()["config"]
        
        BuResCombCuts = "(ADAMASS('B0') < %(BuResCombMassWindow)s)"\
                        " & (APT > %(BuResMinPT)s)"\
                        % locals()["config"]
        
        BuResMotherCuts = "(SUMTREE('pi+'==ABSID,PT) > %(BuResSumTrPTMin)s)"\
                          " & (ADMASS('B+') < %(BuResMothMassWindow)s)"\
                          " & (MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))"\
                          " > %(BuResMaxTrIPChi2Min)s)"\
                          " & (MAXTREE('pi+'==ABSID,PT) > %(BuResMaxTrPTMin)s)"\
                          " & (BPVVDCHI2 > %(BuResMinFDChi2)s)"\
                          " & (MIPCHI2DV(PRIMARY) < %(BuResMaxIPChi2)s)"\
                          " & (BPVDIRA > %(BuResMinDira)s)"\
                          " & (VFASPF(VCHI2/VDOF) < %(BuResMinVChi2Dof)s)"\
                          % locals()["config"]
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::        
        rho0MgdDaughtersCuts = {
            "pi+" : "PT > %(rho0MgdDauMinPT)s" % locals()["config"],
            "pi-" : "PT > %(rho0MgdDauMinPT)s" % locals()["config"]
            }
        
        rho0MgdMotherCuts = "(MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))"\
                            " > %(rho0MgdMaxTrIPChi2Min)s)"\
                            " & (MIPCHI2DV(PRIMARY) > %(rho0MgdMinIPChi2)s)"\
                            " & (BPVVDCHI2 > %(rho0MgdMinFDChi2)s)"\
                            " & (M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s)"\
                            " & (VFASPF(VCHI2/VDOF) < %(rho0MinVChi2Dof)s)"\
                            % locals()["config"]
        
        rhoPMgdDaughtersCuts = {
            "pi+" : "(PT > %(piPRhoPMgdMinPT)s)" % locals()["config"],
            "pi0" : "(PT > %(pi0MgdMinPT)s)" % locals()["config"]
            }
        
        rhoPMgdMotherCuts = "ALL"
        
        BuMgdCombCuts = "(AM > %(BuMgdCombMassMin)s) & (AM < %(BuMgdCombMassMax)s)"\
                        " & (APT > %(BuMgdMinPT)s)"\
                        % locals()["config"]
        BuMgdMotherCuts = "(MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))"\
                          " > %(BuMgdMaxTrIPChi2Min)s)"\
                          " & (M > %(BuMgdMothMassMin)s) & (M < %(BuMgdMothMassMax)s)"\
                          " & (BPVVDCHI2 > %(BuMgdMinFDChi2)s)"\
                          " & (BPVDIRA > %(BuMgdMinDira)s)"\
                          " & (VFASPF(VCHI2/VDOF) < %(BuMgdMinVChi2Dof)s)"\
                          % locals()["config"]
        ### shared particles ::::::::::::::::::::::::::::::::::::::::::::::::::::
        trackFilter = FilterDesktop( Code = trackCuts )
        tracks = Selection( "tracks" + self.name,
                            Algorithm = trackFilter,
                            RequiredSelections = [ StdAllNoPIDsPions ] )        
        ### resolved line :::::::::::::::::::::::::::::::::::::::::::::::::::::::
        selRho0Res = makeRho0( "Rho0Res",
                               combCuts = rho0CombCuts,
                               mothCuts = rho0ResMotherCuts,
                               inputSel = [ tracks ] )
        
        selRhoPRes = makeRhoP( "RhoPRes",
                               DauCuts = rhoPResDaughtersCuts,
                               combCuts = rhoCombMassWindow,
                               mothCuts = rhoPResMotherCuts,
                               inputSel = [ tracks, StdLooseResolvedPi0 ] )
        
	selBuRes = makeBu2rho0rhoPlus(
            modeName + "Resolved",
            combCuts = BuResCombCuts,
            mothCuts = BuResMotherCuts,
            inputSel = [ selRho0Res, selRhoPRes ],
            refit = config["refitPVs"] )
        
        self.stripCandResolved = selBuRes
        self.Bu2rho0rhoPlusResolvedLine = StrippingLine(
            modeName + "ResolvedLine",
            prescale = config["PrescaleBu2rho0rhoPlusResolved"],
            HLT = resHltFilter,
            FILTER = resVoidFilter,
            checkPV = True,
            RelatedInfoTools = [ relInfoCVDict( angle = 1.7,
                                                selections = [selBuRes, selRho0Res, selRhoPRes],
                                                recursion = 1),
                                 relInfoCVDict( angle = 1.35,
                                                selections = [selBuRes, selRho0Res, selRhoPRes],
                                                recursion = 1),
                                 relInfoCVDict( angle = 1.0,
                                                selections = [selBuRes, selRho0Res, selRhoPRes],
                                                recursion = 1),
                                 relInfoVtxDict( selections = [selBuRes],
                                                 recursion = 0 ) ],
            
            RequiredRawEvents = [ 'Calo' ],
            selection = self.stripCandResolved )
        
        self.registerLine( self.Bu2rho0rhoPlusResolvedLine )
        ### merged line :::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        selRho0Mgd = makeRho0( "Rho0Mgd",
                               DauCuts = rho0MgdDaughtersCuts,
                               combCuts = rho0CombCuts,
                               mothCuts = rho0MgdMotherCuts,
                               inputSel = [ tracks ]
                               )
        
        selRhoPMgd = makeRhoP( "RhoPMgd",
                               DauCuts = rhoPMgdDaughtersCuts,
                               combCuts = rhoCombMassWindow,
                               mothCuts = rhoPMgdMotherCuts,
                               inputSel = [ tracks, StdLooseMergedPi0 ]
                               )
        
	selBuMgd = makeBu2rho0rhoPlus(
            modeName + "Merged",
            combCuts = BuMgdCombCuts,
            mothCuts = BuMgdMotherCuts,
            inputSel = [ selRho0Mgd, selRhoPMgd ],
            refit = config["refitPVs"] )
            
        self.stripCandMerged = selBuMgd
        self.Bu2rho0rhoPlusMergedLine = StrippingLine(
            modeName + "MergedLine",
            prescale = config["PrescaleBu2rho0rhoPlusMerged"],
            HLT = mgdHltFilter,
            FILTER = mgdVoidFilter,
            checkPV = True,
            RelatedInfoTools = [ relInfoCVDict( angle = 1.7,
                                                selections = [selBuMgd, selRho0Mgd, selRhoPMgd],
                                                recursion = 1),
                                 relInfoCVDict( angle = 1.35,
                                                selections = [selBuMgd, selRho0Mgd, selRhoPMgd],
                                                recursion = 1),
                                 relInfoCVDict( angle = 1.0,
                                                selections = [selBuMgd, selRho0Mgd, selRhoPMgd],
                                                recursion = 1),
                                 relInfoVtxDict( selections = [selBuMgd],
                                                 recursion = 0 ) ],
            RequiredRawEvents = [ 'Calo' ],
            selection = self.stripCandMerged )
        
        self.registerLine( self.Bu2rho0rhoPlusMergedLine )
### helper functions ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeRho0( name ,
              combCuts,
              mothCuts,
              inputSel,
              DauCuts = {} ) :
    
    rho0 = CombineParticles( DecayDescriptor = "rho(770)0 -> pi+ pi-",
                             DaughtersCuts = DauCuts,
                             CombinationCut = combCuts,
                             MotherCut = mothCuts )
    
    return Selection( name + "Sel",
                      Algorithm = rho0,
                      RequiredSelections = inputSel )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeRhoP( name,
              DauCuts,
              combCuts,
              mothCuts,
              inputSel ) :
    
    rhoP = CombineParticles( DecayDescriptor = "[rho(770)+ -> pi+ pi0]cc",
                             DaughtersCuts = DauCuts,
                             CombinationCut = combCuts,
                             MotherCut = mothCuts,
                             ParticleCombiners = { "" : "ParticleAdder" } )
    
    return Selection( name + "Sel",
                      Algorithm = rhoP,
                      RequiredSelections = inputSel )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeBu2rho0rhoPlus( name,
                        combCuts,
                        mothCuts,
                        inputSel,
                        refit = True ) :

    Bu = CombineParticles( DecayDescriptor = "[B+ -> rho(770)0 rho(770)+]cc",
                           CombinationCut = combCuts,
                           MotherCut = mothCuts,
                           ReFitPVs = refit )

    return Selection( name + "Sel",
                      Algorithm = Bu,
                      RequiredSelections = inputSel )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def relInfoCVDict( prefix = '', angle = 1, selections = None, recursion = 0 ):
    Locations = {}
    for index, sel in enumerate(selections):
        Locations[sel] = prefix + 'maps/' + str(angle) + '/' + str(index)
    return { 'Type'           : 'RelInfoConeVariables',
             'ConeAngle'      : angle,
             'Variables'      : [ 'CONEANGLE', 'CONEMULT',
                                  'CONEP', 'CONEPASYM',
                                  'CONEPT', 'CONEPTASYM' ],
             'RecursionLevel' : recursion,
             'Locations'      : Locations }
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def relInfoVtxDict( prefix = '', selections = None, recursion = 0 ):
    Locations = {}
    for index, sel in enumerate(selections):
        Locations[sel] = prefix + 'maps/vtx/' + str(index)
    return { 'Type'           : 'RelInfoVertexIsolation',
             'Variables'      : [ 'VTXISONUMVTX',
                                  'VTXISODCHI2ONETRACK', 'VTXISODCHI2MASSONETRACK',
                                  'VTXISODCHI2TWOTRACK', 'VTXISODCHI2MASSTWOTRACK' ],
             'RecursionLevel' : recursion,
             'Locations'      : Locations }
