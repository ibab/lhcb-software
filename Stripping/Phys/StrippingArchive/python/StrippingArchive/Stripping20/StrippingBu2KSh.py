"""
Module for construction of Bu->KSh stripping Selections and StrippingLines.
Provides functions to build KS->DD, KS->LL, Bu selections.
Provides class Bu2KShConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Exported symbols (use python help!):
   - Bu2KShConf
This is freely inspired for Hb2Charged2Body and B2KShh packages...
"""

__author__ = ['Aurelien Martens']
__date__ = '31/05/2012'
__version__ = 'Stripping18'
__all__ = 'Bu2KShConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Beauty2Charm_LoKiCuts import LoKiCuts

from StandardParticles import StdNoPIDsPions as Pions

default_config = { 'KSDaug_LTrk_GhostProb'   : 0.5,
                   'Trk_GhostProb'           : 0.5,
                   'Trk_Chi2'                : 3.0,
                   'KS_DD_MassWindow'        : 30.0,
                   'KS_LL_MassWindow'        : 15.0,
                   'KS_DD_VtxChi2'           : 10.0,
                   'KS_LL_VtxChi2'           : 10.0,
                   'KS_DD_FDChi2'            : 50.0,
                   'KS_LL_FDChi2'            : 100.0,
                   'KS_DD_Pmin'              : 8000.0,
                   'KS_LL_Pmin'              : 8000.0,
                   'KS_DD_Ptmin'             : 1000.0,
                   'KS_LL_Ptmin'             : 800.0,
                   'Bach_Ptmin'              : 1000.0,
                   'BDaug_MaxPT_IP'          : 0.05,
                   'BDaug_DD_PTsum'          : 4000.0,
                   'BDaug_LL_PTsum'          : 4000.0,
                   'B_Mlow'                  : 500.0,
                   'B_Mhigh'                 : 500.0,
                   'B_Pmin'                  : 25000.0,
                   'B_LL_VtxChi2'            : 6.0,
                   'B_DD_VtxChi2'            : 6.0,
                   'B_Dira'                  : 0.9995,
                   'B_DD_IPCHI2wrtPV'        : 10.0,
                   'B_LL_IPCHI2wrtPV'        : 10.0,
                   'B_FDwrtPV'               : 1.0,
                   'B_DD_FDChi2'             : 50,
                   'B_LL_FDChi2'             : 50,
                   'GEC_MaxTracks'           : 250,
                   'Prescale'                : 1.0,
                   'Postscale'               : 1.0
                   }

