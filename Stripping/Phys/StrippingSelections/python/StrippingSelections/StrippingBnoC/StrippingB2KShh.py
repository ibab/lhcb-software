"""
Module for construction of B->KShh stripping Selections and StrippingLines.
Provides functions to build KS->DD, KS->LL, KS->LD abd B selections.
Provides class B2KShhConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Selections based on previous version B2KShh line but optimised for DP analysis.
Exported symbols (use python help!):
   - B2KShhConf
"""

__author__ = ['Thomas Latham','Rafael Coutinho']
__date__ = '20/05/2015'
__version__ = 'Stripping23'
__all__ = {'B2KShhConf',
           'default_config'}

from Gaudi.Configuration import *
from Configurables import CombineParticles 
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from Configurables import FilterDesktop as FilterDesktopConf
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from MVADictHelpers import *
from Configurables import LoKi__ODINFilter as ODINFilter
from Configurables import LoKi__Hybrid__DictValue as DictValue
from Configurables import LoKi__Hybrid__DictTransform_TMVATransform_ as TMVAClassifier
from Configurables import LoKi__Hybrid__DictOfFunctors

from StandardParticles import StdAllNoPIDsPions as Pions

default_config = {
    'NAME'        : 'B2KShh',
    'WGs'         : ['BnoC'],
    'BUILDERTYPE' : 'B2KShhConf',
    'CONFIG'      : {'Trk_Chi2'                : 3.0,
                     'Trk_GhostProb'           : 0.3,
                     'KS_DD_MassWindow'        : 30.0,
                     'KS_DD_VtxChi2'           : 12.0,
                     'KS_DD_FDChi2'            : 50.0,
                     'KS_DD_Pmin'              : 6000.0,
                     'KS_LL_MassWindow'        : 20.0,
                     'KS_LL_VtxChi2'           : 12.0,
                     'KS_LL_FDChi2'            : 80.0,
                     'KS_LL_Pmin'              : 0.0,
                     'KS_LD_MassWindow'        : 25.0,
                     'KS_LD_VtxChi2'           : 12.0,
                     'KS_LD_FDChi2'            : 50.0,
                     'KS_LD_Pmin'              : 6000.0,
                     'B_Mlow'                  : 1279.0,
                     'B_Mhigh'                 : 921.0,
                     'B_APTmin'                : 1000.0,
                     'BDaug_MedPT_PT'          : 800.0,
                     'BDaug_MaxPT_IP'          : 0.05,
                     'BDaug_DD_maxDocaChi2'    : 5.0,
                     'BDaug_LL_maxDocaChi2'    : 5.0,
                     'BDaug_LD_maxDocaChi2'    : 5.0,
                     'BDaug_DD_PTsum'          : 4200.0,
                     'BDaug_LL_PTsum'          : 3000.0,
                     'BDaug_LD_PTsum'          : 4200.0,
                     'B_DD_Dira'               : 0.999,
                     'B_LL_Dira'               : 0.999,
                     'B_LD_Dira'               : 0.999,
                     'KS_FD_Z'                 : 15.,
                     'B_DD_MVA'                : -0.20,
                     'B_LL_MVA'                : -0.20,   
                     'B_LD_MVA'                : -0.20,
                     'GEC_MaxTracks'           : 250,
                     'ConeAngle10'             : 1.0, 
                     'ConeAngle15'             : 1.5, 
                     'ConeAngle17'             : 1.7, 
                     # 2012 Triggers
                     #'HLT1Dec'                 : 'Hlt1TrackAllL0Decision',
                     #'HLT2Dec'                 : 'Hlt2Topo[234]Body.*Decision',
                     # 2015 Triggers
                     'HLT1Dec'                 : 'Hlt1(Two)?TrackMVADecision',
                     'HLT2Dec'                 : 'Hlt2Topo[234]BodyDecision',
                     'Prescale'                : 1.0,
                     'Prescale_SameSign'       : 1.0,
                     'Postscale'               : 1.0
                     },
    'STREAMS'     : { 'Bhadron' : ['StrippingB2KShh_DD_2015_OS_Line',
                                   'StrippingB2KShh_LL_2015_OS_Line',
                                   'StrippingB2KShh_LD_2015_OS_Line',
                                   'StrippingB2KShh_DD_2015_SS_Line',
                                   'StrippingB2KShh_LL_2015_SS_Line',
                                   'StrippingB2KShh_LD_2015_SS_Line',
                                   ] 
                      }
    }

