"""
Update of StrippingBu2Kpi0 from S20rXp2
rewritten for Stripping21
lines for both resolved and merged pi0's
"""

__author__  = "Jason E Andrews"
__date__    = "31/07/2014"
__version__ = "2.0"
__all__     = ( "default_config",
                "StrippingBu2Kpi0Conf" )

from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables import FilterDesktop
from StandardParticles import StdNoPIDsKaons, StdLooseMergedPi0, StdLooseResolvedPi0

from PhysSelPython.Wrappers      import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
default_config = {
    "NAME"        : "Bu2Kpi0",
    "WGs"         : [ "Charmless" ],
    "BUILDERTYPE" : "StrippingBu2Kpi0Conf",
    "STREAMS"     : { "Bhadron" : [ "StrippingBu2Kpi0MergedLine",
                                    "StrippingBu2Kpi0ResolvedLine" ] },
    "CONFIG"      : { "TISTOSSpecs" : { 'Hlt1Track(AllL0|Photon)Decision%TOS' : 0 },
                      "isMC"             : False, ### True = no Hlt filter
                      "refitPVs"         : True,
                      
                      "vetoTrISMUON"     : True,
                      "trMaxChi2Dof"     : 3.0,
                      "trMaxGhostProb"   : 0.5,
                      "trMinProbNNk"     : 0.08,
                      "trMinPTasy"       : -0.7,  # unitless
                      
                      "MKuMIPCHI2DV"     : 30,
                      "MKuMinP"          : 8000,  # MeV
                      "MKuMinPT"         : 1100,  # MeV
                      "Mpi0MinP"         : 0,     # MeV
                      "Mpi0MinPT"        : 2400,  # MeV
                      "Mpi0MinCL"        :-1000,
                      "MBuMinM"          : 4200,  # MeV
                      "MBuMaxM"          : 6300,  # MeV
                      "MBuMinPT"         : 1800,  # MeV
                      "MBuMaxMTDOCAChi2" : 10,
                      "PrescaleBu2Kpi0Merged"   : 1.0,
                      
                      "RKuMIPCHI2DV"     : 25,
                      "RKuMinPT"         : 1300,  # MeV
                      "Rpi0MinP"         : 8000,  # MeV
                      "Rpi0MinPT"        : 1400,  # MeV
                      "RgammaMinCL"      :-1000,
                      "RBuMinM"          : 4800,  # MeV
                      "RBuMaxM"          : 5800,  # MeV
                      "RBuMinPT"         : 0,     # MeV
                      "RBuMaxMTDOCAChi2" : 7,
                      "PrescaleBu2Kpi0Resolved" : 1.0
                      }
    }

### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class StrippingBu2Kpi0Conf(LineBuilder) :
    __configuration_keys__ = ( "TISTOSSpecs",
                               "isMC",
                               "refitPVs",
                               
                               "vetoTrISMUON",
                               "trMaxChi2Dof",
                               "trMaxGhostProb",
                               "trMinProbNNk",
                               "trMinPTasy",

			       "MKuMIPCHI2DV",
                               "MKuMinP",
                               "MKuMinPT",
			       "Mpi0MinP",
			       "Mpi0MinPT",
			       "Mpi0MinCL",
                               "MBuMinM",
                               "MBuMaxM",
                               "MBuMinPT",
                               "MBuMaxMTDOCAChi2",
                               "PrescaleBu2Kpi0Merged",
                               
                               "RKuMIPCHI2DV",
                               "RKuMinPT",
			       "Rpi0MinP",
			       "Rpi0MinPT",
			       "RgammaMinCL",
			       "RBuMinM",
			       "RBuMaxM",
                               "RBuMinPT",
                               "RBuMaxMTDOCAChi2",
			       "PrescaleBu2Kpi0Resolved" )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    def __init__(self, name, config) :
        self.name = name
        LineBuilder.__init__(self, name, config)
        modeName = "Bu2Kpi0"
        ### filters :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        mgdVoidFilter = "(CONTAINS('Phys/StdLooseMergedPi0/Particles')>0)"
        resVoidFilter = "(CONTAINS('Phys/StdLooseResolvedPi0/Particles')>0)"

        mgdHltFilter = None
        resHltFilter = None
        if not config["isMC"]:
            hlt1Filter = "HLT_PASS_RE('Hlt1Track(AllL0|Photon)Decision')"
            mgdHltFilter = hlt1Filter
            resHltFilter = hlt1Filter
        ### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::               
        trackCuts = ""
        if config["vetoTrISMUON"]:
            trackCuts += "(~ISMUON) &"
        trackCuts += "(TRCHI2DOF < %(trMaxChi2Dof)s)"\
                     " & (TRGHOSTPROB < %(trMaxGhostProb)s)"\
                     " & (PROBNNk > %(trMinProbNNk)s)"\
                     " & ( ((PT - ptC1) / (ptC1 + PT)) > %(trMinPTasy)s)"\
                     % locals()["config"]
        trackFilter = FilterDesktop(
            "trackFilter",
            Preambulo = [ "ptC1 = SUMCONE( 1.2, PT,"\
                          " '/Event/Phys/StdNoPIDsPions/Particles' )" ],
            Code = trackCuts )
        
        filteredKaons = Selection(
            "FilteredKaons",
            Algorithm = trackFilter,
            RequiredSelections = [ StdNoPIDsKaons ] )
        ### merged ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        selMerged = makeBu2Kpi0Merged(
            modeName + "Merged",
            config,
            DecayDescriptor = "[B+ -> K+ pi0]cc",
            inputSel = [ filteredKaons, StdLooseMergedPi0 ],
            refit = config["refitPVs"] )

        self.Bu2Kpi0Merged_line = StrippingLine(
            modeName + "MergedLine",
            prescale = config["PrescaleBu2Kpi0Merged"],
            HLT = mgdHltFilter,
            FILTER = mgdVoidFilter,
            checkPV = True,
            RelatedInfoTools = [ relInfoCVDict( angle = 1.7,
                                                selections = [selMerged],
                                                recursion = 0),
                                 relInfoCVDict( angle = 1.35,
                                                selections = [selMerged],
                                                recursion = 0),
                                 relInfoCVDict( angle = 1.0,
                                                selections = [selMerged],
                                                recursion = 0),
                                 relInfoVtxDict( selections = [selMerged],
                                                 recursion = 0 ) ],
            RequiredRawEvents = [ 'Calo' ],
            selection = selMerged )
        
        self.registerLine( self.Bu2Kpi0Merged_line )
        ### resolved ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	selResolved = makeBu2Kpi0Resolved(
            modeName + "Resolved",
            config,
            DecayDescriptor = "[B+ -> K+ pi0]cc",
            inputSel = [ filteredKaons, StdLooseResolvedPi0 ],
            refit = config["refitPVs"] )
        
        self.Bu2Kpi0Resolved_line = StrippingLine(
            modeName + "ResolvedLine",
            prescale = config["PrescaleBu2Kpi0Resolved"],
            HLT = resHltFilter,
            FILTER = resVoidFilter,
            checkPV = True,
            RelatedInfoTools = [ relInfoCVDict( angle = 1.7,
                                                selections = [selResolved],
                                                recursion = 0),
                                 relInfoCVDict( angle = 1.35,
                                                selections = [selResolved],
                                                recursion = 0),
                                 relInfoCVDict( angle = 1.0,
                                                selections = [selResolved],
                                                recursion = 0),
                                 relInfoVtxDict( selections = [selResolved],
                                                 recursion = 0 ) ],
            RequiredRawEvents = [ 'Calo' ],
            selection = selResolved )

        self.registerLine( self.Bu2Kpi0Resolved_line )
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeBu2Kpi0Resolved( name,
                         config,
                         DecayDescriptor,
                         inputSel,
                         refit = True ) :

    _KuCuts = "(MIPCHI2DV(PRIMARY)>%(RKuMIPCHI2DV)s) & (PT>%(RKuMinPT)s)"\
              % locals()["config"]
    _pi0Cuts = "(P>%(Rpi0MinP)s *MeV) & (PT>%(Rpi0MinPT)s *MeV)"\
               " & (CHILD(CL,1)>%(RgammaMinCL)s)"\
               " & (CHILD(CL,2)>%(RgammaMinCL)s)"% locals()["config"]
    _daughterCuts = { "K+" : _KuCuts, "pi0" : _pi0Cuts }
    _combCuts = "(AM>%(RBuMinM)s *MeV) & (AM<%(RBuMaxM)s *MeV)" % locals()["config"]
    _motherCuts = "(MTDOCACHI2(1) < %(RBuMaxMTDOCAChi2)s)"\
                  " & (PT>%(RBuMinPT)s *MeV)" % locals()["config"]

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts,
                           ParticleCombiners = { "" : "ParticleAdder" },
                           ReFitPVs = refit )

    _selB = Selection( name + "Sel",
                       Algorithm = _B,
                       RequiredSelections = inputSel )
    if not config["isMC"]:
        _selB = makeTISTOSSelection( _selB, config["TISTOSSpecs"] )

    return _selB
### :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
def makeBu2Kpi0Merged( name,
                       config,
                       DecayDescriptor,
                       inputSel,
                       refit = True ) :

    _KuCuts = "(MIPCHI2DV(PRIMARY)>%(MKuMIPCHI2DV)s) & (P>%(MKuMinP)s)"\
              " & (PT>%(MKuMinPT)s)" % locals()["config"]
    _pi0Cuts = "(P>%(Mpi0MinP)s *MeV) & (PT>%(Mpi0MinPT)s *MeV)"\
               " & (CL>%(Mpi0MinCL)s)" % locals()["config"]
    _daughterCuts = { "K+" : _KuCuts, "pi0" : _pi0Cuts }
    _combCuts = "(AM>%(MBuMinM)s *MeV) & (AM<%(MBuMaxM)s *MeV)" % locals()["config"]
    _motherCuts = "(MTDOCACHI2(1) < %(MBuMaxMTDOCAChi2)s)"\
                  " & (PT>%(MBuMinPT)s *MeV)" % locals()["config"]

    _B = CombineParticles( DecayDescriptor = DecayDescriptor,
                           MotherCut = _motherCuts,
                           CombinationCut = _combCuts,
                           DaughtersCuts = _daughterCuts,
                           ParticleCombiners = { "" : "ParticleAdder" },
                           ReFitPVs = refit )

    _selB = Selection( name + "Sel",
                       Algorithm = _B,
                       RequiredSelections = inputSel )
    if not config["isMC"]:
        _selB = makeTISTOSSelection( _selB, config["TISTOSSpecs"] )
        
    return _selB
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
def makeTISTOSSelection( selection, Specs ):
    from Configurables import TisTosParticleTagger
    myTagger = TisTosParticleTagger('myTagger')
    myTagger.TisTosSpecs = Specs
    # ### :::::: to speed things up:
    # myTagger.ProjectTracksToCalo = False
    # myTagger.CaloClustForCharged = False
    # myTagger.CaloClustForNeutral = False
    # myTagger.TOSFrac = {4:0.0, 5:0.0}
    return Selection( selection.name() + "TISTOS",
                      Algorithm = myTagger,
                      RequiredSelections = [ selection ]
                      )
