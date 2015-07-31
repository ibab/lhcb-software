"""
Module for construction of Lb->V0h(h) stripping Selections and StrippingLines.
Provides functions to build Lambda0->DD, Lambda0->LL, and Lambda0->LD selections.
Stripping20 with an inclusive approach for Lb->Lambda h(h) modes.
Provides class Lb2V0hhConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Selections based on previous version of the line by Jussara Miranda.
Exported symbols (use python help!):
   - Lb2V0hhConf
"""

__author__ = ["Thomas Latham", "Rafael Coutinho", "Christian Voss", "Christoph Hombach", "Daniel O'Hanlon"]
__date__ = '21/05/2015'
__version__ = 'Stripping23'
__all__ = 'Lb2V0hhConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdNoPIDsPions as Pions

default_config = {
    'NAME' : 'Lb2V0h',
    'WGs'  : ['BnoC'],
    'BUILDERTYPE' : 'Lb2V0hhConf',
    'CONFIG' : {'Trk_Chi2'                 : 3.0,
                'Trk_GhostProb'            : 0.5,
                'Lambda_DD_MassWindow'     : 25.0,
                'Lambda_DD_VtxChi2'        : 15.0,
                'Lambda_DD_FDChi2'         : 50.0,
                'Lambda_DD_FD'             : 300.0,
                'Lambda_DD_Pmin'           : 5000.0,
                'Lambda_LL_MassWindow'     : 20.0,
                'Lambda_LL_VtxChi2'        : 15.0,
                'Lambda_LL_FDChi2'         : 80.0,
                'Lambda_LD_MassWindow'     : 25.0,
                'Lambda_LD_VtxChi2'        : 15.0,
                'Lambda_LD_FDChi2'         : 50.0,
                'Lambda_LD_FD'             : 300.0,
                'Lambda_LD_Pmin'           : 5000.0,
                'Lb_Mlow'                  : 1319.0,
                'Lb_Mhigh'                 : 600.0,
                'Lb_2bodyMlow'             : 800.0,
                'Lb_2bodyMhigh'            : 800.0,
                'Lb_APTmin'                : 1000.0,
                'Lb_PTmin'                 : 800.0,
                'LbDaug_MedPT_PT'          : 800.0,
                'LbDaug_MaxPT_IP'          : 0.05,
                'LbDaug_DD_maxDocaChi2'    : 5.0,
                'LbDaug_LL_maxDocaChi2'    : 5.0,
                'LbDaug_LD_maxDocaChi2'    : 5.0,
                'LbDaug_DD_PTsum'          : 4200.0,
                'LbDaug_LL_PTsum'          : 3000.0,
                'LbDaug_LD_PTsum'          : 4200.0,
                'Lbh_DD_PTMin'             : 500.0,
                'Lbh_LL_PTMin'             : 500.0,
                'Lbh_LD_PTMin'             : 500.0,
                'Lb_VtxChi2'               : 12.0,
                'Lb_DD_Dira'               : 0.995,
                'Lb_LL_Dira'               : 0.995,
                'Lb_LD_Dira'               : 0.995,
                'Lb_DD_IPCHI2wrtPV'        : 15.0,
                'Lb_LL_IPCHI2wrtPV'        : 15.0,
                'Lb_LD_IPCHI2wrtPV'        : 15.0,
                'Lb_FDwrtPV'               : 1.0,
                'Lb_DD_FDChi2'             : 30.0,
                'Lb_LL_FDChi2'             : 30.0,
                'Lb_LD_FDChi2'             : 30.0,
                'GEC_MaxTracks'            : 250,
                # 2012 Triggers
                #'HLT1Dec'                  : 'Hlt1TrackAllL0Decision',
                #'HLT2Dec'                  : 'Hlt2Topo[234]Body.*Decision',
                # 2015 Triggers
                'HLT1Dec'                  : 'Hlt1(Two)?TrackMVADecision',
                'HLT2Dec'                  : 'Hlt2Topo[234]BodyDecision',
                'Prescale'                 : 1.0,
                'Postscale'                : 1.0,
                'RelatedInfoTools' : [    { "Type" : "RelInfoConeVariables", 
                                            "ConeAngle" : 1.7, 
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                            "Location"  : 'ConeVar17'
                                            }, 
                                          { "Type" : "RelInfoConeVariables", 
                                            "ConeAngle" : 1.5, 
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                            "Location"  : 'ConeVar15'
                                            }, 
                                          { "Type" : "RelInfoConeVariables", 
                                            "ConeAngle" : 1.0, 
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                            "Location"  : 'ConeVar10'
                                            },
                                          { "Type" : "RelInfoConeVariables", 
                                            "ConeAngle" : 0.8, 
                                            "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                            "Location"  : 'ConeVar08'
                                            },
                                          { "Type" : "RelInfoVertexIsolation",
                                            "Location" : "VtxIsolationVar"
                                            }
                                          ]
                },
    'STREAMS' : ['Bhadron']
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
    selLambda2LD           : Lambda0 -> Long Down Selection object

    selLb2V0DDhh           : Lb -> Lambda0(DD) h+ h- Selection object
    selLb2V0LLhh           : Lb -> Lambda0(LL) h+ h- Selection object
    selLb2V0LDhh           : Lb -> Lambda0(LD) h+ h- Selection object            
    selLb2V0DDh            : Lb -> Lambda0(DD) h- Selection object
    selLb2V0LLh            : Lb -> Lambda0(LL) h- Selection object 
    selLb2V0LDh            : Lb -> Lambda0(LD) h- Selection object        

    Lb_dd_line             : StrippingLine made out of selLb2V0DDhh
    Lb_ll_line             : StrippingLine made out of selLb2V0LLhh 
    Lb_ld_line             : StrippingLine made out of selLb2V0LDhh

    lines                  : List of lines, [Lb_dd_line, Lb_ll_line, Lb_ld_line]

    Exports as class data member:
    Lb2V0hhConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'Trk_GhostProb',
                              'Lambda_DD_MassWindow',
                              'Lambda_DD_VtxChi2',
                              'Lambda_DD_FDChi2',
                              'Lambda_DD_FD',
                              'Lambda_DD_Pmin',
                              'Lambda_LL_MassWindow',
                              'Lambda_LL_VtxChi2',
                              'Lambda_LL_FDChi2',
                              'Lambda_LD_MassWindow',
                              'Lambda_LD_VtxChi2',
                              'Lambda_LD_FDChi2',
                              'Lambda_LD_FD',
                              'Lambda_LD_Pmin',
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
                              'LbDaug_LD_maxDocaChi2',
                              'LbDaug_DD_PTsum',
                              'LbDaug_LL_PTsum',
                              'LbDaug_LD_PTsum',
                              'Lbh_LL_PTMin',
                              'Lbh_DD_PTMin',
                              'Lbh_LD_PTMin',
                              'Lb_VtxChi2',
                              'Lb_DD_Dira',
                              'Lb_LD_Dira',
                              'Lb_LL_Dira',
                              'Lb_DD_IPCHI2wrtPV',
                              'Lb_LL_IPCHI2wrtPV',
                              'Lb_LD_IPCHI2wrtPV',
                              'Lb_FDwrtPV',
                              'Lb_DD_FDChi2',
                              'Lb_LD_FDChi2',
                              'Lb_LL_FDChi2',
                              'GEC_MaxTracks',
                              'HLT1Dec',
                              'HLT2Dec',
                              'Prescale',
                              'Postscale',
                              'RelatedInfoTools'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.hlt1Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT1Dec'],
                           'Preambulo' : ["from LoKiCore.functions import *"]}
        self.hlt2Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT2Dec'],
                           'Preambulo' : ["from LoKiCore.functions import *"]}

        self.pions   = Pions

        self.makeLambda2DD( 'Lambda0DDLbLines', config )
        self.makeLambda2LL( 'Lambda0LLLbLines', config )
        self.makeLambda2LD( 'Lambda0LDLbLines', config )

        namesSelections = [ (name + 'DD', self.makeLb2V0DDh(name + 'DD', config)),
                            (name + 'LL', self.makeLb2V0LLh(name + 'LL', config)),
                            (name + 'LD', self.makeLb2V0LDh(name + 'LD', config)),

                            (name + 'hDD', self.makeLb2V0DDhh(name + 'hDD', config)),
                            (name + 'hLL', self.makeLb2V0LLhh(name + 'hLL', config)),
                            (name + 'hLD', self.makeLb2V0LDhh(name + 'hLD', config)),

                            (name + 'hDDSS', self.makeLb2V0DDhh(name + 'hDDSS', config)),
                            (name + 'hLLSS', self.makeLb2V0LLhh(name + 'hLLSS', config)),
                            (name + 'hLDSS', self.makeLb2V0LDhh(name + 'hLDSS', config)),
                          ]

        # make lines

        for selName, sel in namesSelections:

            extra = {}

            if 'SS' in selName:
                extra['HLT1'] = self.hlt1Filter
                extra['HLT2'] = self.hlt2Filter

            line = StrippingLine(selName + 'Line',
                                 selection = sel,
                                 prescale = config['Prescale'],
                                 postscale = config['Postscale'],
                                 RelatedInfoTools = config['RelatedInfoTools'], 
                                 FILTER = GECCode,
                                 **extra) 

            self.registerLine(line)

    def makeLambda2DD( self, name, config ) :
        # define all the cuts
        _massCut          = "(ADMASS('Lambda0')<%s*MeV)"      % config['Lambda_DD_MassWindow']
        _vtxCut           = "(VFASPF(VCHI2)<%s)   "           % config['Lambda_DD_VtxChi2']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                  % config['Lambda_DD_FDChi2']
        _momCut           = "(P>%s*MeV)"                      % config['Lambda_DD_Pmin']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))"  % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))"  % config['Trk_GhostProb']

        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdChi2Cut+'&'+_trkGhostProbCut1+'&'+_trkGhostProbCut2

        # get the Lambda0's to filter
        _stdLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
        
        # make the filter
        _filterLambdaDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selLambda2DD = Selection( name, Algorithm = _filterLambdaDD, RequiredSelections = [_stdLambdaDD] )

        return self.selLambda2DD

    def makeLambda2LL( self, name, config ) : 
        # define all the cuts
        _massCut    = "(ADMASS('Lambda0')<%s*MeV)"           % config['Lambda_LL_MassWindow']
        _vtxCut     = "(VFASPF(VCHI2)<%s)"                   % config['Lambda_LL_VtxChi2']
        _trkChi2Cut1 = "(CHILDCUT((TRCHI2DOF<%s),1))"        % config['Trk_Chi2']
        _trkChi2Cut2 = "(CHILDCUT((TRCHI2DOF<%s),2))"        % config['Trk_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_GhostProb']

        _allCuts = _massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_vtxCut+'&'+_trkGhostProbCut1+'&'+_trkGhostProbCut2

        # get the Lambda's to filter
        _stdLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")

        # make the filter
        _filterLambdaLL = FilterDesktop( Code = _allCuts )
        
        # make and store the Selection object
        self.selLambda2LL = Selection( name, Algorithm = _filterLambdaLL, RequiredSelections = [_stdLambdaLL] )

        return self.selLambda2LL

    def makeLambda2LD( self, name, config ) :
        # define all the cuts
        _massCut          = "(ADMASS('Lambda0')<%s*MeV)"      % config['Lambda_DD_MassWindow']
        _vtxCut           = "(VFASPF(VCHI2)<%s)   "           % config['Lambda_DD_VtxChi2']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                  % config['Lambda_DD_FDChi2']
        _momCut           = "(P>%s*MeV)"                      % config['Lambda_DD_Pmin']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))"  % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))"  % config['Trk_GhostProb']

        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdChi2Cut+'&'+_trkGhostProbCut1+'&'+_trkGhostProbCut2

        # get the Lambda0's to filter
        _stdLambdaLD = DataOnDemand(Location = "Phys/StdLooseLambdaLD/Particles")
        
        # make the filter
        _filterLambdaLD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selLambda2LD = Selection( name, Algorithm = _filterLambdaLD, RequiredSelections = [_stdLambdaLD] )

        return self.selLambda2LD

    def makeLb2V0DDh( self, name, config ) :
        """
        Create and store a Lb ->Lambda0(DD) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5619-%s)*MeV)"               % config['Lb_2bodyMlow']
        _massCutHigh    = "(AM<(5619+%s)*MeV)"               % config['Lb_2bodyMhigh']
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
        _Lb.ReFitPVs = True
        
        self.selLb2V0DDh = Selection (name, Algorithm = _Lb, RequiredSelections = [self.selLambda2DD, self.pions ])

        return self.selLb2V0DDh

    def makeLb2V0DDhh( self, name, config ) :
        """
        Create and store a Lb ->Lambda0(DD) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5619-%s)*MeV)"               % config['Lb_Mlow']
        _massCutHigh    = "(AM<(5619+%s)*MeV)"               % config['Lb_Mhigh']
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

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdChi2Cut

        _Lb = CombineParticles()

        if 'SS' in name: # Same sign

            _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi+ Lambda0", "Lambda_b~0 -> pi+ pi+ Lambda~0",
                                     "Lambda_b0 -> pi- pi- Lambda0", "Lambda_b~0 -> pi- pi- Lambda~0"]

        else:

            _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi- Lambda0", "Lambda_b~0 -> pi+ pi- Lambda~0"]

        _trkGhostProbCut  = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']
        _trkChi2Cut       = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']

        _daughtersCuts = _trkChi2Cut + '&' + _trkGhostProbCut

        _Lb.DaughtersCuts = { "pi+" : _daughtersCuts }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts
        _Lb.ReFitPVs = True
        
        _LbConf = _Lb.configurable(name + '_combined')

        if 'SS' in name: # Same sign
            self.selLb2V0DDhhSS = Selection (name, Algorithm = _LbConf, RequiredSelections = [self.selLambda2DD, self.pions ])
            return self.selLb2V0DDhhSS
        else:
            self.selLb2V0DDhh = Selection (name, Algorithm = _LbConf, RequiredSelections = [self.selLambda2DD, self.pions ])
            return self.selLb2V0DDhh

    def makeLb2V0LLh( self, name, config ) :
        """
        Create and store a Lb -> Lambda0(LL) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5619-%s)*MeV)"               % config['Lb_2bodyMlow']
        _massCutHigh    = "(AM<(5619+%s)*MeV)"               % config['Lb_2bodyMhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_LL_maxDocaChi2']
        _daugPtSumCut   = "(APT1>%s*MeV)"                    % config['Lbh_LL_PTMin']

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
        _Lb.ReFitPVs = True
        
        self.selLb2V0LLh = Selection (name, Algorithm = _Lb, RequiredSelections = [self.selLambda2LL, self.pions  ])

        return self.selLb2V0LLh

    def makeLb2V0LLhh( self, name, config ) :
        """
        Create and store a Lb -> Lambda0(LL) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5619-%s)*MeV)"               % config['Lb_Mlow']
        _massCutHigh    = "(AM<(5619+%s)*MeV)"               % config['Lb_Mhigh']
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

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdChi2Cut+'&'+_fdCut

        _Lb = CombineParticles()

        if 'SS' in name: # Same sign

            _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi+ Lambda0", "Lambda_b~0 -> pi+ pi+ Lambda~0",
                                     "Lambda_b0 -> pi- pi- Lambda0", "Lambda_b~0 -> pi- pi- Lambda~0"]

        else:

            _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi- Lambda0", "Lambda_b~0 -> pi+ pi- Lambda~0"]

        _trkGhostProbCut  = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']
        _trkChi2Cut       = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']

        _daughtersCuts = _trkChi2Cut + '&' + _trkGhostProbCut

        _Lb.DaughtersCuts = { "pi+" : _daughtersCuts }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts
        _Lb.ReFitPVs = True
        
        _LbConf = _Lb.configurable(name + '_combined')

        if 'SS' in name: # Same sign
            self.selLb2V0LLhhSS = Selection (name, Algorithm = _LbConf, RequiredSelections = [self.selLambda2LL, self.pions  ])
            return self.selLb2V0LLhhSS
        else:
            self.selLb2V0LLhh = Selection (name, Algorithm = _LbConf, RequiredSelections = [self.selLambda2LL, self.pions  ])
            return self.selLb2V0LLhh

    def makeLb2V0LDh( self, name, config ) :
        """
        Create and store a Lb ->Lambda0(LD) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5619-%s)*MeV)"               % config['Lb_2bodyMlow']
        _massCutHigh    = "(AM<(5619+%s)*MeV)"               % config['Lb_2bodyMhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['LbDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_LD_maxDocaChi2']
        _daugPtSumCut   = "(APT1>%s*MeV)"                    % config['Lbh_LD_PTMin']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['Lb_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['Lb_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_LD_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_LD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_LD_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut

        _Lb = CombineParticles()
        _Lb.DecayDescriptors = [ "Lambda_b0 -> pi- Lambda0", "Lambda_b~0 -> pi+ Lambda~0"]
        _Lb.DaughtersCuts = { "pi+" : "TRCHI2DOF<%s"% config['Trk_Chi2'] }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts
        _Lb.ReFitPVs = True

        self.selLb2V0LDh = Selection (name, Algorithm = _Lb, RequiredSelections = [self.selLambda2LD, self.pions ])

        return self.selLb2V0LDh

    def makeLb2V0LDhh( self, name, config ) :
        """
        Create and store a Lb ->Lambda0(LD) p+ h- Selection object.
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow     = "(AM>(5619-%s)*MeV)"               % config['Lb_Mlow']
        _massCutHigh    = "(AM<(5619+%s)*MeV)"               % config['Lb_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                     % config['Lb_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"             % config['LbDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)" % config['LbDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"            % config['LbDaug_LD_maxDocaChi2']
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"        % config['LbDaug_LD_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut+'&'+_maxDocaChi2Cut

        _ptCut      = "(PT>%s*MeV)"                    % config['Lb_PTmin']
        _vtxChi2Cut = "(VFASPF(VCHI2)<%s)"             % config['Lb_VtxChi2']
        _diraCut    = "(BPVDIRA>%s)"                   % config['Lb_LD_Dira']
        _ipChi2Cut  = "(MIPCHI2DV(PRIMARY)<%s)"        % config['Lb_LD_IPCHI2wrtPV']
        _fdCut      = "(VFASPF(VMINVDDV(PRIMARY))>%s)" % config['Lb_FDwrtPV']
        _fdChi2Cut  = "(BPVVDCHI2>%s)"                 % config['Lb_LD_FDChi2']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdChi2Cut

        _Lb = CombineParticles()

        if 'SS' in name: # Same sign

            _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi+ Lambda0", "Lambda_b~0 -> pi+ pi+ Lambda~0",
                                     "Lambda_b0 -> pi- pi- Lambda0", "Lambda_b~0 -> pi- pi- Lambda~0"]

        else:

            _Lb.DecayDescriptors = [ "Lambda_b0 -> pi+ pi- Lambda0", "Lambda_b~0 -> pi+ pi- Lambda~0"]

        _trkGhostProbCut  = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']
        _trkChi2Cut       = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']

        _daughtersCuts = _trkChi2Cut + '&' + _trkGhostProbCut

        _Lb.DaughtersCuts = { "pi+" : _daughtersCuts }
        _Lb.CombinationCut = _combCuts
        _Lb.MotherCut = _motherCuts
        _Lb.ReFitPVs = True
        
        _LbConf = _Lb.configurable(name + '_combined')

        if 'SS' in name: # Same sign
            self.selLb2V0LDhhSS = Selection (name, Algorithm = _LbConf, RequiredSelections = [self.selLambda2LD, self.pions ])
            return self.selLb2V0LDhhSS
        else:
            self.selLb2V0LDhh = Selection (name, Algorithm = _LbConf, RequiredSelections = [self.selLambda2LD, self.pions ])
            return self.selLb2V0LDhh
