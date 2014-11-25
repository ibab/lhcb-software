"""
Stripping lines for B+ -> rho0 rho+
Rewritten for Stripping21
Lines for both resolved and merged pi0's
Lines for both long and upstream tracks
"""

__author__  = "Jason E Andrews"
__date__    = "10/06/2014"
__version__ = "3.0"
__all__     = ( "default_config",
                "StrippingBu2rho0rhoPlusConf" )

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles import ( StdAllNoPIDsPions, StdNoPIDsUpPions,
                                StdLooseMergedPi0, StdLooseResolvedPi0 )

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
default_config = {
    "NAME"        : "Bu2rho0rhoPlus",
    "WGs"         : [ "Charmless" ],
    "BUILDERTYPE" : "StrippingBu2rho0rhoPlusConf",
    "STREAMS"     : { "Bhadron" : [ "StrippingBu2rho0rhoPlusMergedLine",
                                    "StrippingBu2rho0rhoPlusResolvedLine",
                                    "StrippingBu2rho0rhoPlusUpMergedLine",
                                    "StrippingBu2rho0rhoPlusUpResolvedLine", ] },
    "CONFIG"      : { "isMC"           : False,  # True = no Hlt filter
                      "refitPVs"       : True,
                      
                      "vetoTrISMUON"   : True,   # ~ISMUON causes errors?
                      "trMinIPChi2"    : 4,
                      "trUpMinIPChi2"  : 8,
                      "trMinProbNNpi"  : 0.0,
                      "trMaxChi2Dof"   : 3.0,
                      "trMaxGhostProb" : 0.5,

                      "longLines"     : { "rhoCombMassMax"        : 1300,   # MeV
                                          "rhoCombMassMin"        : 100,    # MeV
                                          "rhoMothMassMax"        : 1200,   # MeV
                                          "rhoMothMassMin"        : 200,    # MeV
                                          "rho0MinVChi2Dof"       : 14,
                                          
                                          "pi0ResMinCL"           :-1000,
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
                                          "rho0MgdMinIPChi2"      : 33,
                                          "rho0MgdMinFDChi2"      : 25,
                                          "rho0MgdMaxTrIPChi2Min" : 20,
                                          
                                          "BuMgdCombMassMin"      : 3900,   # MeV
                                          "BuMgdCombMassMax"      : 7150,   # MeV
                                          "BuMgdMaxTrIPChi2Min"   : 20,
                                          "BuMgdMinFDChi2"        : 120,
                                          "BuMgdMaxIPChi2"        : 450,
                                          "BuMgdMinVChi2Dof"      : 8,
                                          "BuMgdMothMassMin"      : 4000,   # MeV
                                          "BuMgdMothMassMax"      : 7000,   # MeV
                                          "BuMgdMinDira"          : 0.9997,
                                          "BuMgdMinPT"            : 4000,   # MeV
                                          },
                      "upstreamLines" : { "rhoCombMassMax"        : 1300,   # MeV
                                          "rhoCombMassMin"        : 100,    # MeV
                                          "rhoMothMassMax"        : 1200,   # MeV
                                          "rhoMothMassMin"        : 200,    # MeV
                                          "rho0MinVChi2Dof"       : 14,
                                          
                                          "pi0ResMinCL"           :-1000,
                                          "pi0ResMinP"            : 1000,   # MeV
                                          "pi0ResMinPT"           : 400,    # MeV
                                          "piPRhoPResMinIPChi2"   : 10,
                                          "rho0ResMinIPChi2"      : 55,
                                          "rho0ResMinFDChi2"      : 55,
                                          "rhoPResMinP"           : 9000,   # MeV
                                          "rhoPResMinPT"          : 1400,   # MeV
                                          
                                          "BuResCombMassWindow"   : 650,    # MeV
                                          "BuResMaxTrIPChi2Min"   : 0,
                                          "BuResMaxTrPTMin"       : 1600,   # MeV
                                          "BuResSumTrPTMin"       : 3500,   # MeV
                                          "BuResMinVChi2Dof"      : 8,
                                          "BuResMothMassWindow"   : 600,    # MeV
                                          "BuResMinFDChi2"        : 55,
                                          "BuResMaxIPChi2"        : 20,
                                          "BuResMinDira"          : 0.9998,
                                          "BuResMinPT"            : 1500,   # MeV
                                          
                                          "pi0MgdMinPT"           : 0,      # MeV
                                          "piPRhoPMgdMinPT"       : 0,      # MeV
                                          "rho0MgdDauMinPT"       : 0,      # MeV
                                          "rho0MgdMinIPChi2"      : 55,
                                          "rho0MgdMinFDChi2"      : 55,
                                          "rho0MgdMaxTrIPChi2Min" : 55,
                                          
                                          "BuMgdCombMassMin"      : 3900,   # MeV
                                          "BuMgdCombMassMax"      : 7150,   # MeV
                                          "BuMgdMaxTrIPChi2Min"   : 55,
                                          "BuMgdMinFDChi2"        : 55,
                                          "BuMgdMaxIPChi2"        : 55,
                                          "BuMgdMinVChi2Dof"      : 8,
                                          "BuMgdMothMassMin"      : 4000,   # MeV
                                          "BuMgdMothMassMax"      : 7000,   # MeV
                                          "BuMgdMinDira"          : 0.999,
                                          "BuMgdMinPT"            : 5000,   # MeV
                                          },
                      
                      "PrescaleBu2rho0rhoPlusResolved"   : 1.0,
                      "PrescaleBu2rho0rhoPlusMerged"     : 1.0,
                      "PrescaleBu2rho0rhoPlusUpResolved" : 1.0,
                      "PrescaleBu2rho0rhoPlusUpMerged"   : 1.0
                      }
    }

### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class StrippingBu2rho0rhoPlusConf(LineBuilder) :

    __configuration_keys__ = (
        "isMC",
        "refitPVs",
        
        "vetoTrISMUON",       
        "trMinIPChi2",
        "trUpMinIPChi2",
        "trMinProbNNpi",      
        "trMaxChi2Dof",       
        "trMaxGhostProb",

        "longLines",
        "upstreamLines",
        
        "PrescaleBu2rho0rhoPlusResolved",
        "PrescaleBu2rho0rhoPlusMerged",
        "PrescaleBu2rho0rhoPlusUpResolved",
        "PrescaleBu2rho0rhoPlusUpMerged"
        )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::    
    def __init__(self, name, config) :
        self.name = name
        LineBuilder.__init__(self, name, config)
        modeName = "Bu2rho0rhoPlus"
        ### filters :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        mgdVoidFilter = "(CONTAINS('Phys/StdLooseMergedPi0/Particles')>0)"
        resVoidFilter = "(CONTAINS('Phys/StdLooseResolvedPi0/Particles')>0)"

        mgdHltFilter   = None
        resHltFilter   = None
        mgdUpHltFilter = None
        resUpHltFilter = None
        if not config["isMC"]:
            hlt1Filter     = "HLT_PASS_RE('Hlt1Track(AllL0|Photon)Decision')"
            mgdHltFilter   = hlt1Filter + " & "\
                             "HLT_PASS_RE('Hlt2(Topo[23]BodyBBDT|B2HHPi0_Merged|"\
                             "RadiativeTopoPhoton)Decision')"
            resHltFilter   = hlt1Filter + " & "\
                             "HLT_PASS_RE('Hlt2Topo[23]BodyBBDTDecision')"
            mgdUpHltFilter = hlt1Filter + " & "\
                             "HLT_PASS_RE('Hlt2(Topo2BodyBBDT|B2HHPi0_Merged|"\
                             "RadiativeTopoPhoton)Decision')"
            resUpHltFilter = "HLT_PASS('Hlt1TrackAllL0Decision') & "\
                             "HLT_PASS('Hlt2Topo2BodyBBDTDecision')"
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        longTracksCuts      = ""
        upAndLongTracksCuts = ""
        if config["vetoTrISMUON"]:
            longTracksCuts      += "(~ISMUON) &"
            upAndLongTracksCuts += "(~ISMUON) &"
        longTracksCuts      += "(MIPCHI2DV(PRIMARY) > %(trMinIPChi2)s)"\
                               " & (PROBNNpi > %(trMinProbNNpi)s)"\
                               " & (TRCHI2DOF < %(trMaxChi2Dof)s)"\
                               " & (TRGHOSTPROB < %(trMaxGhostProb)s)"\
                               % locals()["config"]
        upAndLongTracksCuts += "(MIPCHI2DV(PRIMARY) > %(trUpMinIPChi2)s)"\
                               " & (PROBNNpi > %(trMinProbNNpi)s)"\
                               " & (TRCHI2DOF < %(trMaxChi2Dof)s)"\
                               " & (TRGHOSTPROB < %(trMaxGhostProb)s)"\
                               % locals()["config"]
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        longTracksFilter = FilterDesktop( Code = longTracksCuts )
        longTracks = Selection( "longTracksFor" + self.name,
                                Algorithm = longTracksFilter,
                                RequiredSelections = [ StdAllNoPIDsPions ] )        
        upAndLongTracksFilter = FilterDesktop( Code = upAndLongTracksCuts )
        upAndLongTracks = Selection( "upAndLongTracksFor" + self.name,
                                     Algorithm = upAndLongTracksFilter,
                                     RequiredSelections = [ StdAllNoPIDsPions,
                                                            StdNoPIDsUpPions ] )
        ### long tracks :::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        selBuMgdLong, mgdDaughters = makeBu2rho0rhoPlus(
            modeName + "Merged",
            config["longLines"],
            [ longTracks ],
            [ StdLooseMergedPi0 ],
            merged = True,
            refit = config["refitPVs"] )
        
        self.Bu2rho0rhoPlusMergedLine = StrippingLine(
            modeName + "MergedLine",
            prescale = config["PrescaleBu2rho0rhoPlusMerged"],
            HLT = mgdHltFilter,
            FILTER = mgdVoidFilter,
            checkPV = True,
            RelatedInfoTools = relInfoToolList( selBuMgdLong, mgdDaughters ),
            RequiredRawEvents = [ 'Calo' ],
            selection = selBuMgdLong )
        
        self.registerLine( self.Bu2rho0rhoPlusMergedLine )            
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        selBuResLong, resDaughters = makeBu2rho0rhoPlus(
            modeName + "Resolved",
            config["longLines"],
            [ longTracks ],
            [ StdLooseResolvedPi0 ],
            merged = False,
            refit = config["refitPVs"] )
        
        self.Bu2rho0rhoPlusResolvedLine = StrippingLine(
            modeName + "ResolvedLine",
            prescale = config["PrescaleBu2rho0rhoPlusResolved"],
            HLT = resHltFilter,
            FILTER = resVoidFilter,
            checkPV = True,
            RelatedInfoTools = relInfoToolList( selBuResLong, resDaughters ),
            RequiredRawEvents = [ 'Calo' ],
            selection = selBuResLong )
        
        self.registerLine( self.Bu2rho0rhoPlusResolvedLine )
        ### upstream ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        selBuMgdUp, mgdUpDaughters = makeBu2rho0rhoPlus(
            modeName + "UpMerged",
            config["upstreamLines"],
            [ upAndLongTracks ],
            [ StdLooseMergedPi0 ],
            merged = True,
            upstream = True,
            refit = config["refitPVs"] )
        
        self.Bu2rho0rhoPlusUpMergedLine = StrippingLine(
            modeName + "UpMergedLine",
            prescale = config["PrescaleBu2rho0rhoPlusUpMerged"],
            HLT = mgdUpHltFilter,
            FILTER = mgdVoidFilter,
            checkPV = True,
            RelatedInfoTools = relInfoToolList( selBuMgdUp, mgdUpDaughters ),
            RequiredRawEvents = [ 'Calo' ],
            selection = selBuMgdUp )
        
        self.registerLine( self.Bu2rho0rhoPlusUpMergedLine )            
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        selBuResUp, resUpDaughters = makeBu2rho0rhoPlus(
            modeName + "UpResolved",
            config["upstreamLines"],
            [ upAndLongTracks ],
            [ StdLooseResolvedPi0 ],
            merged = False,
            upstream = True,
            refit = config["refitPVs"] )
        
        self.Bu2rho0rhoPlusUpResolvedLine = StrippingLine(
            modeName + "UpResolvedLine",
            prescale = config["PrescaleBu2rho0rhoPlusUpResolved"],
            HLT = resUpHltFilter,
            FILTER = resVoidFilter,
            checkPV = True,
            RelatedInfoTools = relInfoToolList( selBuResUp, resUpDaughters ),
            RequiredRawEvents = [ 'Calo' ],
            selection = selBuResUp )
        
        self.registerLine( self.Bu2rho0rhoPlusUpResolvedLine )
