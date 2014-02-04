"""
Module for construction of Lb->V0h(h) stripping Selections and StrippingLines.
Provides functions to build Lambda0->DD and Lambda0->LL selections.
Stripping20 with an inclusive approach for Lb->Lambda h(h) modes.
Provides class Lb2V0hhConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Selections based on previous version of the line by Jussara Miranda.
Exported symbols (use python help!):
   - Lb2V0hhConf
"""

__author__ = ['Thomas Latham','Rafael Coutinho', 'Christian Voss', 'Christoph Hombach']
__date__ = '23/08/2013'
__version__ = 'Stripping20r0p2'
__all__ = 'Lb2V0hhConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdNoPIDsPions as Pions
from CommonParticles.StdLooseLambda import StdLooseLambdaLL, StdLooseLambdaDD

default_config = {'Trk_Chi2'                 : 3.0,
                  'Lambda_DD_MassWindow'     : 20.0,
                  'Lambda_DD_VtxChi2'        : 12.0,
                  'Lambda_DD_FDChi2'         : 50.0,
                  'Lambda_DD_FD'             : 300.0,
                  'Lambda_DD_Pmin'           : 8000.0,
                  'Lambda_LL_MassWindow'     : 15.0,
                  'Lambda_LL_VtxChi2'        : 12.0,
                  'Lambda_LL_FDChi2'         : 50.0,
                  'Lb_Mlow'                  : 1319.0,
                  'Lb_Mhigh'                 : 500.0,
                  'Lb_2bodyMlow'             : 800.0,
                  'Lb_2bodyMhigh'            : 800.0,
                  'Lb_APTmin'                : 1000.0,
                  'Lb_PTmin'                 : 1500.0,
                  'LbDaug_MedPT_PT'          : 800.0,
                  'LbDaug_MaxPT_IP'          : 0.05,
                  'LbDaug_DD_maxDocaChi2'    : 5.0,
                  'LbDaug_LL_maxDocaChi2'    : 5.0,
                  'LbDaug_DD_PTsum'          : 4200.0,
                  'LbDaug_LL_PTsum'          : 3000.0,
                  'Lbh_DD_PTMin'             : 500.0,
                  'Lbh_LL_PTMin'             : 500.0,
                  'Lb_VtxChi2'               : 12.0,
                  'Lb_DD_Dira'               : 0.995,
                  'Lb_LL_Dira'               : 0.995,
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
    Builder of Lb->V0h(h) stripping Selection and StrippingLine.
    Constructs Lb -> V0 (h+) h- Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> lb2v0hhConf = LB2V0hhConf('Lb2V0hhTest',config)
    >>> lb2v0hhLines = lb2v0hhConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selLambda2DD           : Lambda0 -> Down Down Selection object
    selLambda2LL           : Lambda0 -> Long Long Selection object
    selLb2V0DDhh           : Lb -> Lambda0(DD) h+ h- Selection object
    selLb2V0LLhh           : Lb -> Lambda0(LL) h+ h- Selection object        
    selLb2V0DDh            : Lb -> Lambda0(DD) h- Selection object
    selLb2V0LLh            : Lb -> Lambda0(LL) h- Selection object        

    Lb_dd_line             : StrippingLine made out of selLb2V0DDhh
    Lb_ll_line             : StrippingLine made out of selLb2V0LLhh        
    lines                  : List of lines, [Lb_line, Lb_line]

    Exports as class data member:
    Lb2V0hhConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'Lambda_DD_MassWindow',
                              'Lambda_DD_VtxChi2',
                              'Lambda_DD_FDChi2',
                              'Lambda_DD_FD',
                              'Lambda_DD_Pmin',
                              'Lambda_LL_MassWindow',
                              'Lambda_LL_VtxChi2',
                              'Lambda_LL_FDChi2',
                              'Lb_Mlow',
                              'Lb_Mhigh',
                              'Lb_2bodyMlow',
                              'Lb_2bodyMhigh',
                              'Lb_APTmin',
                              'Lb_PTmin',
                              'LbDaug_MedPT_PT',
                              'LbDaug_MaxPT_IP',
                              'LbDaug_DD_maxDocaChi2',
                              'LbDaug_LL_maxDocaChi2',
                              'LbDaug_DD_PTsum',
                              'LbDaug_LL_PTsum',
                              'Lbh_LL_PTMin',
                              'Lbh_DD_PTMin',
                              'Lb_VtxChi2',
                              'Lb_DD_Dira',
                              'Lb_LL_Dira',
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

        Lb2_dd_name    = name+'DD'
        Lb2_ll_name    = name+'LL'
        Lb3_dd_name    = name+'hDD'
        Lb3_ll_name    = name+'hLL'
        

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions   = Pions

        self.makeLambda2DD( 'Lambda0DDLbLines', config )
        self.makeLambda2LL( 'Lambda0LLLbLines', config )

        self.makeLb2V0DDh( Lb2_dd_name, config )
        self.makeLb2V0LLh( Lb2_ll_name, config )
        self.makeLb2V0DDhh( Lb3_dd_name, config )
        self.makeLb2V0LLhh( Lb3_ll_name, config )
                

        self.Lb2_dd_line = StrippingLine(Lb2_dd_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selLb2V0DDh,
                                     FILTER = GECCode
                                     )
        self.Lb2_ll_line = StrippingLine(Lb2_ll_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selLb2V0LLh,
                                     FILTER = GECCode
                                     )
        self.Lb3_dd_line = StrippingLine(Lb3_dd_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selLb2V0DDhh,
                                     FILTER = GECCode
                                     )
        self.Lb3_ll_line = StrippingLine(Lb3_ll_name+"Line",
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection =  self.selLb2V0LLhh,
                                     FILTER = GECCode
                                     )
        self.registerLine(self.Lb2_dd_line)
        self.registerLine(self.Lb2_ll_line)
        self.registerLine(self.Lb3_dd_line)
        self.registerLine(self.Lb3_ll_line)

    def makeLambda2DD( self, name, config ) :
        # define all the cuts
        _massCut   = "(ADMASS('Lambda0')<%s*MeV)"      % config['Lambda_DD_MassWindow']
        _vtxCut    = "(VFASPF(VCHI2)<%s)   "           % config['Lambda_DD_VtxChi2']
        _fdChi2Cut = "(BPVVDCHI2>%s)"                  % config['Lambda_DD_FDChi2']
        _fdCut     = "(VFASPF(VMINVDDV(PRIMARY))>%s)"  % config['Lambda_DD_FD']
        _momCut    = "(P>%s*MeV)"                      % config['Lambda_DD_Pmin']

        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_fdChi2Cut

        # get the Lambda0's to filter
        _stdLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
        
        # make the filter
        _filterLambdaDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selLambda2DD = Selection( name, Algorithm = _filterLambdaDD, RequiredSelections = [_stdLambdaDD] )

    def makeLambda2LL( self, name, config ) : 
        # define all the cuts
        _massCut    = "(ADMASS('Lambda0')<%s*MeV)" % config['Lambda_LL_MassWindow']
        _vtxCut     = "(VFASPF(VCHI2)<%s)"         % config['Lambda_LL_VtxChi2']
        _fdCut      = "(BPVVDCHI2>%s)"             % config['Lambda_LL_FDChi2']
        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))" % config['Trk_Chi2']
        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))" % config['Trk_Chi2']
        _allCuts = _massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_vtxCut+'&'+_fdCut

        # get the Lambda's to filter
        _stdLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")

        # make the filter
        _filterLambdaLL = FilterDesktop( Code = _allCuts )
        
        # make and store the Selection object
        self.selLambda2LL = Selection( name, Algorithm = _filterLambdaLL, RequiredSelections = [_stdLambdaLL] )

    def makeLb2V0DDh( self, name, config ) :
        """
        Create and store a Lb ->Lambda0(DD) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5620-%s)*MeV)"               % config['Lb_2bodyMlow']
        _massCutHigh    = "(AM<(5620+%s)*MeV)"               % config['Lb_2bodyMhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['LbDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_DD_maxDocaChi2']
        _daugPtSumCut   = "(APT1>%s*MeV)"                    % config['Lbh_DD_PTMin']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['Lb_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['Lb_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_DD_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_DD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_DD_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _Lb = CombineParticles()
        _Lb.DecayDescriptors = [ "Lambda_b0 -> pi- Lambda0", "Lambda_b~0 -> pi+ Lambda~0"]
        _Lb.DaughtersCuts = { "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts

        self.selLb2V0DDh = Selection (name, Algorithm = _Lb, RequiredSelections = [self.selLambda2DD, self.pions ])

    def makeLb2V0DDhh( self, name, config ) :
        """
        Create and store a Lb ->Lambda0(DD) p+ h- Selection object.
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
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_DD_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_DD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_DD_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _Lb = CombineParticles()
        _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi- Lambda0", "Lambda_b~0 -> pi+ pi- Lambda~0"]
        _Lb.DaughtersCuts = { "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts

        self.selLb2V0DDhh = Selection (name, Algorithm = _Lb, RequiredSelections = [self.selLambda2DD, self.pions ])


    def makeLb2V0LLh( self, name, config ) :
        """
        Create and store a Lb -> Lambda0(LL) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5620-%s)*MeV)"               % config['Lb_2bodyMlow']
        _massCutHigh    = "(AM<(5620+%s)*MeV)"               % config['Lb_2bodyMhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "(APT1>%s*MeV)"                   % config['Lbh_LL_PTMin']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['Lb_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['Lb_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_LL_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_LL_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_LL_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _Lb = CombineParticles()
        _Lb.DecayDescriptors = [ "Lambda_b0 -> pi- Lambda0", "Lambda_b~0 -> pi+ Lambda~0" ]
        _Lb.DaughtersCuts = { "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts

        self.selLb2V0LLh = Selection (name, Algorithm = _Lb, RequiredSelections = [self.selLambda2LL, self.pions  ])

    def makeLb2V0LLhh( self, name, config ) :
        """
        Create and store a Lb -> Lambda0(LL) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5620-%s)*MeV)"               % config['Lb_Mlow']
        _massCutHigh    = "(AM<(5620+%s)*MeV)"               % config['Lb_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['LbDaug_LL_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['Lb_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['Lb_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_LL_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_LL_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_LL_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _Lb = CombineParticles()
        _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi- Lambda0", "Lambda_b~0 -> pi+ pi- Lambda~0" ]
        _Lb.DaughtersCuts = { "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts

        self.selLb2V0LLhh = Selection (name, Algorithm = _Lb, RequiredSelections = [self.selLambda2LL, self.pions  ])
