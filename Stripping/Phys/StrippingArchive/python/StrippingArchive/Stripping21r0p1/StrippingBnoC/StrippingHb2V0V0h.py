"""
Module for construction of Hb->V0V0h stripping Selections and StrippingLines.
Provides functions to build KS(Lambda)->DD, KS(Lambda)->LL and B selections.
Provides class Hb2V0V0hConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Exported symbols (use python help!):
   - Hb2V0V0hConf
"""

__author__ = ['Rafael Coutinho']
__date__ = '10/11/2015'
__version__ = 'Stripping24'
__all__ = 'Hb2V0V0hConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdAllNoPIDsPions as Pions

default_config = {
    'NAME'        : 'Hb2V0V0h',
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'Hb2V0V0hConf',
    'CONFIG'      : {
                  'Trk_Chi2'                : 4.0,
                  'Trk_GhostProb'           : 0.5,
                  'V0_DD_MassWindow'        : 30.0,
                  'V0_DD_VtxChi2'           : 12.0,
                  'V0_DD_FDChi2'            : 50.0,
                  'V0_LL_MassWindow'        : 20.0,
                  'V0_LL_VtxChi2'           : 12.0,
                  'V0_LL_FDChi2'            : 80.0,
                  'B_Mlow'                  : 1279.0,
                  'B_Mhigh'                 : 921.0,
                  'Hb_Mlow'                 : 400.0,
                  'Hb_Mhigh'                : 400.0,
                  'B_APTmin'                : 1000.0,
                  'B_Dira'                  : 0.999,
                  'B_VtxChi2'               : 12.0,
                  'B_FDChi2'                : 50.0,
                  'B_IPCHI2wrtPV'           : 12.0,
                  'GEC_MaxTracks'           : 250,
                  'HLT1Dec'                 : 'Hlt1(Two)?TrackMVADecision',
                  'HLT2Dec'                 : 'Hlt2Topo[234]BodyDecision',
                  'Prescale'                : 1.0,
                  'Prescale_SameSign'       : 1.0,
                  'Postscale'               : 1.0,
                  'RelatedInfoTools'        : [ { "Type" : "RelInfoConeVariables"
                                                , "ConeAngle" : 1.0
                                                , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                                                , "Location"  : 'P2ConeVar1'},
                                                { "Type" : "RelInfoConeVariables"
                                                , "ConeAngle" : 1.5
                                                , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                                                , "Location"  : 'P2ConeVar2'},
                                                { "Type" : "RelInfoConeVariables"
                                                , "ConeAngle" : 1.7
                                                , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                                                , "Location"  : 'P2ConeVar3'},
                                                {'Type' : 'RelInfoVertexIsolation'
                                                , 'Location': "VtxIsolationInfo"  }
                                                ]
                  },
    'STREAMS'     : [ 'Bhadron' ]
    }