class Bu2KShConf(LineBuilder) :
    """
    Builder of Bu->KSh stripping Selection and StrippingLine.
    Constructs B+ -> KS h+ Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> bu2kshConf = Bu2KShConf('Bu2KShTest',config)
    >>> bu2kshLines = bu2kshConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKS2DD               : KS -> Down Down Selection object
    selKS2LL               : KS -> Long Long Selection object
    selBu2KSPiDD            : Bu -> KS(DD) pi+ Selection object
    selBu2KSPiLL            : Bu -> KS(LL) pi+ Selection object
    selBu2KSKDD            : Bu -> KS(DD) K+ Selection object
    selBu2KSKLL            : Bu -> KS(LL) K+ Selection object 
    dd_line                : StrippingLine made out of selB2KSDDhh
    ll_line                : StrippingLine made out of selBu2KSLLh
    lines                  : List of lines, [dd_line, ll_line]

    Exports as class data member:
    Bu2KShConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('KSDaug_LTrk_GhostProb',
                              'Trk_GhostProb',
                              'Trk_Chi2',
                              'KS_DD_MassWindow',
                              'KS_LL_MassWindow',
                              'KS_DD_VtxChi2',
                              'KS_LL_VtxChi2',
                              'KS_DD_FDChi2',
                              'KS_LL_FDChi2',
                              'KS_DD_Pmin',
                              'KS_LL_Pmin',
                              'KS_DD_Ptmin',
                              'KS_LL_Ptmin',
                              'Bach_Ptmin',
                              'BDaug_MaxPT_IP',
                              'BDaug_DD_PTsum',
                              'BDaug_LL_PTsum',
                              'B_Mlow',
                              'B_Mhigh',
                              'B_Pmin',
                              'B_LL_VtxChi2',
                              'B_DD_VtxChi2',
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
        Bu2KSPiDDName = dd_name.replace("Bu2KSh","Bu2KSPi")
        Bu2KSPiLLName = ll_name.replace("Bu2KSh","Bu2KSPi")
        Bu2KSKDDName = dd_name.replace("Bu2KSh","Bu2KSK")
        Bu2KSKLLName = ll_name.replace("Bu2KSh","Bu2KSK")

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions

        self.makeKS2DD( 'KSfor'+dd_name, config )
        self.makeKS2LL( 'KSfor'+ll_name, config )

        self.makeH( 'Hfor'+name, config )

        self.Bu2KSPiDD = self.makeBu2KSDDh( Bu2KSPiDDName, config )
        self.Bu2KSPiLL = self.makeBu2KSLLh( Bu2KSPiLLName, config )

        self.Bu2KSPiDDLine = StrippingLine(Bu2KSPiDDName+"Line",
                                           prescale = config['Prescale'],
                                           postscale = config['Postscale'],
                                           selection = self.Bu2KSPiDD,
                                           FILTER = GECCode
                                           )
        
        self.Bu2KSPiLLLine = StrippingLine(Bu2KSPiLLName+"Line",
                                           prescale = config['Prescale'],
                                           postscale = config['Postscale'],
                                           selection =  self.Bu2KSPiLL,
                                           FILTER = GECCode
                                           )

        self.registerLine(self.Bu2KSPiDDLine)
        self.registerLine(self.Bu2KSPiLLLine)

    def makeKS2DD( self, name, config ) :
        # define all the cuts
        _massCut = "(ADMASS('KS0')<%s*MeV)" % config['KS_DD_MassWindow']
        _vtxCut  = "(VFASPF(VCHI2)<%s)"     % config['KS_DD_VtxChi2']
        _fdCut   = "(BPVVDCHI2>%s)"         % config['KS_DD_FDChi2']
        _momCut  = "(P>%s*MeV)"             % config['KS_DD_Pmin']
        _ptCut   = "(PT>%s*MeV)"            % config['KS_DD_Ptmin']
        _allCuts = _momCut+'&'+_ptCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut

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
        _momCut     = "(P>%s*MeV)"             % config['KS_DD_Pmin']
        _ptCut      = "(PT>%s*MeV)"            % config['KS_DD_Ptmin']
        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))" % config['Trk_Chi2']
        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))" % config['Trk_Chi2']
        _trkGPCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['KSDaug_LTrk_GhostProb']
        _trkGPCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['KSDaug_LTrk_GhostProb']
        _allCuts = _trkGPCut1 + '&' + _trkGPCut2 + '&' + _momCut+'&'+_ptCut+'&'+_massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_vtxCut+'&'+_fdCut

        # get the KS's to filter
        _stdKSLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )

        # make the filter
        _filterKSLL = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2LL = Selection( name, Algorithm = _filterKSLL, RequiredSelections = [_stdKSLL] )

    def makeH( self, name, config ) :
        # define all the cuts
        _bachPtCut      = "(PT>%s*MeV)"                      % config['Bach_Ptmin']
        _trkChi2Cut = "(TRCHI2DOF<%s)" % config['Trk_Chi2']
        _trkGPCut = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']
                
        _allCuts = _trkGPCut + '&' + _trkChi2Cut + '&' + _bachPtCut

        # make the filter
        _filterH = FilterDesktop( Code = _allCuts )

        # make and store the selection object
        self.selH = Selection( name, Algorithm = _filterH, RequiredSelections = [self.pions] )

    def makeBu2KSDDh( self, name, config ) :
        """
        Create and store a B -> KS(DD) h+ Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5279-%s)*MeV)"               % config['B_Mlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"               % config['B_Mhigh']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        _daugPtSumCut   = "((APT1+APT2)>%s*MeV)"             % config['BDaug_DD_PTsum']
        
        _combCuts = _daugPtSumCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut

        _pCut       = "(P>%s*MeV)"                     % config['B_Pmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['B_DD_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['B_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_DD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['B_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_DD_FDChi2']

        _motherCuts = _pCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _B = CombineParticles()
        _B.DecayDescriptors = [ "B+ -> pi+ KS0", "B- -> pi- KS0" ]
        _B.DaughtersCuts = { "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        return Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2DD, self.selH ])

    def makeBu2KSLLh( self, name, config ) :
        """
        Create and store a Bu -> KS(LL) h+ Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5279-%s)*MeV)"               % config['B_Mlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"               % config['B_Mhigh']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['BDaug_MaxPT_IP']
        _daugPtSumCut   = "((APT1+APT2)>%s*MeV)"             % config['BDaug_LL_PTsum']
        
        _combCuts = _daugPtSumCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut

        _pCut       = "(P>%s*MeV)"                     % config['B_Pmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['B_LL_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['B_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['B_LL_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['B_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['B_LL_FDChi2']

        _motherCuts = _pCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _B = CombineParticles()

        _B.DecayDescriptors = [ "B+ -> pi+ KS0", "B- -> pi- KS0" ]

        _B.DaughtersCuts = { "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        return Selection (name, Algorithm = _B, RequiredSelections = [ self.selKS2LL, self.selH ])
