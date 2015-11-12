"""
Stripping selections for
    B+ -> K+ pi0
    B0 -> K0 pi0
using merged pi0s
"""

__author__  = "Jason E Andrews"
__date__    = "11/11/2015"
__version__ = "1.0"
__all__     = ( "default_config",
                "B2Kpi0Lines",
                "filterPi0",
                "filterK",
                "filterK0",
                "makeB2Kpi0Line",
                "relInfoCVDict",
                "relInfoNeutralCVDict",
                "relInfoVtxDict"
                )

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from StandardParticles import StdNoPIDsKaons, StdLooseMergedPi0, StdLooseKsLL

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder, checkConfig
from GaudiKernel.SystemOfUnits   import MeV
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
default_config = {
    "NAME"        : "B2Kpi0Lines",
    "WGs"         : [ "Charmless" ],
    "BUILDERTYPE" : "B2Kpi0Lines",
    "STREAMS"     : [ "BhadronCompleteEvent" ],
    "CONFIG" : { "checkPV" : True,
                 "L0Filter"      : None,#"L0_CHANNEL('Photon')|L0_CHANNEL('Electron')",
                 "pi0VoidFilter" : "(CONTAINS('Phys/StdLooseMergedPi0/Particles')>0)",
                 "pi0Config"     :
                 { "pi0PT_min"     : 3500*MeV,
                   "pi0P_min"      : 5000*MeV },
                 "K+Config"      :
                 { "K+PT_min"      : 1200*MeV,
                   "K+P_min"       : 12000*MeV,
                   "K+IPChi2_min"  : 50,
                   "K+PID_min"     : -0.5 },
                 "KS0Config"     :
                 { "KSPT_min"      : 500*MeV,
                   "KSP_min"       : 8000*MeV,
                   "KSMass_delta"  : 15*MeV,
                   "KSVChi2DOF_max": 15,
                   "KSIPChi2_min"  : 10 },
                 "B2Kpi0Config"  :
                 { "prescale"      : 1.0,
                   "Hlt1Filter"    : None,#"HLT_PASS('Hlt1TrackMVADecision')",
                   "Hlt2Filter"    : None,#"HLT_PASS('Hlt2B2Kpi0Decision')",
                   "BMass_min"     : 4000*MeV,
                   "BMass_max"     : 6200*MeV,
                   "ASumPT_min"    : 6500*MeV,
                   "BPT_min"       : 5000*MeV,
                   "MTDOCAChi2_max": 8.0 },
                 "B2K0pi0Config" :
                 { "prescale"      : 1.0,
                   "Hlt1Filter"    : None,#"HLT_PASS_RE('Hlt1(Two)?TrackMVADecision')",
                   "Hlt2Filter"    : None,#"HLT_PASS('Hlt2B2K0pi0Decision')",
                   "BMass_min"     : 4000*MeV,
                   "BMass_max"     : 6200*MeV,
                   "ASumPT_min"    : 5000*MeV,
                   "BPT_min"       : 4000*MeV,
                   "MTDOCAChi2_max": 10.0 },
                 }
    }
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class B2Kpi0Lines(LineBuilder) :
    __configuration_keys__ = ( "checkPV",
                               "L0Filter",
                               "pi0VoidFilter",
                               "pi0Config",
                               "K+Config",
                               "KS0Config",
                               "B2Kpi0Config",
                               "B2K0pi0Config" )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    def __init__(self, name, config) :
        self.name = name
        LineBuilder.__init__(self, name, config)
        ### Merged pi0's ::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        self.filteredPi0 = filterPi0( config["pi0Config"] )
        ### B2K+pi0 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        self.filteredK = filterK( config["K+Config"] )
        self.B2Kpi0Line = makeB2Kpi0Line("B2Kpi0",
                                         config,
                                         config["B2Kpi0Config"],
                                         "[B+ -> K+ pi0]cc",
                                         { "K+" : "ALL", "pi0" : "ALL" },
                                         { "[B+ -> ^K+ pi0]CC" : "K" },
                                         [ self.filteredK, self.filteredPi0 ] )
        self.registerLine( self.B2Kpi0Line )
        ### B2K0pi0 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        self.filteredKS0 = filterKS0( config["KS0Config"] )
        self.B2K0pi0Line = makeB2Kpi0Line("B2K0pi0",
                                          config,
                                          config["B2K0pi0Config"],
                                          "B0 -> KS0 pi0",
                                          { "KS0" : "ALL", "pi0" : "ALL" },
                                          { "B0 -> ^KS0 pi0" : "K0" },
                                         [ self.filteredKS0, self.filteredPi0 ] )
        self.registerLine( self.B2K0pi0Line )        
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def filterPi0( pi0Conf ) :
    
    __configuration_keys__ = ("pi0PT_min", "pi0P_min")
    checkConfig( __configuration_keys__, pi0Conf )
    
    particleFilter = FilterDesktop(
        Code = "(PT > %(pi0PT_min)s) & (P > %(pi0P_min)s)" % locals()["pi0Conf"] )
    return Selection("filteredPi0s", Algorithm = particleFilter,
                     RequiredSelections = [ StdLooseMergedPi0 ] )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def filterK( KConf ) :

    __configuration_keys__ = ("K+PT_min", "K+P_min", "K+IPChi2_min", "K+PID_min")
    checkConfig( __configuration_keys__, KConf )

    cutString = "(PT > %(K+PT_min)s) & (P > %(K+P_min)s)"\
                " & (MIPCHI2DV(PRIMARY) > %(K+IPChi2_min)s)"\
                " & (PIDK > %(K+PID_min)s)" % locals()["KConf"]
    particleFilter = FilterDesktop( Code = cutString )
    return Selection("filteredK", Algorithm = particleFilter,
                     RequiredSelections = [ StdNoPIDsKaons ] )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def filterKS0( KS0Conf ) :

    __configuration_keys__ = ("KSPT_min", "KSP_min", "KSMass_delta",
                              "KSVChi2DOF_max", "KSIPChi2_min")
    checkConfig( __configuration_keys__, KS0Conf )

    cutString = "(PT > %(KSPT_min)s) & (P > %(KSP_min)s)"\
                " & (ADMASS('KS0') < %(KSMass_delta)s)"\
                " & (VFASPF(VCHI2PDOF) < %(KSVChi2DOF_max)s)"\
                " & (MIPCHI2DV(PRIMARY) > %(KSIPChi2_min)s)" % locals()["KS0Conf"]
    particleFilter = FilterDesktop( Code = cutString )
    return Selection("filteredKS0", Algorithm = particleFilter,
                     RequiredSelections = [ StdLooseKsLL ] )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeB2Kpi0Line( name,
                    conf,
                    lineConf,
                    decayDescriptor,
                    daughtersCuts,
                    daughterIsoDicts,
                    inputs ) :

    __configuration_keys__ = ( "prescale",
                               "Hlt1Filter",
                               "Hlt2Filter",
                               "BMass_min",
                               "BMass_max",
                               "ASumPT_min",
                               "BPT_min",
                               "MTDOCAChi2_max")
    checkConfig( __configuration_keys__, lineConf )
    
    _combCuts = "(in_range(%(BMass_min)s, AM, %(BMass_max)s))"\
                " & (ASUM(PT)> %(ASumPT_min)s)" % locals()["lineConf"]
    _motherCuts = "(PT>%(BPT_min)s)"\
                  " & (MTDOCACHI2(1)<%(MTDOCAChi2_max)s)" % locals()["lineConf"]
    combiner = CombineParticles( DecayDescriptor = decayDescriptor,
                                 MotherCut = _motherCuts,
                                 CombinationCut = _combCuts,
                                 DaughtersCuts = daughtersCuts,
                                 ParticleCombiners = { "" : "ParticleAdder" } )

    selector = Selection( name + "Selection", Algorithm = combiner,
                          RequiredSelections = inputs )

    return StrippingLine(
        name + "Line",
        prescale = lineConf["prescale"],
        L0DU = conf["L0Filter"],
        HLT1 = lineConf["Hlt1Filter"],
        HLT2 = lineConf["Hlt2Filter"],
        FILTER = conf["pi0VoidFilter"],
        checkPV = conf["checkPV"],
        RequiredRawEvents = [ "Calo" ],
        selection = selector,
        RelatedInfoTools = [ relInfoCVDict( 1.7,  daughterIsoDicts ),
                             relInfoCVDict( 1.35, daughterIsoDicts ),
                             relInfoCVDict( 1.0,  daughterIsoDicts ),
                             relInfoVtxDict( daughterIsoDicts ),
                             relInfoNeutralCVDict( 1.7,  daughterIsoDicts ),
                             relInfoNeutralCVDict( 1.35, daughterIsoDicts ),
                             relInfoNeutralCVDict( 1.0,  daughterIsoDicts ),
                             ]
        )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def relInfoCVDict( angle = 1, DauDict = {} ):
    ModifiedDauDict = DauDict.copy()
    for flag, name in ModifiedDauDict.iteritems():
        ModifiedDauDict[flag] = 'maps/coneIso_' + name + '/' + str(angle)
    return { 'Type'             : 'RelInfoConeVariables',
             'ConeAngle'        : angle,
             'Variables'        : [ 'CONEANGLE', 'CONEMULT',
                                    'CONEP', 'CONEPASYM',
                                    'CONEPT', 'CONEPTASYM' ],
             'DaughterLocations': ModifiedDauDict,
             'Location'         : 'maps/coneIso_B/' + str(angle) }
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def relInfoNeutralCVDict( angle = 1, DauDict = {} ):
    ModifiedDauDict = DauDict.copy()
    for flag, name in ModifiedDauDict.iteritems():
        ModifiedDauDict[flag] = 'maps/neutralConeIso_' + name + '/' + str(angle)
    return { 'Type'             : 'RelInfoConeVariablesForEW',
             'ConeAngle'        : angle,
             'Variables'        : [ 'EWCONEANGLE', 'EWCONENMULT',
                                    'EWCONENVP',   'EWCONENSP',
                                    'EWCONENVPT',  'EWCONENSPT',
                                    'EWCONENPX', 'EWCONENPY', 'EWCONENPZ' ],
             'DaughterLocations': ModifiedDauDict,
             'Location'         : 'maps/neutralConeIso_B/' + str(angle) }
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def relInfoVtxDict( DauDict = {} ):
    ModifiedDauDict = DauDict.copy()
    for flag, name in ModifiedDauDict.iteritems():
        ModifiedDauDict[flag] = 'maps/vtxIso_' + name
    return { 'Type'             : 'RelInfoVertexIsolation',
             'Variables'        : [ 'VTXISONUMVTX',
                                    'VTXISODCHI2ONETRACK', 'VTXISODCHI2MASSONETRACK',
                                    'VTXISODCHI2TWOTRACK', 'VTXISODCHI2MASSTWOTRACK',
                                    ],
             'DaughterLocations': ModifiedDauDict,
             'Location'         : 'maps/vtxIso_B' }
