"""
Module for construction of B->KSKpi stripping Selections and StrippingLines.
Provides functions to build KS->DD, KS->LL, KS->LD abd B selections.
Provides class B2KSKpiConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Selections based on previous version B2KSKpi line but optimised for DP analysis.
Exported symbols (use python help!):
   - B2KSKpiConf
"""

__author__ = ['Thomas Latham','Rafael Coutinho', 'David Dossett']
__date__ = '15/03/2013'
__version__ = 'Stripping21'
__all__ = 'B2KSKpiConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdAllNoPIDsPions as Pions
from StandardParticles import StdAllNoPIDsKaons as Kaons

default_config = {'Trk_Chi2'                : 3.0,
                  'Trk_GhostProb'           : 0.3,
                  'KS_DD_MassWindow'        : 30.0,
                  'KS_DD_VtxChi2'           : 12.0,
                  'KS_DD_FDChi2'            : 50.0,
                  'KS_DD_Pmin'              : 6000.0,
                  'KS_LL_MassWindow'        : 20.0,
                  'KS_LL_VtxChi2'           : 12.0,
                  'KS_LL_FDChi2'            : 80.0,
                  'KS_LD_MassWindow'        : 25.0,
                  'KS_LD_VtxChi2'           : 12.0,
                  'KS_LD_FDChi2'            : 50.0,
                  'KS_LD_Pmin'              : 6000.0,
                  'B_Mlow'                  : 587.0,
                  'B_Mhigh'                 : 591.0,
                  'B_APTmin'                : 1000.0,
                  'BDaug_MedPT_PT'          : 800.0,
                  'BDaug_MaxPT_IP'          : 0.05,
                  'BDaug_DD_maxDocaChi2'    : 5.0,
                  'BDaug_LL_maxDocaChi2'    : 5.0,
                  'BDaug_LD_maxDocaChi2'    : 5.0,
                  'BDaug_DD_PTsum'          : 4200.0,
                  'BDaug_LL_PTsum'          : 3000.0,
                  'BDaug_LD_PTsum'          : 4200.0,
                  'B_DD_Dira'               : 0.,
                  'B_LL_Dira'               : 0.,
                  'B_LD_Dira'               : 0.,
                  'DD_BDTCutValue'          : -0.05,
                  'LD_BDTCutValue'          : -0.05,
                  'LL_BDTCutValue'          : -0.10,
                  'DD_BDTWeightsFile'       : '/afs/cern.ch/user/r/rsilvaco/public/forTom/B2KShhBDT_DD.xml',
                  'LD_BDTWeightsFile'       : '/afs/cern.ch/user/r/rsilvaco/public/forTom/B2KShhBDT_DD.xml', #same DD weight
                  'LL_BDTWeightsFile'       : '/afs/cern.ch/user/r/rsilvaco/public/forTom/B2KShhBDT_LL.xml',
                  'GEC_MaxTracks'           : 250,
                  'Prescale'                : 1.0,
                  'Prescale_SameSign'       : 1.0,
                  'Postscale'               : 1.0
                  }