class B2KShhConf(LineBuilder) :
    """
    Builder of B->KShh stripping Selection and StrippingLine.
    Constructs B -> KS h+ h- Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> b2kshhConf  = B2KShhConf('B2KShhTest',config)
    >>> b2kshhLines = b2kshhConf.lines
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
                              'KS_DD_MassWindow',
                              'KS_DD_VtxChi2',
                              'KS_DD_FDChi2',
                              'KS_DD_Pmin',
                              'KS_LL_MassWindow',
                              'KS_LL_VtxChi2',
                              'KS_LL_FDChi2',
                              'KS_LL_Pmin',
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
                              'KS_FD_Z',
                              'B_DD_MVA',  
                              'B_LL_MVA',
                              'B_LD_MVA', 
                              'GEC_MaxTracks',
                              'ConeAngle10',
                              'ConeAngle15',
                              'ConeAngle17',
                              'HLT1Dec',
                              'HLT2Dec',
                              'Prescale',
                              'Prescale_SameSign',
                              'Postscale'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        _ks_types = [ 'DD', 'LL', 'LD' ]
        _years = [ '2015' ]
        _signs = [ 'OS', 'SS' ]

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        Hlt1Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT1Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}
                          
        Hlt2Filter = {'Code' : "HLT_PASS_RE('%s')" % config['HLT2Dec'],
                     'Preambulo' : ["from LoKiCore.functions import *"]}

        relinfo_OS = [ { "Type" : "RelInfoConeVariables" 
                      , "ConeAngle" : config['ConeAngle10']
                      , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'] 
                      , 'Location'  : 'P2ConeVar10_B'
                      , "DaughterLocations" : { "B0 -> ^pi+ pi- (KS0 -> pi+ pi-)" : 'P2ConeVar10_1',
                                                "B0 -> pi+ ^pi- (KS0 -> pi+ pi-)" : 'P2ConeVar10_2',
                                                "B0 -> pi+ pi- (KS0 -> ^pi+ pi-)" : 'P2ConeVar10_3',
                                                "B0 -> pi+ pi- (KS0 -> pi+ ^pi-)" : 'P2ConeVar10_4'} },
                      { "Type" : "RelInfoConeVariables"
                      , "ConeAngle" : config['ConeAngle15'] 
                      , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                      , 'Location'  : 'P2ConeVar15_B'
                      , "DaughterLocations" : { "B0 -> ^pi+ pi- (KS0 -> pi+ pi-)" : 'P2ConeVar15_1',
                                                "B0 -> pi+ ^pi- (KS0 -> pi+ pi-)" : 'P2ConeVar15_2',
                                                "B0 -> pi+ pi- (KS0 -> ^pi+ pi-)" : 'P2ConeVar15_3',
                                                "B0 -> pi+ pi- (KS0 -> pi+ ^pi-)" : 'P2ConeVar15_4'} },
                      { "Type" : "RelInfoConeVariables"
                      , "ConeAngle" : config['ConeAngle17'] 
                      , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                      , 'Location'  : 'P2ConeVar17_B'
                      , "DaughterLocations" : { "B0 -> ^pi+ pi- (KS0 -> pi+ pi-)" : 'P2ConeVar17_1',
                                                "B0 -> pi+ ^pi- (KS0 -> pi+ pi-)" : 'P2ConeVar17_2',
                                                "B0 -> pi+ pi- (KS0 -> ^pi+ pi-)" : 'P2ConeVar17_3',
                                                "B0 -> pi+ pi- (KS0 -> pi+ ^pi-)" : 'P2ConeVar17_4'} },
                      { "Type"      : "RelInfoVertexIsolation"
                      , "Location"  : 'VtxIsolationVar'}
                     ]

        relinfo_SS = [ { "Type" : "RelInfoConeVariables" 
                      , "ConeAngle" : config['ConeAngle10']
                      , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'] 
                      , 'Location'  : 'P2ConeVar10_B'
                      , "DaughterLocations" : { "B0 -> ^[pi+]cc [pi+]cc (KS0 -> pi+ pi-)" : 'P2ConeVar10_1',
                                                "B0 -> [pi+]cc ^[pi+]cc (KS0 -> pi+ pi-)" : 'P2ConeVar10_2',
                                                "B0 -> [pi+]cc [pi+]cc (KS0 -> ^pi+ pi-)" : 'P2ConeVar10_3',
                                                "B0 -> [pi+]cc [pi+]cc (KS0 -> pi+ ^pi-)" : 'P2ConeVar10_4'} },
                      { "Type" : "RelInfoConeVariables"
                      , "ConeAngle" : config['ConeAngle15'] 
                      , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                      , 'Location'  : 'P2ConeVar15_B'
                      , "DaughterLocations" : { "B0 -> ^[pi+]cc [pi+]cc (KS0 -> pi+ pi-)" : 'P2ConeVar10_1',
                                                "B0 -> [pi+]cc ^[pi+]cc (KS0 -> pi+ pi-)" : 'P2ConeVar10_2',
                                                "B0 -> [pi+]cc [pi+]cc (KS0 -> ^pi+ pi-)" : 'P2ConeVar10_3',
                                                "B0 -> [pi+]cc [pi+]cc (KS0 -> pi+ ^pi-)" : 'P2ConeVar10_4'} },
                      { "Type" : "RelInfoConeVariables"
                      , "ConeAngle" : config['ConeAngle17'] 
                      , "Variables" : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
                      , 'Location'  : 'P2ConeVar17_B'
                      , "DaughterLocations" : { "B0 -> ^[pi+]cc [pi+]cc (KS0 -> pi+ pi-)" : 'P2ConeVar10_1',
                                                "B0 -> [pi+]cc ^[pi+]cc (KS0 -> pi+ pi-)" : 'P2ConeVar10_2',
                                                "B0 -> [pi+]cc [pi+]cc (KS0 -> ^pi+ pi-)" : 'P2ConeVar10_3',
                                                "B0 -> [pi+]cc [pi+]cc (KS0 -> pi+ ^pi-)" : 'P2ConeVar10_4'} },
                      { "Type"      : "RelInfoVertexIsolation"
                      , "Location"  : 'VtxIsolationVar'}
                     ]

        # the input charged particles
        self.pions = Pions

        # build the various KS input lists
        self.selKS = {}
        for ks_type in _ks_types :
            self.selKS[ks_type] = self.makeKS( 'KSfor'+name+ks_type, ks_type, config )

        # build the various B's
        _selB = {}
        _mvaCut = {}
        _mvaFilter = {}
        _weightfile1 = {}
        _weightfile2 = {}
        _mvaVars = {}
        _mylines = {}
        for ks_type in _ks_types :
            _selB[ks_type] = {}
            _mvaCut[ks_type] = {}
            _mvaFilter[ks_type] = {}
            _weightfile1[ks_type] = {}
            _weightfile2[ks_type] = {}
            _mvaVars[ks_type] = {}
            _mylines[ks_type] = {}

            for year in _years :
                _selB[ks_type][year] = {}
                _mvaCut[ks_type][year] = {}
                _mvaFilter[ks_type][year] = {}
                _mvaVars[ks_type][year] = {}
                _mylines[ks_type][year] = {}

                # BDT weight file
                _weightfile1[ks_type][year] = '$TMVAWEIGHTSROOT/data/B2KShh_BDT1_%s_%s_v1r4.xml' % (ks_type, '2012b')
                _weightfile2[ks_type][year] = '$TMVAWEIGHTSROOT/data/B2KShh_BDT2_%s_%s_v1r4.xml' % (ks_type, '2012b')

                for sign in _signs :

                    _linename = name+'_'+ks_type+'_'+year+'_'+sign+'_Line'

                    _selB[ks_type][year][sign] = self.makeB2KShh( name, ks_type, year, sign, config )

                    _mvaCut[ks_type][year][sign] = "(VALUE('LoKi::Hybrid::DictValue/MVA1Response_%s_%s_%s')>%s) | (VALUE('LoKi::Hybrid::DictValue/MVA2Response_%s_%s_%s')>%s)" % ( ks_type, '2012b', sign, config['B_%s_MVA'%(ks_type)], ks_type, '2012b', sign, config['B_%s_MVA'%(ks_type)] )

                    _mvaFilter[ks_type][year][sign] = FilterDesktopConf( name+'_MVAFilter_%s_%s_%s'%(ks_type,year,sign),  Code = _mvaCut[ks_type][year][sign],  Inputs = [ 'Phys/' + name + '_' + ks_type + '_' + year + '_' + sign + '_Line/Particles' ] )

                    # Input variables for BDT
                    _mvaVars[ks_type][year][sign] = {
                         "B_PT"                                                        : "PT"
                        ,"B_ETA"                                                       : "ETA"
                        ,"log10(B_IPCHI2_OWNPV)"                                       : "log10(BPVIPCHI2())"
                        ,"log10(B_VDCHI2_OWNPV)"                                       : "log10(BPVVDCHI2)"
                        ,"B_DIRA_OWNPV!=1.0?log10(1.0-TMath::Abs(B_DIRA_OWNPV)):-10.0" : "(log10(1-BPVDIRA) if (BPVDIRA != 1.0) else -10.0)"
                        ,"log10(B_ENDVERTEX_CHI2)"                                     : "log10(VFASPF(VCHI2))"
                        ,"log10(B_SMALLESTDELTACHI2)"                                  : "log10(RELINFO('/Event/Phys/"+_linename+"/VtxIsolationVar', 'VTXISODCHI2ONETRACK', -1.1))"
                        ,"B_PTASYM_1_5"                                                : "RELINFO('/Event/Phys/"+_linename+"/P2ConeVar15_B', 'CONEPTASYM', -1.1)"
                        ,"log10(h1_IPCHI2_OWNPV+h2_IPCHI2_OWNPV)"                      : "log10(CHILD(BPVIPCHI2(),1) + CHILD(BPVIPCHI2(),2))"
                    }
                    if ks_type == 'LL' :
                        _mvaVars[ks_type][year][sign]['log10(KS_VDCHI2_OWNPV)'] = 'log10(CHILD(BPVVDCHI2,3))'

                    # Configuration of the MVA tool
                    addTMVAclassifierValue(Component = _mvaFilter[ks_type][year][sign], XMLFile = _weightfile1[ks_type][year], Variables = _mvaVars[ks_type][year][sign], ToolName = 'MVA1Response_%s_%s_%s' % (ks_type,'2012b',sign) )
                    addTMVAclassifierValue(Component = _mvaFilter[ks_type][year][sign], XMLFile = _weightfile2[ks_type][year], Variables = _mvaVars[ks_type][year][sign], ToolName = 'MVA2Response_%s_%s_%s' % (ks_type,'2012b',sign) )

                    # Main Algorithm initialisation
                    _flavourFlag = True
                    if sign == 'SS' : 
                        _flavourFlag = False

                    # Related info configuration
                    relinfo = relinfo_OS
                    if sign == 'SS' :
                        relinfo = relinfo_SS

                    _mylines[ks_type][year][sign] = StrippingLine(_linename,
                                           prescale = config['Prescale'],
                                           postscale = config['Postscale'],
                                           selection = _selB[ks_type][year][sign],
                                           HLT1 = Hlt1Filter,
                                           HLT2 = Hlt2Filter,
                                           FILTER = GECCode,
                                           RelatedInfoTools = relinfo, 
                                           RelatedInfoFilter = _mvaFilter[ks_type][year][sign],
                                           EnableFlavourTagging = _flavourFlag,
                                           MDSTFlag = _flavourFlag
                                           )
        

                    self.registerLine(_mylines[ks_type][year][sign])


    def makeKS( self, name, ks_type, config ) :
        # define all the cuts
        _massCut          = "(ADMASS('KS0')<%s*MeV)"         % config['KS_%s_MassWindow'%ks_type]
        _vtxCut           = "(VFASPF(VCHI2)<%s)"             % config['KS_%s_VtxChi2'%ks_type]
        _fdCut            = "(BPVVDCHI2>%s)"                 % config['KS_%s_FDChi2'%ks_type]
        _momCut           = "(P>%s*MeV)"                     % config['KS_%s_Pmin'%ks_type]
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_GhostProb']

        _allCuts = _massCut

        if ks_type != 'LL' :
            _allCuts += '&'+_momCut

        _allCuts += '&'+_trkChi2Cut1
        _allCuts += '&'+_trkChi2Cut2

        if ks_type == 'LL' :
            _allCuts += '&'+_trkGhostProbCut1
            _allCuts += '&'+_trkGhostProbCut2

        _allCuts += '&'+_vtxCut
        _allCuts += '&'+_fdCut

        # get the KS's to filter
        _stdKS = DataOnDemand( Location = 'Phys/StdLooseKs%s/Particles' % ks_type )

        # make the filter
        _filterKS = FilterDesktop( Code = _allCuts )

        # make and return the Selection object
        return Selection( name, Algorithm = _filterKS, RequiredSelections = [_stdKS] )


    def makeB2KShh( self, name, ks_type, year, sign, config ) :
        """
        Create and return either a B -> KS h+ h- Selection object, or a B -> KS h+(-) h+(-) Same Sign Selection Object
        Arguments:
        name             : name of the Selection.
        ks_type          : type of the KS, e.g. DD
        year             : the year for which we are making the selection
        sign             : whether we use opposite-sign or same-sign h's
        config           : config dictionary
        """

        _trkChi2Cut      = "(TRCHI2DOF<%s)"   % config['Trk_Chi2']
        _trkGhostProbCut = "(TRGHOSTPROB<%s)" % config['Trk_GhostProb']

        _daughtersCuts = _trkChi2Cut+'&'+_trkGhostProbCut

        _massCutLow     = "(AM>(5279-%s)*MeV)"                                                        % config['B_Mlow']
        _massCutHigh    = "(AM<(5279+%s)*MeV)"                                                        % config['B_Mhigh']
        _aptCut         = "(APT>%s*MeV)"                                                              % config['B_APTmin']
        _daugMedPtCut   = "(ANUM(PT>%s*MeV)>=2)"                                                      % config['BDaug_MedPT_PT']
        _daugMaxPtIPCut = "(AVAL_MAX(MIPDV(PRIMARY),PT)>%s)"                                          % config['BDaug_MaxPT_IP']
        _maxDocaChi2Cut = "(ACUTDOCACHI2(%s,''))"                                                     % config['BDaug_%s_maxDocaChi2' % ks_type]
        _daugPtSumCut   = "((APT1+APT2+APT3)>%s*MeV)"                                                 % config['BDaug_%s_PTsum' % ks_type]

        _combCuts = _daugPtSumCut+'&'+_massCutLow+'&'+_massCutHigh+'&'+_maxDocaChi2Cut+'&'+_aptCut+'&'+_daugMedPtCut+'&'+_daugMaxPtIPCut

        _diraCut        = "(BPVDIRA>%s)"                                 % config['B_%s_Dira' % ks_type]
        _KSdiffZ        = "((CHILD(VFASPF(VZ),3) - VFASPF(VZ)) > %s*mm)" % config['KS_FD_Z']

        _motherCuts = _diraCut+'&'+_KSdiffZ

        _combName = name + '_' + ks_type + '_' + year + '_' + sign + '_Comb3Body'
        _B = CombineParticles( _combName )
        _B.DaughtersCuts  = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut      = _motherCuts
        _B.ReFitPVs       = True 

        if sign == 'OS' :
            _B.DecayDescriptors = [ "B0 -> pi+ pi- KS0" ]
        else:
            _B.DecayDescriptors = [ "B0 -> pi+ pi+ KS0", "B0 -> pi- pi- KS0" ]

        _selname = name + '_' + ks_type + '_' + year + '_' + sign + '_Presel'

        return Selection (_selname, Algorithm = _B, RequiredSelections = [ self.selKS[ks_type], self.pions ])

