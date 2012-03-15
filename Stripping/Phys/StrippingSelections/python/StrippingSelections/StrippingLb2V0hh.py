"""
Module for construction of Lb->V0hh stripping Selections and StrippingLines.
Provides functions to build KS->DD, KS->LL and Lb selections.
For the Stripping18 the only mode included are Lb->Ksph and for the next
version the idea is to include Lb->Lshh modes.
Provides class Lb2KShhConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Selections based on previous version of the line by Jussara Miranda.
Exported symbols (use python help!):
   - Lb2V0hhConf
"""

__author__ = ['Thomas Latham','Rafael Coutinho']
__date__ = '15/03/2012'
__version__ = 'Stripping18'
__all__ = 'Lb2V0hhConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdLoosePions as Pions
from StandardParticles import StdLooseKaons as Kaons
from StandardParticles import StdLooseProtons as Protons

default_config = {'Trk_Chi2'                 : 4.0,
                  'KS_DD_MassWindow'         : 30.0,
                  'KS_DD_VtxChi2'            : 12.0,
                  'KS_DD_FDChi2'             : 50.0,
                  'KS_DD_Pmin'               : 6000.0,
                  'KS_LL_MassWindow'         : 20.0,
                  'KS_LL_VtxChi2'            : 12.0,
                  'KS_LL_FDChi2'             : 80.0,
                  'Lb_Mlow'                  : 200.0,
                  'Lb_Mhigh'                 : 651.0,
                  'Lb_APTmin'                : 1000.0,
                  'Lb_PTmin'                 : 1500.0,
                  'LbDaug_MedPT_PT'          : 800.0,
                  'LbDaug_MaxPT_IP'          : 0.05,
                  'LbDaug_DD_maxDocaChi2'    : 5.0,
                  'LbDaug_LL_maxDocaChi2'    : 5.0,
                  'LbDaug_DD_PTsum'          : 3000.0,
                  'LbDaug_LL_PTsum'          : 3000.0,
                  'Lb_VtxChi2'               : 12.0,
                  'Lb_Dira'                  : 0.9999,
                  'Lb_DD_IPCHI2wrtPV'        : 8.0,
                  'Lb_LL_IPCHI2wrtPV'        : 8.0,
                  'Lb_FDwrtPV'               : 1.0,
                  'Lb_DD_FDChi2'             : 50.0,
                  'Lb_LL_FDChi2'             : 50.0,
                  'GEC_MaxTracks'            : 250,
                  'Prescale'                 : 1.0,
                  'Postscale'                : 1.0
                  }

class Lb2V0hhConf(LineBuilder) :
    """
    Builder of Lb->V0hh stripping Selection and StrippingLine.
    Constructs Lb -> V0 h+ h- Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> lb2v0hhConf = LB2V0hhConf('Lb2V0hhTest',config)
    >>> lb2v0hhLines = lb2v0hhConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKS2DD               : KS -> Down Down Selection object
    selKS2LL               : KS -> Long Long Selection object
    selLb2V0DDhh           : Lb -> KS(DD) p+ h- Selection object
    selLb2V0LLhh           : Lb -> KS(LL) p+ h- Selection object        
    Lb_dd_line             : StrippingLine made out of selLb2V0DDhh
    Lb_ll_line             : StrippingLine made out of selLb2V0LLhh        
    lines                  : List of lines, [Lb_line, Lb_line]

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
                              'Lb_Mlow',
                              'Lb_Mhigh',
                              'Lb_APTmin',
                              'Lb_PTmin',
                              'LbDaug_MedPT_PT',
                              'LbDaug_MaxPT_IP',
                              'LbDaug_DD_maxDocaChi2',
                              'LbDaug_LL_maxDocaChi2',
                              'LbDaug_DD_PTsum',
                              'LbDaug_LL_PTsum',
                              'Lb_VtxChi2',
                              'Lb_Dira',
                              'Lb_DD_IPCHI2wrtPV',
                              'Lb_LL_IPCHI2wrtPV',
                              'Lb_FDwrtPV',
                              'Lb_DD_FDChi2',
                              'Lb_LL_FDChi2',
                              'GEC_MaxTracks',
                              'Prescale',
                              'Postscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        Lb_dd_name    = name+'DD'
        Lb_ll_name    = name+'LL'
        

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions   = Pions
        self.kaons   = Kaons
        self.protons = Protons

        self.hadrons = MergedSelection("HadronsFor" + name,
                                       RequiredSelections = [ self.pions, self.kaons, self.protons ] )
        

        self.makeKS2DD( 'KSfor'+Lb_dd_name, config )
        self.makeKS2LL( 'KSfor'+Lb_ll_name, config )        

        self.makeLb2V0DDhh( Lb_dd_name, config )
        self.makeLb2V0LLhh( Lb_ll_name, config )
                

        self.Lb_dd_line = StrippingLine(Lb_dd_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selLb2V0DDhh,
                                     FILTER = GECCode
                                     )
        self.Lb_ll_line = StrippingLine(Lb_ll_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selLb2V0LLhh,
                                     FILTER = GECCode
                                     )

        self.registerLine(self.Lb_dd_line)
        self.registerLine(self.Lb_ll_line)

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


    def makeLb2V0DDhh( self, name, config ) :
        """
        Create and store a Lb -> KS(DD) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5620-%s)*MeV)"               % config['Lb_Mlow']
        _massCutHigh    = "(AM<(5620+%s)*MeV)"               % config['Lb_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['LbDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_DD_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['LbDaug_DD_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['Lb_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['Lb_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_DD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_DD_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _Lb = CombineParticles()
        _Lb.DecayDescriptors = [ "Lambda_b0 -> p+ pi- KS0", "Lambda_b~0 -> p~- pi+ KS0", \
                                 "Lambda_b0 -> p+ K- KS0",  "Lambda_b~0 -> p~- K+ KS0"]
        _Lb.DaughtersCuts = { "K+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "p+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts

        self.selLb2V0DDhh = Selection (name, Algorithm = _Lb, RequiredSelections = [ self.selKS2DD, self.hadrons ])


    def makeLb2V0LLhh( self, name, config ) :
        """
        Create and store a Lb -> KS(LL) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5620-%s)*MeV)"               % config['Lb_Mlow']
        _massCutHigh    = "(AM<(5620+%s)*MeV)"               % config['Lb_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['LbDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['LbDaug_LL_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['Lb_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['Lb_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_LL_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_LL_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _Lb = CombineParticles()
        _Lb.DecayDescriptors = [ "Lambda_b0 -> p+ pi- KS0", "Lambda_b~0 -> p~- pi+ KS0", \
                                 "Lambda_b0 -> p+ K- KS0",  "Lambda_b~0 -> p~- K+ KS0"]
        _Lb.DaughtersCuts = { "K+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'], "p+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts

        self.selLb2V0LLhh = Selection (name, Algorithm = _Lb, RequiredSelections = [ self.selKS2LL, self.hadrons ])