class Hb2V0V0hConf(LineBuilder) :
    """
    Builder of Hb->V0V0h stripping Selection and StrippingLine.
    Constructs Hb -> V0 V0 h Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> hb2v0v0hConf  = Hb2V0V0hConf('Hb22V0V0hTest',config)
    >>> hb2v0v0hLines = hb2v0v0hConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selKS2DD               : KS -> Down Down Selection object
    selKS2LL               : KS -> Long Long Selection object
    selKS2LD               : KS -> Long Down Selection object
    selB2KSDDhh            : B -> KS(DD) h+ h- Selection object
    selB2KSLLhh            : B -> KS(LL) h+ h- Selection object
    selB2KSLDhh            : B -> KS(LD) h+ h- Selection object
    selB2KSDDhh_SameSign   : B -> KS(DD) h+(-) h+(-) Selection object
    selB2KSLLhh_SameSign   : B -> KS(LL) h+(-) h+(-) Selection object
    selB2KSLDhh_SameSign   : B -> KS(LD) h+(-) h+(-) Selection object
    dd_line                : StrippingLine made out of selB2KSDDhh
    ll_line                : StrippingLine made out of selB2KSLLhh
    ld_line                : StrippingLine made out of selB2KSLDhh
    dd_line_same           : StrippingLine made out of selB2KSDDhh_SameSign
    ll_line_same           : StrippingLine made out of selB2KSLLhh_SameSign
    ld_line_same           : StrippingLine made out of selB2KSLDhh_SameSign
    lines                  : List of lines, [dd_line, ll_line, ld_line, dd_line_same, ll_line_same, ld_line_same]

    Exports as class data member:
    B2KShhConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('Trk_Chi2',
                              'Trk_GhostProb',
                              'V0_DD_MassWindow',
                              'V0_DD_VtxChi2',
                              'V0_DD_FDChi2',
                              'V0_LL_MassWindow',
                              'V0_LL_VtxChi2',
                              'V0_LL_FDChi2',
                              'B_Mlow',
                              'B_Mhigh',
                              'Hb_Mlow', 
                              'Hb_Mhigh',
                              'B_APTmin',
                              'B_Dira',
                              'B_VtxChi2',    
                              'B_FDChi2',
                              'B_IPCHI2wrtPV',
                              'GEC_MaxTracks',
                              'HLT1Dec',
                              'HLT2Dec',
                              'Prescale',
                              'Prescale_SameSign',
                              'Postscale',
                              'RelatedInfoTools'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        Hlt1Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT1Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}
                          
        Hlt2Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT2Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}
                          
        # the input charged particles
        self.pions = Pions

        _v0_configs = ['KS', 'Lz']
        _v0_types   = ['DD', 'LL']

        # build the various V0 input lists
        self.selV0 = {}
        for v0_config in _v0_configs :
            self.selV0[v0_config] = {}
            for v0_type in _v0_types :
                self.selV0[v0_config][v0_type] = self.makeV0( 'V0for'+name+v0_config+v0_type, v0_config, v0_type, config )

        # build the V0V0h modes
        _v0v0_types = ['KSKS', 'KSLz', 'LzLz']
        _v0v0_combs = [ 'LL', 'LD', 'DL', 'DD' ]

        _selV0V0h  = {}
        _mylines_V0V0h = {}
        for v0v0_type in _v0v0_types :
            _selV0V0h[v0v0_type]  = {}
            _mylines_V0V0h[v0v0_type] = {}
            for v0v0_comb in _v0v0_combs :
                if ((v0v0_type != 'KSKS') | (v0v0_type != 'LzLz')) & (v0v0_comb != 'DL') :    
                    _selV0V0h[v0v0_type][v0v0_comb] = self.makeHb2V0V0h( name+v0v0_type+v0v0_comb+'h', v0v0_type, v0v0_comb, config )

                    _linename = name+'_'+v0v0_type+'h'+v0v0_comb+'_Line'

                    _mylines_V0V0h[v0v0_type][v0v0_comb] = StrippingLine(_linename,
                                           prescale = config['Prescale'],
                                           postscale = config['Postscale'],
                                           selection = _selV0V0h[v0v0_type][v0v0_comb],
                                           FILTER = GECCode,
                                           HLT1 = Hlt1Filter,
                                           HLT2 = Hlt2Filter,
                                           RelatedInfoTools = config['RelatedInfoTools'], 
                                           EnableFlavourTagging = False,
                                           MDSTFlag = True
                                           )

                    self.registerLine(_mylines_V0V0h[v0v0_type][v0v0_comb])

        # build the V0V0V0 modes
        _v0v0v0_types = ['KSKSKS', 'KSKSLz', 'LzLzKS', 'LzLzLz']
        _v0v0v0_combs = [ 'LLL', 'LLD', 'LDL', 'LDD', 'DDL', 'DDD']

        _selV0V0V0 = {}
        _mylines_V0V0V0 = {}
        for v0v0v0_type in _v0v0v0_types :
            _selV0V0V0[v0v0v0_type] = {}
            _mylines_V0V0V0[v0v0v0_type] = {}
            for v0v0v0_comb in _v0v0v0_combs :
                if ((v0v0v0_type != 'KSKSKS') | (v0v0v0_type != 'LzLzLz')) & ((v0v0v0_comb != 'LDL') | (v0v0v0_comb != 'DDL')) :
                    _selV0V0V0[v0v0v0_type][v0v0v0_comb] = self.makeHb2V0V0V0( name+v0v0v0_type+v0v0v0_comb, v0v0v0_type, v0v0v0_comb, config )

                    _linename = name+'_'+v0v0v0_type+v0v0v0_comb+'_Line'

                    _mylines_V0V0V0[v0v0v0_type][v0v0v0_comb] = StrippingLine(_linename,
                                           prescale = config['Prescale'],
                                           postscale = config['Postscale'],
                                           selection = _selV0V0V0[v0v0v0_type][v0v0v0_comb],
                                           FILTER = GECCode,
                                           RelatedInfoTools = config['RelatedInfoTools'], 
                                           EnableFlavourTagging = True,
                                           MDSTFlag = True
                                           )

                    self.registerLine(_mylines_V0V0V0[v0v0v0_type][v0v0v0_comb])


    def makeV0( self, name, v0_config, v0_type, config ) :

        # define all the cuts
        if v0_config != 'Lz':
            _massCut          = "(ADMASS('KS0')<%s*MeV)"     % config['V0_%s_MassWindow'%v0_type]
        else :
            _massCut          = "(ADMASS('Lambda0')<%s*MeV)" % config['V0_%s_MassWindow'%v0_type]
        _vtxCut           = "(VFASPF(VCHI2)<%s)"             % config['V0_%s_VtxChi2'%v0_type]
        _fdCut            = "(BPVVDCHI2>%s)"                 % config['V0_%s_FDChi2'%v0_type]
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_GhostProb']

        _allCuts = _massCut

        _allCuts += '&'+_trkChi2Cut1
        _allCuts += '&'+_trkChi2Cut2

        if v0_type == 'LL' :
            _allCuts += '&'+_trkGhostProbCut1
            _allCuts += '&'+_trkGhostProbCut2

        _allCuts += '&'+_vtxCut
        _allCuts += '&'+_fdCut

        # get the V0's to filter
        if v0_config == 'KS' :
            _stdV0 = DataOnDemand( Location = 'Phys/StdLooseKs%s/Particles' % v0_type )
        else :
            _stdV0 = DataOnDemand( Location = 'Phys/StdLooseLambda%s/Particles' % v0_type )

        # make the filter
        _filterV0 = FilterDesktop( Code = _allCuts )

        # make and return the Selection object
        return Selection( name, Algorithm = _filterV0, RequiredSelections = [_stdV0] )


    def makeHb2V0V0h( self, name, v0v0_type, v0v0_comb, config ) :
        """
        Create and store either a Hb+ -> V0(LL,DD) V0 (LL,DD) h+ Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut

        if v0v0_comb != 'KSLz':
            _massCutLow     = "(AM>(5279-%s)*MeV)"           % config['B_Mlow']
            _massCutHigh    = "(AM<(5279+%s)*MeV)"           % config['B_Mhigh']
        else :
            _massCutLow     = "(AM>(5791-%s)*MeV)"           % config['Hb_Mlow']
            _massCutHigh    = "(AM<(5791+%s)*MeV)"           % config['Hb_Mhigh']

        _aptCut         = "(APT>%s*MeV)"                     % config['B_APTmin']

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut        = "(BPVDIRA>%s)"                     % config['B_Dira']
        _vtxChi2Cut     = "(VFASPF(VCHI2)<%s)"               % config['B_VtxChi2']
        _fdChi2Cut      = "(BPVVDCHI2>%s)"                   % config['B_FDChi2']
        _ipChi2Cut      = "(MIPCHI2DV(PRIMARY)<%s)"          % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        if v0v0_comb == 'LL' : 
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISLONG, 2), 1)) & (CHILDCUT(CHILDCUT(ISLONG, 2), 2))"
        elif v0v0_comb == 'LD' :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISLONG, 2), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 2))"
        elif v0v0_comb == 'DL' :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISDOWN, 2), 1)) & (CHILDCUT(CHILDCUT(ISLONG, 2), 2))"
        else :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISDOWN, 2), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 2))"

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts
        _B.ReFitPVs       = True 

        _selname = name + '_' + v0v0_type + '_' + v0v0_comb + '_Sel'

        if v0v0_type == 'KSKS' :
            _B.DecayDescriptors = [ "B+ -> KS0 KS0 pi+" ]
            if v0v0_comb == 'LL' : 
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.pions ])
            elif v0v0_comb == 'LD' :
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['KS']['DD'], self.pions ])
            else :
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['DD'], self.pions ])

        elif v0v0_type == 'KSLz' :
            _B.DecayDescriptors = [ "Xi_b- -> KS0 Lambda0 pi-" ]
            if v0v0_comb == 'LL' : 
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['Lz']['LL'], self.pions ])
            elif v0v0_comb == 'LD' :
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['Lz']['DD'], self.pions ])
            elif v0v0_comb == 'DL' :
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['DD'], self.selV0['Lz']['LL'], self.pions ])
            else :
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['DD'], self.selV0['Lz']['DD'], self.pions ])

        else :
            _B.DecayDescriptors = [ "B+ -> Lambda0 Lambda0 pi+" ]
            if v0v0_comb == 'LL' : 
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.pions ])
            elif v0v0_comb == 'LD' :
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.selV0['Lz']['DD'], self.pions ])
            else :
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['DD'], self.pions ])


    def makeHb2V0V0V0( self, name, v0v0v0_type, v0v0v0_comb, config ) :
        """
        Create and store either a B -> V0(LL, DD) V0(LL, DD) V0(LL, DD) Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        if (v0v0v0_comb != 'KSKSLz') | (v0v0v0_comb != 'LzLzLz') :
            _massCutLow       = "(AM>(5279-%s)*MeV)"                                                    % config['B_Mlow']
            _massCutHigh      = "(AM<(5279+%s)*MeV)"                                                    % config['B_Mhigh']
        else : 
            _massCutLow       = "(AM>(5619-%s)*MeV)"                                                    % config['Hb_Mlow']
            _massCutHigh      = "(AM<(5619+%s)*MeV)"                                                    % config['Hb_Mhigh']

        _aptCut           = "(APT>%s*MeV)"                                                              % config['B_APTmin']

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_Dira']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                                                            % config['B_FDChi2']
        _ipChi2Cut        = "(MIPCHI2DV(PRIMARY)<%s)"                                                   % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        if v0v0v0_comb == 'LLL' : 
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISLONG, 2), 1)) & (CHILDCUT(CHILDCUT(ISLONG, 2), 2)) & (CHILDCUT(CHILDCUT(ISLONG, 2), 3))"
        elif v0v0v0_comb == 'LLD' :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISLONG, 2), 1)) & (CHILDCUT(CHILDCUT(ISLONG, 2), 2)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 3))"
        elif v0v0v0_comb == 'LDL' :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISLONG, 2), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 2)) & (CHILDCUT(CHILDCUT(ISLONG, 2), 3))"
        elif v0v0v0_comb == 'LDD' :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISLONG, 2), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 2)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 3))"
        elif v0v0v0_comb == 'DDL' :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISDOWN, 2), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 2)) & (CHILDCUT(CHILDCUT(ISLONG, 2), 3))"
        else :
            _motherCuts += "& (CHILDCUT(CHILDCUT(ISDOWN, 2), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 2)) & (CHILDCUT(CHILDCUT(ISDOWN, 2), 3))"

        _B = CombineParticles()
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts
        _B.ReFitPVs       = True 

        _selname = name + '_' + v0v0v0_type + '_' + v0v0v0_comb + '_Sel'

        if v0v0v0_type == 'KSKSKS' in name:
            _B.DecayDescriptors = [ "B0 -> KS0 KS0 KS0" ]
            if v0v0v0_comb == 'LLL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'] ])
            elif v0v0v0_comb == 'LLD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['KS']['DD'] ])
            elif v0v0v0_comb == 'LDD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['KS']['DD'] ])
            else:
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['DD'] ])

        elif v0v0v0_type == 'KSKSLz' :
            _B.DecayDescriptors = [ "Lambda_b0 -> KS0 KS0 Lambda0" ]
            if v0v0v0_comb == 'LLL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['Lz']['LL'] ])
            elif v0v0v0_comb == 'LLD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['Lz']['DD'] ])
            elif v0v0v0_comb == 'LDL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['KS']['DD'], self.selV0['Lz']['LL'] ])
            elif v0v0v0_comb == 'LDD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['LL'], self.selV0['KS']['DD'], self.selV0['Lz']['DD'] ])
            elif v0v0v0_comb == 'DDL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['DD'], self.selV0['Lz']['LL'] ])
            else:
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['KS']['DD'], self.selV0['Lz']['DD'] ])
            
        elif "LzLzKS" in name:
            _B.DecayDescriptors = [ "B0 -> KS0 Lambda0 Lambda0" ]
            if v0v0v0_comb == 'LLL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.selV0['KS']['LL'] ])
            elif v0v0v0_comb == 'LLD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.selV0['KS']['DD'] ])
            elif v0v0v0_comb == 'LDL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.selV0['Lz']['DD'], self.selV0['KS']['LL'] ])
            elif v0v0v0_comb == 'LDD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.selV0['Lz']['DD'], self.selV0['KS']['DD'] ])
            elif v0v0v0_comb == 'DDL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['DD'], self.selV0['KS']['LL'] ])
            else:
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['DD'], self.selV0['KS']['DD'] ])
            
        else:
            _B.DecayDescriptors = [ "Lambda_b0 -> Lambda0 Lambda0 Lambda0" ]
            if v0v0v0_comb == 'LLL':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'] ])
            elif v0v0v0_comb == 'LLD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.selV0['Lz']['DD'] ])
            elif v0v0v0_comb == 'LDD':
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['LL'], self.selV0['Lz']['DD'] ])
            else:
                return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selV0['Lz']['DD'] ])
            

