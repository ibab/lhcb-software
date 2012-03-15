"""
Module for construction of Bd/Bs->KShh stripping Selections and StrippingLines.
Provides functions to build KS->DD, KS->LL, Bd/Bs selections.
Provides class B2KShhConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Selections based on previous version of the line by Jussara Miranda.
Exported symbols (use python help!):
   - B2KShhConf
"""

__author__ = ['Thomas Latham','David Dossett','Jussara Miranda','Rafael Coutinho']
__date__ = '15/03/2012'
__version__ = 'Stripping18'
__all__ = 'B2KShhConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdLoosePions as Pions
from StandardParticles import StdLooseKaons as Kaons
from StandardParticles import StdLooseProtons as Protons

default_config = {'Trk_Chi2'                : 4.0,
                  'KS_DD_MassWindow'        : 30.0,
                  'KS_DD_VtxChi2'           : 12.0,
                  'KS_DD_FDChi2'            : 50.0,
                  'KS_DD_Pmin'              : 6000.0,
                  'KS_LL_MassWindow'        : 20.0,
                  'KS_LL_VtxChi2'           : 12.0,
                  'KS_LL_FDChi2'            : 80.0,
                  'B_Mlow'                  : 500.0,
                  'B_Mhigh'                 : 587.0,
                  'B_APTmin'                : 1000.0,
                  'B_PTmin'                 : 1500.0,
                  'BDaug_MedPT_PT'          : 800.0,
                  'BDaug_MaxPT_IP'          : 0.05,
                  'BDaug_DD_maxDocaChi2'    : 5.0,
                  'BDaug_LL_maxDocaChi2'    : 5.0,
                  'BDaug_DD_PTsum'          : 3000.0,
                  'BDaug_LL_PTsum'          : 3000.0,
                  'B_VtxChi2'               : 12.0,
                  'B_Dira'                  : 0.9999,
                  'B_DD_IPCHI2wrtPV'        : 8.0,
                  'B_LL_IPCHI2wrtPV'        : 8.0,
                  'B_FDwrtPV'               : 1.0,
                  'B_DD_FDChi2'             : 50.0,
                  'B_LL_FDChi2'             : 50.0,
                  'GEC_MaxTracks'           : 250,
                  'Prescale'                : 1.0,
                  'Postscale'               : 1.0
                  }