### helper functions ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeRho0( name ,
              config,
              tracks,
              merged = True ) :
    ### -------------------------------------------------------------------------
    combCuts = "(AM > %(rhoCombMassMin)s)"\
               " & (AM < %(rhoCombMassMax)s)" % locals()["config"]
    ### -------------------------------------------------------------------------
    DaughtersCuts = {}
    
    motherCuts = "(MIPCHI2DV(PRIMARY) > %(rho0ResMinIPChi2)s)"\
                 " & (BPVVDCHI2 > %(rho0ResMinFDChi2)s)"\
                 " & (M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s)"\
                 " & (VFASPF(VCHI2/VDOF) < %(rho0MinVChi2Dof)s)"\
                 % locals()["config"]
    ### -------------------------------------------------------------------------
    if merged:
        DaughtersCuts = { "pi+" : "PT > %(rho0MgdDauMinPT)s" % locals()["config"],
                          "pi-" : "PT > %(rho0MgdDauMinPT)s" % locals()["config"] }
        motherCuts = "(MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))"\
                     " > %(rho0MgdMaxTrIPChi2Min)s)"\
                     " & (MIPCHI2DV(PRIMARY) > %(rho0MgdMinIPChi2)s)"\
                     " & (BPVVDCHI2 > %(rho0MgdMinFDChi2)s)"\
                     " & (M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s)"\
                     " & (VFASPF(VCHI2/VDOF) < %(rho0MinVChi2Dof)s)"\
                     % locals()["config"]
    ### -------------------------------------------------------------------------
    rho0 = CombineParticles( DecayDescriptor = "rho(770)0 -> pi+ pi-",
                             DaughtersCuts = DaughtersCuts,
                             CombinationCut = combCuts,
                             MotherCut = motherCuts,
                             ParticleCombiners = { "" : "OfflineVertexFitter" } )
    ### -------------------------------------------------------------------------
    return Selection( name,
                      Algorithm = rho0,
                      RequiredSelections = tracks )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeRhoP( name,
              config,
              tracks,
              pi0s,
              merged = True ) :
    ### -------------------------------------------------------------------------
    combCuts = "(AM > %(rhoCombMassMin)s)"\
               " & (AM < %(rhoCombMassMax)s)" % locals()["config"]
    ### -------------------------------------------------------------------------
    DaughtersCuts = { "pi+" : "(MIPCHI2DV(PRIMARY) > %(piPRhoPResMinIPChi2)s)"
                      % locals()["config"],
                      "pi0" : "(CL > %(pi0ResMinCL)s) & (P > %(pi0ResMinP)s)"\
                      " & (PT > %(pi0ResMinPT)s)" % locals()["config"] }
    
    motherCuts = "(M > %(rhoMothMassMin)s) & (M < %(rhoMothMassMax)s)"\
                 " & (P > %(rhoPResMinP)s) & (PT > %(rhoPResMinPT)s)"\
                 % locals()["config"]
    ### -------------------------------------------------------------------------    
    if merged:
        DaughtersCuts = { "pi+" : "(PT > %(piPRhoPMgdMinPT)s)" % locals()["config"],
                          "pi0" : "(PT > %(pi0MgdMinPT)s)" % locals()["config"] }
        motherCuts = "ALL"
    ### -------------------------------------------------------------------------
    rhoP = CombineParticles( DecayDescriptor = "[rho(770)+ -> pi+ pi0]cc",
                             DaughtersCuts = DaughtersCuts,
                             CombinationCut = combCuts,
                             MotherCut = motherCuts,
                             ParticleCombiners = { "" : "ParticleAdder" } )
    ### -------------------------------------------------------------------------
    return Selection( name,
                      Algorithm = rhoP,
                      RequiredSelections = tracks + pi0s )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeBu2rho0rhoPlus( name,
                        config,
                        tracks,
                        pi0s,
                        merged = True,
                        upstream = False,
                        refit = True ) :
    __configuration_keys__ = (
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
        "BuMgdMaxIPChi2",
        "BuMgdMinVChi2Dof",   
        "BuMgdMothMassMin",   
        "BuMgdMothMassMax",   
        "BuMgdMinDira",
        "BuMgdMinPT",
        )

    checkConfig( __configuration_keys__, config )

    ### -------------------------------------------------------------------------
    rho0 = makeRho0( "rho0For" + name, config, tracks, merged = merged )
    rhoP = makeRhoP( "rhoPFor" + name, config, tracks, pi0s, merged = merged )
    ### -------------------------------------------------------------------------
    combCuts = "(ADAMASS('B0') < %(BuResCombMassWindow)s)"\
               " & (APT > %(BuResMinPT)s)" % locals()["config"]
    
    motherCuts = "(SUMTREE('pi+'==ABSID,PT) > %(BuResSumTrPTMin)s)"\
                 " & (ADMASS('B+') < %(BuResMothMassWindow)s)"\
                 " & (MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))"\
                 " > %(BuResMaxTrIPChi2Min)s)"\
                 " & (MAXTREE('pi+'==ABSID,PT) > %(BuResMaxTrPTMin)s)"\
                 " & (BPVVDCHI2 > %(BuResMinFDChi2)s)"\
                 " & (MIPCHI2DV(PRIMARY) < %(BuResMaxIPChi2)s)"\
                 " & (BPVDIRA > %(BuResMinDira)s)"\
                 " & (VFASPF(VCHI2/VDOF) < %(BuResMinVChi2Dof)s)"\
                 % locals()["config"]
    ### -------------------------------------------------------------------------
    if merged:
        combCuts = "(AM > %(BuMgdCombMassMin)s) & (AM < %(BuMgdCombMassMax)s)"\
                   " & (APT > %(BuMgdMinPT)s)"\
                   % locals()["config"]
        motherCuts = "(MAXTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))"\
                     " > %(BuMgdMaxTrIPChi2Min)s)"\
                     " & (M > %(BuMgdMothMassMin)s) & (M < %(BuMgdMothMassMax)s)"\
                     " & (BPVVDCHI2 > %(BuMgdMinFDChi2)s)"\
                     " & (MIPCHI2DV(PRIMARY) < %(BuMgdMaxIPChi2)s)"\
                     " & (BPVDIRA > %(BuMgdMinDira)s)"\
                     " & (VFASPF(VCHI2/VDOF) < %(BuMgdMinVChi2Dof)s)"\
                     % locals()["config"]
    ### -------------------------------------------------------------------------
    if upstream:
        motherCuts = "(INTREE( HASTRACK & ISUP )) & " + motherCuts
    ### -------------------------------------------------------------------------
    Bu = CombineParticles( DecayDescriptor = "[B+ -> rho(770)0 rho(770)+]cc",
                           CombinationCut = combCuts,
                           MotherCut = motherCuts,
                           ReFitPVs = refit,
                           ParticleCombiners = { "" : "OfflineVertexFitter" } )
    ### -------------------------------------------------------------------------
    daughters = [ rho0, rhoP ]
    return ( Selection( name + "Sel", Algorithm = Bu, RequiredSelections = daughters ),
             daughters )

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
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def relInfoToolList( mother, daughters, prefix = '' ):
    return [ relInfoCVDict( angle = 1.70,
                            selections = [ mother ] + daughters,
                            recursion = 1),
             relInfoCVDict( angle = 1.35,
                            selections = [ mother ] + daughters,
                            recursion = 1),
             relInfoCVDict( angle = 1.00,
                            selections = [ mother ] + daughters,
                            recursion = 1),
             relInfoVtxDict( selections = [ mother ] + daughters, recursion = 1 )
             ]