class B2KSKpiConf(LineBuilder) :
    """
    Builder of B->KSKpi stripping Selection and StrippingLine.
    Constructs B -> KS K+ pi- Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bs2kskpiConf  = B2KSKpiConf('B2KSKpiTest',config)
    >>> bs2kskpiLines = bs2kskpiConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKS2DD               : KS -> Down Down Selection object
    selKS2LL               : KS -> Long Long Selection object
    selKS2LD               : KS -> Long Down Selection object
    selB2KSDDKpi           : B -> KS(DD) K+ pi- Selection object
    selB2KSLLKpi           : B -> KS(LL) K+ pi- Selection object
    selB2KSLDKpi           : B -> KS(LD) K+ pi- Selection object
    selB2KSDDKpi_SameSign  : B -> KS(DD) K+(-) pi+(-) Selection object
    selB2KSLLKpi_SameSign  : B -> KS(LL) K+(-) pi+(-) Selection object
    selB2KSLDKpi_SameSign  : B -> KS(LD) K+(-) pi+(-) Selection object
    dd_line                : StrippingLine made out of selB2KSDDKpi
    ll_line                : StrippingLine made out of selB2KSLLKpi
    ld_line                : StrippingLine made out of selB2KSLDKpi
    dd_line_same           : StrippingLine made out of selB2KSDDKpi_SameSign
    ll_line_same           : StrippingLine made out of selB2KSLLKpi_SameSign
    ld_line_same           : StrippingLine made out of selB2KSLDKpi_SameSign
    lines                  : List of lines, [dd_line, ll_line, ld_line, dd_line_same, ll_line_same, ld_line_same]

    Exports as class data member:
    B2KSKpiConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'Trk_GhostProb',
                              'KS_DD_MassWindow',
                              'KS_DD_VtxChi2',
                              'KS_DD_FDChi2',
                              'KS_DD_Pmin',
                              'KS_LL_MassWindow',
                              'KS_LL_VtxChi2',
                              'KS_LL_FDChi2',
                              'KS_LD_MassWindow',
                              'KS_LD_VtxChi2',
                              'KS_LD_FDChi2',
                              'KS_LD_Pmin',
                              'B_Mlow',
                              'B_Mhigh',
                              'B_APTmin',
                              'BDaug_MedPT_PT',
                              'BDaug_MaxPT_IP',
                              'BDaug_DD_maxDocaChi2',
                              'BDaug_LL_maxDocaChi2',
                              'BDaug_LD_maxDocaChi2',
                              'BDaug_DD_PTsum',
                              'BDaug_LL_PTsum',
                              'BDaug_LD_PTsum',
                              'B_DD_Dira',
                              'B_LL_Dira',
                              'B_LD_Dira',
                              'DD_BDTCutValue',
                              'LD_BDTCutValue',
                              'LL_BDTCutValue',
                              'DD_BDTWeightsFile',
                              'LD_BDTWeightsFile',
                              'LL_BDTWeightsFile',
                              'GEC_MaxTracks',
                              'Prescale',
                              'Prescale_SameSign',
                              'Postscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        dd_name = name+'DD'
        ll_name = name+'LL'
        ld_name = name+'LD'

        dd_name_same = name+'DD'+'SameSign'
        ll_name_same = name+'LL'+'SameSign'
        ld_name_same = name+'LD'+'SameSign'

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions
        self.kaons = Kaons

        self.makeKS2DD( 'KSfor'+dd_name, config )
        self.makeKS2LL( 'KSfor'+ll_name, config )
        self.makeKS2LD( 'KSfor'+ld_name, config )

        self.makeB2KSDDhh( dd_name+"_Presel", config )
        self.makeB2KSLLhh( ll_name+"_Presel", config )
        self.makeB2KSLDhh( ld_name+"_Presel", config )

        self.makeB2KSDDhh( dd_name_same+"_Presel", config )
        self.makeB2KSLLhh( ll_name_same+"_Presel", config )
        self.makeB2KSLDhh( ld_name_same+"_Presel", config )

        # MVA algorithm on the top of the Presel
        self.applyBDT( dd_name, config )
        self.applyBDT( ll_name, config )
        self.applyBDT( ld_name, config )

        self.applyBDT( dd_name_same, config )
        self.applyBDT( ll_name_same, config )
        self.applyBDT( ld_name_same, config )

        self.dd_line = StrippingLine(dd_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selB2KSDDhhBDTCut,
                                     HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                     FILTER = GECCode
                                     )
        self.ll_line = StrippingLine(ll_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selB2KSLLhhBDTCut,
                                     HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                     FILTER = GECCode
                                     )

        self.ld_line = StrippingLine(ld_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selB2KSLDhhBDTCut,
                                     HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                     FILTER = GECCode
                                     )
        
        self.dd_line_same = StrippingLine(dd_name_same+"Line",
                                     prescale = config['Prescale_SameSign'],
                                     postscale = config['Postscale'],
                                     selection = self.selB2KSDDhhBDTCut_SameSign,
                                     HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                     FILTER = GECCode
                                     )   
        self.ll_line_same = StrippingLine(ll_name_same+"Line",
                                     prescale = config['Prescale_SameSign'],
                                     postscale = config['Postscale'],
                                     selection =  self.selB2KSLLhhBDTCut_SameSign,
                                     HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                     FILTER = GECCode
                                     )
        
        self.ld_line_same = StrippingLine(ld_name_same+"Line",
                                     prescale = config['Prescale_SameSign'],
                                     postscale = config['Postscale'],
                                     selection =  self.selB2KSLDhhBDTCut_SameSign,
                                     HLT = "(HLT_PASS_RE('Hlt1TrackAllL0Decision') & HLT_PASS_RE('Hlt2Topo[234]Body.*Decision'))",
                                     FILTER = GECCode
                                     )
        
        self.registerLine(self.dd_line)
        self.registerLine(self.ll_line)
        self.registerLine(self.ld_line)
        
        self.registerLine(self.dd_line_same)
        self.registerLine(self.ll_line_same)
        self.registerLine(self.ld_line_same)


    def makeKS2DD( self, name, config ) :
        # define all the cuts
        _massCut = "(ADMASS('KS0')<%s*MeV)" % config['KS_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2)<%s)"     % config['KS_DD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"         % config['KS_DD_FDChi2']
        _momCut  = "(P>%s*MeV)"             % config['KS_DD_Pmin']
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']

        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2

        # get the KS's to filter
        _stdKSDD = DataOnDemand( Location = "Phys/StdLooseKsDD/Particles" )

        # make the filter
        _filterKSDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2DD = Selection( name, Algorithm = _filterKSDD, RequiredSelections = [_stdKSDD] )


    def makeKS2LL( self, name, config ) :
        # define all the cuts
        _massCut          = "(ADMASS('KS0')<%s*MeV)"         % config['KS_LL_MassWindow']
        _vtxCut           = "(VFASPF(VCHI2)<%s)"             % config['KS_LL_VtxChi2']
        _fdCut            = "(BPVVDCHI2>%s)"                 % config['KS_LL_FDChi2']
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_GhostProb']

        _allCuts = _massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_trkGhostProbCut1+'&'+_trkGhostProbCut2+'&'+_vtxCut+'&'+_fdCut

        # get the KS's to filter
        _stdKSLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )

        # make the filter
        _filterKSLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2LL = Selection( name, Algorithm = _filterKSLL, RequiredSelections = [_stdKSLL] )

    def makeKS2LD( self, name, config ) :
        # define all the cuts
        _massCut = "(ADMASS('KS0')<%s*MeV)" % config['KS_LD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2)<%s)"     % config['KS_LD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"         % config['KS_LD_FDChi2']
        _momCut  = "(P>%s*MeV)"             % config['KS_LD_Pmin']
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']

        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2

        # get the KS's to filter
        _stdKSLD = DataOnDemand( Location = "Phys/StdLooseKsLD/Particles" )

        # make the filter
        _filterKSLD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2LD = Selection( name, Algorithm = _filterKSLD, RequiredSelections = [_stdKSLD] )


    def makeB2KSDDhh( self, name, config ) :
        """
        Create and store either a B -> KS(DD) h+ h- Selection object, or a B -> KS(DD) h+(-) h+(-) Same Sign Selection Object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut

        _massCutLow       = "(AM>(5279-%s)*MeV)"                                                        % config['B_Mlow']
        _massCutHigh      = "(AM<(5279+%s)*MeV)"                                                        % config['B_Mhigh']
        _aptCut           = "(APT>%s*MeV)"                                                              % config['B_APTmin']
        _daugMedPtCut     = "(ANUM(PT>%s*MeV)>=2)"                                                      % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut   = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)"                                          % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut   = "(ACUTDOCACHI2(%s,''))"                                                     % config['BDaug_DD_maxDocaChi2']
        _daugPtSumCut     = "((APT1+APT2+APT3)>%s*MeV)"                                                 % config['BDaug_DD_PTsum']

        _combCuts = _daugPtSumCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_maxDocaChi2Cut+'&'+_aptCut+'&'+_daugMedPtCut+'&'+_daugMaxPtIPCut

        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_DD_Dira']

        _motherCuts = _diraCut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "SameSign" in name:
            _B.DecayDescriptors = [ "B_s0 -> K+ pi+ KS0", "B_s0 -> K- pi- KS0" ]

            self.selB2KSDDhh_SameSign = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2DD, self.pions, self.kaons ])

        else:
            _B.DecayDescriptors = [ "B_s0 -> K+ pi- KS0", "B_s0 -> pi+ K- KS0"]
            
            self.selB2KSDDhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2DD, self.pions, self.kaons ])


    def makeB2KSLLhh( self, name, config ) :
        """
        Create and store either a B -> KS(LL) h+ h- Selection object, or a B -> KS(LL) h+(-) h+(-) Same Sign Selection Object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut

        _massCutLow     = "(AM>(5279-%s)*MeV)"               % config['B_Mlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"               % config['B_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['B_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['BDaug_LL_PTsum']

        _combCuts = _daugPtSumCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_maxDocaChi2Cut+'&'+_aptCut+'&'+_daugMedPtCut+'&'+_daugMaxPtIPCut

        _diraCut    = "(BPVDIRA>%s)"                   % config['B_LL_Dira']

        _motherCuts = _diraCut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "SameSign" in name:
            _B.DecayDescriptors = [ "B_s0 -> K+ pi+ KS0", "B_s0 -> K- pi- KS0" ]

            self.selB2KSLLhh_SameSign = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2LL, self.pions, self.kaons ])

        else:
            _B.DecayDescriptors = [ "B_s0 -> K+ pi- KS0", "B_s0 -> pi+ K- KS0"]

            self.selB2KSLLhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2LL, self.pions, self.kaons ])

    def makeB2KSLDhh( self, name, config ) :
        """
        Create and store either a B -> KS(LD) h+ h- Selection object, or a B -> KS(LD) h+(-) h+(-) Same Sign Selection Object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut

        _massCutLow     = "(AM>(5279-%s)*MeV)"               % config['B_Mlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"               % config['B_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['B_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_LD_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['BDaug_LD_PTsum']

        _combCuts = _daugPtSumCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_maxDocaChi2Cut+'&'+_aptCut+'&'+_daugMedPtCut+'&'+_daugMaxPtIPCut

        _diraCut    = "(BPVDIRA>%s)"                   % config['B_LD_Dira']

        _motherCuts = _diraCut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "SameSign" in name:
            _B.DecayDescriptors = [ "B_s0 -> K+ pi+ KS0", "B_s0 -> K- pi- KS0"]

            self.selB2KSLDhh_SameSign = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2LD, self.pions, self.kaons ])

        else:
            _B.DecayDescriptors = [ "B_s0 -> K+ pi- KS0", "B_s0 -> pi+ K- KS0"]

            self.selB2KSLDhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2LD, self.pions, self.kaons ])
    
    def applyBDT( self, name, config):
        
        if "DD" in name:
            _FilterB2KShh = FilterDesktop( Code = "FILTER('B2KShhBDTSelection/B2KShhBDTDD')" )
        
            if "SameSign" in name:
                self.selB2KSDDhhBDTCut_SameSign = Selection( name,
                                    Algorithm = _FilterB2KShh,
                                    RequiredSelections = [ self.selB2KSLDhh_SameSign ]
                                    )
            else:    
                self.selB2KSDDhhBDTCut = Selection( name,
                                    Algorithm = _FilterB2KShh,
                                    RequiredSelections = [ self.selB2KSDDhh ]
                                    )
    
            """
            Name is special here, since this is the last algorithm,
            whose name seems to be the one of the stripping line....
            """
            from Configurables import B2KShhBDTSelection
    
            MyBDT = B2KShhBDTSelection( name + "Line.B2KShhBDTDD" )
            MyBDT.BDTCut = config["DD_BDTCutValue"]
            MyBDT.WeightsFile = config["DD_BDTWeightsFile"]
            MyBDT.typeKS = "DD"

        elif "LL" in name:
            _FilterB2KShh = FilterDesktop( Code = "FILTER('B2KShhBDTSelection/B2KShhBDTLL')" )
        
            if "SameSign" in name:
                self.selB2KSLLhhBDTCut_SameSign = Selection( name,
                                    Algorithm = _FilterB2KShh,
                                    RequiredSelections = [ self.selB2KSLLhh_SameSign ]
                                    )
            else:    
                self.selB2KSLLhhBDTCut = Selection( name,
                                    Algorithm = _FilterB2KShh,
                                    RequiredSelections = [ self.selB2KSLLhh ]
                                    )
    
            """
            Name is special here, since this is the last algorithm,
            whose name seems to be the one of the stripping line....
            """
            from Configurables import B2KShhBDTSelection
    
            MyBDT = B2KShhBDTSelection( name + "Line.B2KShhBDTLL" )
            MyBDT.BDTCut = config["LL_BDTCutValue"]
            MyBDT.WeightsFile = config["LL_BDTWeightsFile"]
            MyBDT.typeKS = "LL"

        elif "LD" in name:
            _FilterB2KShh = FilterDesktop( Code = "FILTER('B2KShhBDTSelection/B2KShhBDTLD')" )
        
            if "SameSign" in name:
                self.selB2KSLDhhBDTCut_SameSign = Selection( name,
                                    Algorithm = _FilterB2KShh,
                                    RequiredSelections = [ self.selB2KSLDhh_SameSign ]
                                    )
            else:
                self.selB2KSLDhhBDTCut = Selection( name,
                                    Algorithm = _FilterB2KShh,
                                    RequiredSelections = [ self.selB2KSLDhh ]
                                    )
    
            """
            Name is special here, since this is the last algorithm,
            whose name seems to be the one of the stripping line....
            """
            from Configurables import B2KShhBDTSelection
    
            MyBDT = B2KShhBDTSelection( name + "Line.B2KShhBDTLD" )
            MyBDT.BDTCut = config["LD_BDTCutValue"]
            MyBDT.WeightsFile = config["LD_BDTWeightsFile"]
            MyBDT.typeKS = "LD"
        
        else: print "\nHelp!! 'name' variable didn't have 'LL', 'LD' or 'DD' in it!\n"