class B2KShhConf(LineBuilder) :
    """
    Builder of B->KShh stripping Selection and StrippingLine.
    Constructs B -> KS h+ h- Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> b2kshhConf = B2KShhConf('B2KShhTest',config)
    >>> b2kshhLines = b2kshhConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKS2DD               : KS -> Down Down Selection object
    selKS2LL               : KS -> Long Long Selection object
    selB2KSDDhh            : B -> KS(DD) h+ h- Selection object
    selB2KSLLhh            : B -> KS(LL) h+ h- Selection object
    dd_line                : StrippingLine made out of selB2KSDDhh
    ll_line                : StrippingLine made out of selB2KSLLhh
    lines                  : List of lines, [dd_line, ll_line]

    Exports as class data member:
    B2KShhConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'KS_DD_MassWindow',
                              'KS_DD_VtxChi2',
                              'KS_DD_FDChi2',
                              'KS_DD_Pmin',
                              'KS_LL_MassWindow',
                              'KS_LL_VtxChi2',
                              'KS_LL_FDChi2',
                              'B_Mlow',
                              'B_Mhigh',
                              'B_APTmin',
                              'B_PTmin',
                              'BDaug_MedPT_PT',
                              'BDaug_MaxPT_IP',
                              'BDaug_DD_maxDocaChi2',
                              'BDaug_LL_maxDocaChi2',
                              'BDaug_DD_PTsum',
                              'BDaug_LL_PTsum',
                              'B_VtxChi2',
                              'B_Dira',
                              'B_DD_IPCHI2wrtPV',
                              'B_LL_IPCHI2wrtPV',
                              'B_FDwrtPV',
                              'B_DD_FDChi2',
                              'B_LL_FDChi2',
                              'GEC_MaxTracks',
                              'Prescale',
                              'Postscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        dd_name = name+'DD'
        ll_name = name+'LL'

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions
        self.kaons = Kaons
        self.protons = Protons

        self.hadrons = MergedSelection("HadronsFor" + name,
                                       RequiredSelections = [ self.pions, self.kaons, self.protons ] )
        

        self.makeKS2DD( 'KSfor'+dd_name, config )
        self.makeKS2LL( 'KSfor'+ll_name, config )

        self.makeB2KSDDhh( dd_name, config )
        self.makeB2KSLLhh( ll_name, config )

        self.dd_line = StrippingLine(dd_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selB2KSDDhh,
                                     FILTER = GECCode
                                     )
        self.ll_line = StrippingLine(ll_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selB2KSLLhh,
                                     FILTER = GECCode
                                     )

        self.registerLine(self.dd_line)
        self.registerLine(self.ll_line)

    def makeKS2DD( self, name, config ) :
        # define all the cuts
        _massCut = "(ADMASS('KS0')<%s*MeV)" % config['KS_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2)<%s)"     % config['KS_DD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"         % config['KS_DD_FDChi2']
        _momCut  = "(P>%s*MeV)"             % config['KS_DD_Pmin']
        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut

        # get the KS's to filter
        _stdKSDD = DataOnDemand( Location = "Phys/StdLooseKsDD/Particles" )

        # make the filter
        _filterKSDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2DD = Selection( name, Algorithm = _filterKSDD, RequiredSelections = [_stdKSDD] )

    def makeKS2LL( self, name, config ) :
        # define all the cuts
        _massCut    = "(ADMASS('KS0')<%s*MeV)" % config['KS_LL_MassWindow']
        _vtxCut     = "(VFASPF(VCHI2)<%s)"     % config['KS_LL_VtxChi2']
        _fdCut      = "(BPVVDCHI2>%s)"         % config['KS_LL_FDChi2']
        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))" % config['Trk_Chi2']
        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))" % config['Trk_Chi2']
        _allCuts = _massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_vtxCut+'&'+_fdCut

        # get the KS's to filter
        _stdKSLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )

        # make the filter
        _filterKSLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2LL = Selection( name, Algorithm = _filterKSLL, RequiredSelections = [_stdKSLL] )

    def makeB2KSDDhh( self, name, config ) :
        """
        Create and store a B -> KS(DD) h+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5279-%s)*MeV)"               % config['B_Mlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"               % config['B_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['B_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_DD_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['BDaug_DD_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['B_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['B_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['B_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_DD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['B_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_DD_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _B = CombineParticles()
        _B.DecayDescriptors = [ "B0 -> pi+ pi- KS0", \
                                "B0 -> K+ pi- KS0", "B0 -> pi+ K- KS0", \
                                "B0 -> K+ K- KS0", \
                                "B0 -> p+ p~- KS0" ]
        _B.DaughtersCuts = { "K+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "p+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        self.selB2KSDDhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2DD, self.hadrons ])

    def makeB2KSLLhh( self, name, config ) :
        """
        Create and store a B -> KS(LL) h+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5279-%s)*MeV)"               % config['B_Mlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"               % config['B_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['B_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['BDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['BDaug_LL_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['B_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['B_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['B_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_LL_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['B_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_LL_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _B = CombineParticles()
        _B.DecayDescriptors = [ "B0 -> pi+ pi- KS0", \
                                "B0 -> K+ pi- KS0", "B0 -> pi+ K- KS0", \
                                "B0 -> K+ K- KS0", \
                                "B0 -> p+ p~- KS0"]
        _B.DaughtersCuts = { "K+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "p+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        self.selB2KSLLhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2LL, self.hadrons ])

