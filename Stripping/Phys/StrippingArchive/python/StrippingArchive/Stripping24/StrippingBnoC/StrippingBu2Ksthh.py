"""
Module for construction of B+/- -> K*+/-(892) hh', with K*(892) -> KS pi.
Provides functions to build KS->DD and KS->LL selections.
Provides class Bu2KsthhConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Exported symbols (use python help!):
   - Bu2KsthhConf
"""

__author__ = ['Rafael Coutinho']
__date__ = '10/11/2015'
__version__ = 'Stripping24'
__all__ = {'Bu2KsthhConf',
           'default_config'}

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdLoosePions as Pions

default_config = {
    'NAME'        : 'Bu2Ksthh',
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Bu2KsthhConf',
    'CONFIG'      : {'Trk_Long_Chi2'           : 4.0,
                     'Trk_Long_GhostProb'      : 0.4,
                     'Trk_Down_Chi2'           : 4.0,
                     'Trk_Down_GhostProb'      : 0.5,
                     'KS_DD_MassWindow'        : 30.0,
                     'KS_DD_VtxChi2'           : 12.0,
                     'KS_DD_FDChi2'            : 50.0,
                     'KS_LL_MassWindow'        : 20.0,
                     'KS_LL_VtxChi2'           : 12.0,
                     'KS_LL_FDChi2'            : 80.0,
                     'Kstar_MassLo'            : 0.0,
                     'Kstar_MassHi'            : 5000.0,
                     'B_Mlow'                  : 1279.0,
                     'B_Mhigh'                 : 921.0,
                     'B_APTmin'                : 1000.0,
                     'B_PTmin'                 : 1500.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_DD_maxDocaChi2'    : 5.0,
                     'BDaug_LL_maxDocaChi2'    : 5.0,
                     'BDaug_DD_PTsum'          : 3000.0,
                     'BDaug_LL_PTsum'          : 3000.0,
                     'B_DD_IPCHI2sum'          : 50.0,
                     'B_VtxChi2'               : 12.0,
                     'B_DD_Dira'               : 0.999,
                     'B_LL_Dira'               : 0.9999,
                     'B_DD_IPCHI2wrtPV'        : 8.0,
                     'B_LL_IPCHI2wrtPV'        : 8.0,
                     'B_DD_FDwrtPV'            : 1.0,
                     'B_LL_FDwrtPV'            : 1.0,
                     'B_DD_FDChi2'             : 50.0,
                     'B_LL_FDChi2'             : 50.0,
                     'GEC_MaxTracks'           : 250,
                     'HLT1Dec'                 : 'Hlt1(Two)?TrackMVADecision',
                     'HLT2Dec'                 : 'Hlt2Topo[234]BodyDecision',
                     'Prescale'                : 1.0,
                     'Postscale'               : 1.0
                     },
    'STREAMS' : ['Bhadron']
    }

