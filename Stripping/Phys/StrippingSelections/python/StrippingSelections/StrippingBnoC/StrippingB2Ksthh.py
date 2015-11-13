"""
Module for construction of B(d,s) -> K*0(892) hh', with K*0(892) -> K+ pi-.
Provides class B2KsthhConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Exported symbols (use python help!):
   - B2KsthhConf
"""

__author__ = ['Rafael Coutinho']
__date__ = '10/11/2015'
__version__ = 'Stripping24'
__all__ = {'B2KsthhConf',
           'default_config'}

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdLoosePions as Pions

default_config = {
    'NAME'        : 'B2Ksthh',
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2KsthhConf',
    'CONFIG'      : {'Trk_Chi2'                : 4.0,
                     'Trk_GhostProb'           : 0.4,
                     'Kstar_MassLo'            : 0.0,
                     'Kstar_MassHi'            : 5000.0,
                     'B_Mlow'                  : 1279.0,
                     'B_Mhigh'                 : 921.0,
                     'B_APTmin'                : 1000.0,
                     'B_PTmin'                 : 1500.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_PTsum'             : 3000.0,
                     'B_IPCHI2sum'             : 50.0,
                     'B_VtxChi2'               : 12.0,
                     'B_Dira'                  : 0.999,
                     'B_IPCHI2wrtPV'           : 8.0,
                     'B_FDwrtPV'               : 1.0,
                     'B_FDChi2'                : 50.0,
                     'GEC_MaxTracks'           : 250,
                     'HLT1Dec'                 : 'Hlt1(Two)?TrackMVADecision',
                     'HLT2Dec'                 : 'Hlt2Topo[234]BodyDecision',
                     'Prescale'                : 1.0,
                     'Postscale'               : 1.0
                     },
    'STREAMS' : ['Bhadron']
    }

class B2KsthhConf(LineBuilder) :
    """
    Builder of B(d,s)->Ksthh stripping Selection and StrippingLine.
    Constructs B0 -> Kst0 h+ h- Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> b2ksthhConf = B2KsthhConf('B2KsthhTest',config)
    >>> b2ksthhLines = b2ksthhConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKst2Kpi             : Kst -> K pi Selection object
    selB2Ksthh             : B(d,s) -> Kst0 h+ h- Selection object
    line                   : StrippingLine made out of selB2Ksthh
    lines                  : List of lines, [line]

    Exports as class data member:
    B2KsthhConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'Trk_GhostProb',
                              'Kstar_MassLo', 
                              'Kstar_MassHi', 
                              'B_Mlow',
                              'B_Mhigh',
                              'B_APTmin',
                              'B_PTmin',
                              'BDaug_MedPT_PT',
                              'BDaug_MaxPT_IP',
                              'BDaug_PTsum',
                              'B_IPCHI2sum',
                              'B_VtxChi2',
                              'B_Dira',
                              'B_IPCHI2wrtPV',
                              'B_FDwrtPV',
                              'B_FDChi2',
                              'GEC_MaxTracks',
                              'HLT1Dec',
                              'HLT2Dec',
                              'Prescale',
                              'Postscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        Hlt1Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT1Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}
                          
        Hlt2Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT2Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}
                          
        self.pions = Pions

        self.makeKst2Kpi( 'Kstfor'+name, config )

        self.makeB2Ksthh( name, config )

        line = StrippingLine(name+"Line",
                                     prescale  = config['Prescale'],
                                     postscale = config['Postscale'],
                                     selection = self.selB2Ksthh,
                                     HLT1 = Hlt1Filter,
                                     HLT2 = Hlt2Filter,
                                     FILTER = GECCode
                                     )

        self.registerLine(line)
        
    def makeKst2Kpi( self, name, config ) :
        # define all the cuts
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_GhostProb']

        _allCuts  = _trkChi2Cut1
        _allCuts += '&'+_trkChi2Cut2
        _allCuts += '&'+_trkGhostProbCut1
        _allCuts += '&'+_trkGhostProbCut2

        # make the filter
        _filterKst2Kpi = FilterDesktop( Code = _allCuts )

        _stdKst2Kpi = DataOnDemand(Location="Phys/StdVeryLooseDetachedKst2Kpi/Particles")

        # make and store the Selection object
        self.selKst2Kpi = Selection( name, Algorithm = _filterKst2Kpi, RequiredSelections = [_stdKst2Kpi] ) 


    def makeB2Ksthh( self, name, config ) :
        """
        Create and store either a B(d,s) -> Kst0 h+ h- Selection object
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
        _daugPtSumCut     = "((APT1+APT2+APT3)>%s*MeV)"                                                 % config['BDaug_PTsum']

        _combCuts = _aptCut+'&'+_daugPtSumCut+'&'+_daugMedPtCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_daugMaxPtIPCut

        _ptCut            = "(PT>%s*MeV)"                                                               % config['B_PTmin']
        _vtxChi2Cut       = "(VFASPF(VCHI2)<%s)"                                                        % config['B_VtxChi2']
        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_Dira']
        _ipChi2Cut        = "(MIPCHI2DV(PRIMARY)<%s)"                                                   % config['B_IPCHI2wrtPV']
        _fdCut            = "(VFASPF(VMINVDDV(PRIMARY))>%s)"                                            % config['B_FDwrtPV']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                                                            % config['B_FDChi2']
        _ipChi2SumCut     = "(SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='pi+') | (ABSID=='pi-')),0.0) > %s)"  % config['B_IPCHI2sum']

        _motherCuts = _ptCut+'&'+_vtxChi2Cut+'&'+_diraCut+'&'+_ipChi2Cut+'&'+_fdCut+'&'+_fdChi2Cut+'&'+_ipChi2SumCut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        _B.DecayDescriptors = [ "[B0 -> pi+ pi- K*(892)0]cc" ]

        self.selB2Ksthh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKst2Kpi, self.pions ])