class Bu2KsthhConf(LineBuilder) :
    """
    Builder of Bu->Ksthh stripping Selection and StrippingLine.
    Constructs B+/- -> Kst+/- h+ h- Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bu2ksthhConf = Bu2KsthhConf('Bu2KsthhTest',config)
    >>> bu2ksthhLines = bu2ksthhConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKS2DD               : KS -> Down Down Selection object
    selKS2LL               : KS -> Long Long Selection object
    selKst2KSDDpi          : Kst -> KS pi - Down Down Selection object
    selKst2KSLLpi          : Kst -> KS pi - Long Long Selection object
    selBu2KstDDhh          : Bu -> Kst(DD) h+ h- Selection object
    selBu2KstLLhh          : Bu -> Kst(LL) h+ h- Selection object
    dd_line                : StrippingLine made out of selBu2KstDDhh
    ll_line                : StrippingLine made out of selBu2KstLLhh
    lines                  : List of lines, [dd_line, ll_line]

    Exports as class data member:
    Bu2KsthhConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Long_Chi2',
                              'Trk_Long_GhostProb',
			      'Trk_Down_Chi2',
			      'Trk_Down_GhostProb',
                              'KS_DD_MassWindow',
                              'KS_DD_VtxChi2',
                              'KS_DD_FDChi2',
                              'KS_LL_MassWindow',
                              'KS_LL_VtxChi2',
                              'KS_LL_FDChi2',
                              'Kstar_MassLo', 
                              'Kstar_MassHi', 
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
                              'B_DD_IPCHI2sum',
                              'B_VtxChi2',
                              'B_DD_Dira',
                              'B_LL_Dira',
                              'B_DD_IPCHI2wrtPV',
                              'B_LL_IPCHI2wrtPV',
                              'B_DD_FDwrtPV',
                              'B_LL_FDwrtPV',
                              'B_DD_FDChi2',
                              'B_LL_FDChi2',
                              'GEC_MaxTracks',
                              'HLT1Dec',
                              'HLT2Dec',
                              'Prescale',
                              'Postscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        dd_name = name+'DD'
        ll_name = name+'LL'

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        Hlt1Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT1Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}
                          
        Hlt2Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT2Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}
                          
        self.pions = Pions

        self.makeKS2DD( 'KSfor'+dd_name, config )
        self.makeKS2LL( 'KSfor'+ll_name, config )

        self.makeKst2KSDDpi( 'Kstfor'+dd_name, config )
        self.makeKst2KSLLpi( 'Kstfor'+ll_name, config )

        self.makeBu2KstDDhh( dd_name, config )
        self.makeBu2KstLLhh( ll_name, config )

        self.dd_line = StrippingLine(dd_name+"Line",
                                     prescale  = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selBu2KstDDhh,
                                     HLT1 = Hlt1Filter,
                                     HLT2 = Hlt2Filter,
                                     FILTER = GECCode, 
                                     EnableFlavourTagging = True, 
                                     MDSTFlag = True
                                     )
        self.ll_line = StrippingLine(ll_name+"Line",
                                     prescale  = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selBu2KstLLhh,
                                     HLT1 = Hlt1Filter,
                                     HLT2 = Hlt2Filter,
                                     FILTER = GECCode,
                                     EnableFlavourTagging = True, 
                                     MDSTFlag = True
                                     )

        self.registerLine(self.dd_line)
        self.registerLine(self.ll_line)
        
    def makeKS2DD( self, name, config ) :
        # define all the cuts
        _massCut = "(ADMASS('KS0')<%s*MeV)" % config['KS_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2)<%s)"     % config['KS_DD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"         % config['KS_DD_FDChi2']
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Down_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Down_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_Down_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_Down_GhostProb']

        _allCuts = _massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_trkGhostProbCut1+'&'+_trkGhostProbCut2

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
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Long_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Long_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_Long_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_Long_GhostProb']

        _allCuts = _massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_trkGhostProbCut1+'&'+_trkGhostProbCut2

        # get the KS's to filter
        _stdKSLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )

        # make the filter
        _filterKSLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2LL = Selection( name, Algorithm = _filterKSLL, RequiredSelections = [_stdKSLL] )


    def makeKst2KSDDpi( self, name, config ) :
        # define all the cuts
        _massCutLo = "(AM > %s*MeV)" % config['Kstar_MassLo']
        _massCutHi = "(AM < %s*MeV)" % config['Kstar_MassHi']

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Long_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_Long_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut
      
        _allCuts = _massCutLo+'&'+_massCutHi

        # make the filter
        _filterKst2KSDDpi = CombineParticles()
        _filterKst2KSDDpi.DecayDescriptor = "[K*(892)+ -> KS0 pi+]cc"
        _filterKst2KSDDpi.CombinationCut = _allCuts 
        _filterKst2KSDDpi.DaughtersCuts = { "pi+" : _daughtersCuts }
        _filterKst2KSDDpi.MotherCut = "ALL"

        # make and store the Selection object
        self.selKst2KSDDpi = Selection( name, Algorithm = _filterKst2KSDDpi, RequiredSelections = [self.selKS2DD, self.pions] ) 

    def makeKst2KSLLpi( self, name, config ) :
        # define all the cuts
        _massCutLo = "(AM > %s*MeV)" % config['Kstar_MassLo']
        _massCutHi = "(AM < %s*MeV)" % config['Kstar_MassHi']

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Long_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_Long_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut
      
        _allCuts = _massCutLo+'&'+_massCutHi

        # make the filter
        _filterKst2KSLLpi = CombineParticles()
        _filterKst2KSLLpi.DecayDescriptor = "[K*(892)+ -> KS0 pi+]cc"
        _filterKst2KSLLpi.CombinationCut = _allCuts 
        _filterKst2KSLLpi.DaughtersCuts = { "pi+" : _daughtersCuts }
        _filterKst2KSLLpi.MotherCut = "ALL"

        # make and store the Selection object
        self.selKst2KSLLpi = Selection( name, Algorithm = _filterKst2KSLLpi, RequiredSelections = [self.selKS2LL, self.pions] ) 


    def makeBu2KstDDhh( self, name, config ) :
        """
        Create and store either a B+ -> Kst(DD)+ h+ h- Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Long_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_Long_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut

        _massCutLow       = "(AM>(5279-%s)*MeV)"                                                        % config['B_Mlow']
        _massCutHigh      = "(AM<(5279+%s)*MeV)"                                                        % config['B_Mhigh']
        _aptCut           = "(APT>%s*MeV)"                                                              % config['B_APTmin']
        _daugMedPtCut     = "(ANUM(PT>%s*MeV)>=2)"                                                      % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut   = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)"                                          % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut   = "(ACUTDOCACHI2(%s,''))"                                                     % config['BDaug_DD_maxDocaChi2']
        _daugPtSumCut     = "((APT1+APT2+APT3)>%s*MeV)"                                                 % config['BDaug_DD_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut            = "(PT>%s*MeV)"                                                               % config['B_PTmin']
        _vtxChi2Cut       = "(VFASPF(VCHI2)<%s)"                                                        % config['B_VtxChi2']
        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_DD_Dira']
        _ipChi2Cut        = "(MIPCHI2DV(PRIMARY)<%s)"                                                   % config['B_DD_IPCHI2wrtPV']
        _fdCut            = "(VFASPF(VMINVDDV(PRIMARY))>%s)"                                            % config['B_DD_FDwrtPV']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                                                            % config['B_DD_FDChi2']
        _ipChi2SumCut     = "(SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='pi+') | (ABSID=='pi-')),0.0) > %s)"  % config['B_DD_IPCHI2sum']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut+'&'+_ipChi2SumCut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        _B.DecayDescriptors = [ "[B+ -> pi+ pi- K*(892)+]cc" ]

        self.selBu2KstDDhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKst2KSDDpi, self.pions ])


    def makeBu2KstLLhh( self, name, config ) :
        """
        Create and store either a B+ -> Kst(LL)+ h+ h- Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Long_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_Long_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut

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
        _diraCut    = "(BPVDIRA>%s)"                   % config['B_LL_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_LL_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['B_LL_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_LL_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        _B.DecayDescriptors = [ "[B+ -> pi+ pi- K*(892)+]cc" ]

        self.selBu2KstLLhh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKst2KSLLpi, self.pions ])
