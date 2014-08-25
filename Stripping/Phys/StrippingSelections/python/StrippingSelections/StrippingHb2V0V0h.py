"""
Module for construction of Hb->V0V0h stripping Selections and StrippingLines.
Provides functions to build KS(Lambda)->DD, KS(Lambda)->LL and B selections.
Provides class Hb2V0V0hConf, which constructs the Selections and StrippingLines
given a configuration dictionary.
Exported symbols (use python help!):
   - Hb2V0V0hConf
"""

__author__ = ['Rafael Coutinho']
__date__ = '18/08/2014'
__version__ = 'Stripping21'
__all__ = 'Hb2V0V0hConf'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

from StandardParticles import StdAllNoPIDsPions as Pions

default_config = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Hb2V0V0hConf',
    'CONFIG'      : {
                  'Trk_Chi2'                : 3.0,
                  'Trk_GhostProb'           : 0.3,
                  'V0_DD_MassWindow'        : 30.0,
                  'V0_DD_VtxChi2'           : 12.0,
                  'V0_DD_FDChi2'            : 50.0,
                  'V0_DD_Pmin'              : 6000.0,
                  'V0_LL_MassWindow'        : 20.0,
                  'V0_LL_VtxChi2'           : 12.0,
                  'V0_LL_FDChi2'            : 80.0,
                  'B_Mlow'                  : 1279.0,
                  'B_Mhigh'                 : 921.0,
                  'B_APTmin'                : 1000.0,
                  'BDaug_MedPT_PT'          : 800.0,
                  'BDaug_MaxPT_IP'          : 0.05,
                  'BDaug_DD_maxDocaChi2'    : 5.0,
                  'BDaug_LL_maxDocaChi2'    : 5.0,
                  'BDaug_DD_PTsum'          : 3000.0,
                  'BDaug_LL_PTsum'          : 3000.0,
                  'B_Dira'                  : 0.999,
                  'B_VtxChi2'               : 12.0,
                  'B_FDChi2'                : 50.0,
                  'B_IPCHI2wrtPV'           : 8.0,
                  'GEC_MaxTracks'           : 250,
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
                                                , "Location"  : 'P2ConeVar2'},
                                                {'Type' : 'RelInfoVertexIsolation'
                                                , 'Location': "VtxIsolationInfo"  }
                                                ]
                  },
    'STREAMS'     : { 'Bhadron' : ['StrippingHb2V0V0h_KSKShLL_Line',
                                   'StrippingHb2V0V0h_KSLzhLL_Line',
                                   'StrippingHb2V0V0h_LzLzhLL_Line',
                                   'StrippingHb2V0V0h_KSKShLD_Line',
                                   'StrippingHb2V0V0h_KSLzhLD_Line',
                                   'StrippingHb2V0V0h_KSLzhDL_Line',
                                   'StrippingHb2V0V0h_LzLzhLD_Line',
                                   'StrippingHb2V0V0h_KSKShDD_Line',
                                   'StrippingHb2V0V0h_KSLzhDD_Line',
                                   'StrippingHb2V0V0h_LzLzhDD_Line',
                                   'StrippingHb2V0V0h_KSKSKSLLL_Line',
                                   'StrippingHb2V0V0h_KSKSLzLLL_Line',
                                   'StrippingHb2V0V0h_KSLzLzLLL_Line',
                                   'StrippingHb2V0V0h_LzLzLzLLL_Line',
                                   'StrippingHb2V0V0h_KSKSKSLLD_Line',
                                   'StrippingHb2V0V0h_KSKSLzLLD_Line',
                                   'StrippingHb2V0V0h_KSKSLzLDL_Line',
                                   'StrippingHb2V0V0h_KSLzLzLLD_Line',
                                   'StrippingHb2V0V0h_KSLzLzDLL_Line',
                                   'StrippingHb2V0V0h_LzLzLzLLD_Line',
                                   'StrippingHb2V0V0h_KSKSKSLDD_Line',
                                   'StrippingHb2V0V0h_KSKSLzLDD_Line',
                                   'StrippingHb2V0V0h_KSKSLzDDL_Line',
                                   'StrippingHb2V0V0h_KSLzLzLDD_Line',
                                   'StrippingHb2V0V0h_KSLzLzDLD_Line',
                                   'StrippingHb2V0V0h_LzLzLzLDD_Line',
                                   'StrippingHb2V0V0h_KSKSKSDDD_Line',
                                   'StrippingHb2V0V0h_KSKSLzDDD_Line',
                                   'StrippingHb2V0V0h_KSLzLzDDD_Line',
                                   'StrippingHb2V0V0h_LzLzLzDDD_Line',
                                   ] 
                    }
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
                              'V0_DD_Pmin',
                              'V0_LL_MassWindow',
                              'V0_LL_VtxChi2',
                              'V0_LL_FDChi2',
                              'B_Mlow',
                              'B_Mhigh',
                              'B_APTmin',
                              'BDaug_MedPT_PT',
                              'BDaug_MaxPT_IP',
                              'BDaug_DD_maxDocaChi2',
                              'BDaug_LL_maxDocaChi2',
                              'BDaug_DD_PTsum',
                              'BDaug_LL_PTsum',
                              'B_Dira',
                              'B_VtxChi2',    
                              'B_FDChi2',
                              'B_IPCHI2wrtPV',
                              'GEC_MaxTracks',
                              'Prescale',
                              'Prescale_SameSign',
                              'Postscale',
                              'RelatedInfoTools'
                              )

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        dd_name = name+'DD'
        ll_name = name+'LL'
        ld_name = name+'LD'

        GECCode = {'Code' : "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" % config['GEC_MaxTracks'],
                   'Preambulo' : ["from LoKiTracks.decorators import *"]}

        self.pions = Pions

        self.makeKS2DD( 'KS'+dd_name, config )
        self.makeKS2LL( 'KS'+ll_name, config )
        self.makeLz2DD( 'Lz'+dd_name, config )
        self.makeLz2LL( 'Lz'+ll_name, config )

        # V0V0 Quasi-2Body
        V0Combs = [ 'KSKS', 'KSLz', 'LzLz' ]
        V0Types = [ 'LL', 'LD', 'DD' ]
        for V0Comb in V0Combs:
            for V0Type in V0Types:
                self.makeV0V0( name+V0Type+'_'+V0Comb+'_Q2Body', config )
        # Additional confi for KSLz (DL)
        self.makeV0V0( name+'LD_LzKS_Q2Body', config )

        # V0V0h lines 
        for V0Comb in V0Combs:
            self.makeHb2V0V0LLh( name+'LL_'+V0Comb+'_Sel', config )
            self.makeHb2V0V0LDh( name+'LD_'+V0Comb+'_Sel', config )
            self.makeHb2V0V0DDh( name+'DD_'+V0Comb+'_Sel', config )
        # Additional confi for KSLz (DL)
        self.makeHb2V0V0LDh( name+'LD_LzKS_Sel', config )

        # V0V0V0 lines
        V0V0V0Combs = [ 'KSKSKS', 'KSKSLz', 'KSLzLz', 'LzLzLz' ]
        for V0V0V0Comb in V0V0V0Combs:
            self.makeHb2V0V0V0LLL( name+'LLL_'+V0V0V0Comb+'_Sel', config )
            self.makeHb2V0V0V0LLD( name+'LLD_'+V0V0V0Comb+'_Sel', config )
            self.makeHb2V0V0V0LDD( name+'LDD_'+V0V0V0Comb+'_Sel', config )
            self.makeHb2V0V0V0DDD( name+'DDD_'+V0V0V0Comb+'_Sel', config )
        # Extra combinations
        self.makeHb2V0V0V0LLD( name+'LLD_KSLzKS_Sel', config )
        self.makeHb2V0V0V0LLD( name+'LLD_LzLzKS_Sel', config )
        self.makeHb2V0V0V0LDD( name+'LDD_LzKSKS_Sel', config )
        self.makeHb2V0V0V0LDD( name+'LDD_LzKSLz_Sel', config )

        # Main algorithm
        self.KSKSh_LL_line = StrippingLine(name+"_KSKShLL_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2KSKSLLh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.KSLzh_LL_line = StrippingLine(name+"_KSLzhLL_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2KSLzLLh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.LzLzh_LL_line = StrippingLine(name+"_LzLzhLL_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2LzLzLLh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.KSKSh_LD_line = StrippingLine(name+"_KSKShLD_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2KSKSLDh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.KSLzh_LD_line = StrippingLine(name+"_KSLzhLD_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2KSLzLDh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.KSLzh_DL_line = StrippingLine(name+"_KSLzhDL_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2KSLzDLh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.LzLzh_LD_line = StrippingLine(name+"_LzLzhLD_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2LzLzLDh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.KSKSh_DD_line = StrippingLine(name+"_KSKShDD_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2KSKSDDh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.KSLzh_DD_line = StrippingLine(name+"_KSLzhDD_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2KSLzDDh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        self.LzLzh_DD_line = StrippingLine(name+"_LzLzhDD_Line"
                                          , prescale = config['Prescale']
                                          , postscale = config['Postscale']
                                          , selection = self.selH2LzLzDDh
                                          , FILTER = GECCode
                                          , RelatedInfoTools = config['RelatedInfoTools']
                                          , EnableFlavourTagging = True
                                          , MDSTFlag = True
                                          )
        # V0V0V0 lines
        self.KSKSKS_LLL_line = StrippingLine(name+"_KSKSKSLLL_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSKSLLL
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSLz_LLL_line = StrippingLine(name+"_KSKSLzLLL_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSLzLLL
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSLzLz_LLL_line = StrippingLine(name+"_KSLzLzLLL_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSLzLzLLL
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.LzLzLz_LLL_line = StrippingLine(name+"_LzLzLzLLL_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2LzLzLzLLL
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSKS_LLD_line = StrippingLine(name+"_KSKSKSLLD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSKSLLD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSLz_LLD_line = StrippingLine(name+"_KSKSLzLLD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSLzLLD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSLz_LDL_line = StrippingLine(name+"_KSKSLzLDL_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSLzLDL
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSLzLz_LLD_line = StrippingLine(name+"_KSLzLzLLD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSLzLzLLD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSLzLz_DLL_line = StrippingLine(name+"_KSLzLzDLL_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSLzLzDLL
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.LzLzLz_LLD_line = StrippingLine(name+"_LzLzLzLLD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2LzLzLzLLD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSKS_LDD_line = StrippingLine(name+"_KSKSKSLDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSKSLDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSLz_LDD_line = StrippingLine(name+"_KSKSLzLDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSLzLDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSLz_DDL_line = StrippingLine(name+"_KSKSLzDDL_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSLzDDL
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSLzLz_LDD_line = StrippingLine(name+"_KSLzLzLDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSLzLzLDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSLzLz_DLD_line = StrippingLine(name+"_KSLzLzDLD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSLzLzDLD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.LzLzLz_LDD_line = StrippingLine(name+"_LzLzLzLDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2LzLzLzLDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSKS_DDD_line = StrippingLine(name+"_KSKSKSDDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSKSDDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSKSLz_DDD_line = StrippingLine(name+"_KSKSLzDDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSKSLzDDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.KSLzLz_DDD_line = StrippingLine(name+"_KSLzLzDDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2KSLzLzDDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )
        self.LzLzLz_DDD_line = StrippingLine(name+"_LzLzLzDDD_Line"
                                           , prescale = config['Prescale']
                                           , postscale = config['Postscale']
                                           , selection = self.selH2LzLzLzDDD
                                           , FILTER = GECCode
                                           , RelatedInfoTools = config['RelatedInfoTools']
                                           , EnableFlavourTagging = True
                                           , MDSTFlag = True
                                           )

        # V0V0h lines
        self.registerLine(self.KSKSh_LL_line)
        self.registerLine(self.KSLzh_LL_line)
        self.registerLine(self.LzLzh_LL_line)
        self.registerLine(self.KSKSh_LD_line)
        self.registerLine(self.KSLzh_LD_line)
        self.registerLine(self.KSLzh_DL_line)
        self.registerLine(self.LzLzh_LD_line)
        self.registerLine(self.KSKSh_DD_line)
        self.registerLine(self.KSLzh_DD_line)
        self.registerLine(self.LzLzh_DD_line)
        # V0V0V0 lines
        self.registerLine(self.KSKSKS_LLL_line)
        self.registerLine(self.KSKSLz_LLL_line)
        self.registerLine(self.KSLzLz_LLL_line)
        self.registerLine(self.LzLzLz_LLL_line)
        self.registerLine(self.KSKSKS_LLD_line)
        self.registerLine(self.KSKSLz_LLD_line)
        self.registerLine(self.KSKSLz_LDL_line)
        self.registerLine(self.KSLzLz_LLD_line)
        self.registerLine(self.KSLzLz_DLL_line)
        self.registerLine(self.LzLzLz_LLD_line)
        self.registerLine(self.KSKSKS_LDD_line)
        self.registerLine(self.KSKSLz_LDD_line)
        self.registerLine(self.KSKSLz_DDL_line)
        self.registerLine(self.KSLzLz_LDD_line)
        self.registerLine(self.KSLzLz_DLD_line)
        self.registerLine(self.LzLzLz_LDD_line)
        self.registerLine(self.KSKSKS_DDD_line)
        self.registerLine(self.KSKSLz_DDD_line)
        self.registerLine(self.KSLzLz_DDD_line)
        self.registerLine(self.LzLzLz_DDD_line)

        
    def makeKS2LL( self, name, config ) :
        # define all the cuts
        _massCut          = "(ADMASS('KS0')<%s*MeV)"         % config['V0_LL_MassWindow']
        _vtxCut           = "(VFASPF(VCHI2)<%s)"             % config['V0_LL_VtxChi2']
        _fdCut            = "(BPVVDCHI2>%s)"                 % config['V0_LL_FDChi2']
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


    def makeKS2DD( self, name, config ) :
        # define all the cuts
        _massCut          = "(ADMASS('KS0')<%s*MeV)"         % config['V0_DD_MassWindow']
        _vtxCut           = "(VFASPF(VCHI2)<%s)"             % config['V0_DD_VtxChi2']
        _fdCut            = "(BPVVDCHI2>%s)"                 % config['V0_DD_FDChi2']
        _momCut           = "(P>%s*MeV)"                     % config['V0_DD_Pmin']
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']

        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2

        # get the KS's to filter
        _stdKSDD = DataOnDemand( Location = "Phys/StdLooseKsDD/Particles" )

        # make the filter
        _filterKSDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selKS2DD = Selection( name, Algorithm = _filterKSDD, RequiredSelections = [_stdKSDD] )


    def makeLz2LL( self, name, config ) : 
        # define all the cuts
        _massCut          = "(ADMASS('Lambda0')<%s*MeV)"     % config['V0_LL_MassWindow']
        _vtxCut           = "(VFASPF(VCHI2)<%s)"             % config['V0_LL_VtxChi2']
        _fdCut            = "(BPVVDCHI2>%s)"                 % config['V0_LL_FDChi2']
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']
        _trkGhostProbCut1 = "(CHILDCUT((TRGHOSTPROB<%s),1))" % config['Trk_GhostProb']
        _trkGhostProbCut2 = "(CHILDCUT((TRGHOSTPROB<%s),2))" % config['Trk_GhostProb']

        _allCuts = _massCut+'&'+_trkChi2Cut1+'&'+_trkChi2Cut2+'&'+_vtxCut+'&'+_fdCut

        # get the Lambda's to filter
        _stdLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")

        # make the filter
        _filterLambdaLL = FilterDesktop( Code = _allCuts )
        
        # make and store the Selection object
        self.selLambda2LL = Selection( name, Algorithm = _filterLambdaLL, RequiredSelections = [_stdLambdaLL] )


    def makeLz2DD( self, name, config ) :
        # define all the cuts
        _massCut          = "(ADMASS('Lambda0')<%s*MeV)"     % config['V0_DD_MassWindow']
        _vtxCut           = "(VFASPF(VCHI2)<%s)   "          % config['V0_DD_VtxChi2']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                 % config['V0_DD_FDChi2']
        _momCut           = "(P>%s*MeV)"                     % config['V0_DD_Pmin']
        _trkChi2Cut1      = "(CHILDCUT((TRCHI2DOF<%s),1))"   % config['Trk_Chi2']
        _trkChi2Cut2      = "(CHILDCUT((TRCHI2DOF<%s),2))"   % config['Trk_Chi2']

        _allCuts = _momCut+'&'+_massCut+'&'+_vtxCut+'&'+_fdChi2Cut

        # get the Lambda0's to filter
        _stdLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
        
        # make the filter
        _filterLambdaDD = FilterDesktop( Code = _allCuts )

        # make and store the Selection object
        self.selLambda2DD = Selection( name, Algorithm = _filterLambdaDD, RequiredSelections = [_stdLambdaDD] )
        

    def makeV0V0( self, name, config ) :
        """
        Create a fake quasi-two X0 -> V0 V0 Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """
        _Q = CombineParticles()
        _Q.MotherCut = "ALL"
        
        if "LL" in name:
            if "KSKS" in name: 
                _Q.DecayDescriptors = [ "phi(1020) -> KS0 KS0" ]
                self.selKSKSLL = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selKS2LL ])
            elif "KSLz" in name: 
                _Q.DecayDescriptors = [ "phi(1020) -> KS0 Lambda0" ]
                self.selKSLzLL = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selKS2LL, self.selLambda2LL ])
            else:
                _Q.DecayDescriptors = [ "phi(1020) -> Lambda0 Lambda0" ]
                self.selLzLzLL = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selLambda2LL ])
        elif "LD" in name:
            if "KSKS" in name: 
                _Q.DecayDescriptors = [ "phi(1020) -> KS0 KS0" ]
                _Q.MotherCut = "(CHILDCUT(CHILDCUT(ISLONG, 1), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 1), 2))"
                self.selKSKSLD = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selKS2LL, self.selKS2DD ])
            elif "KSLz" in name: 
                _Q.DecayDescriptors = [ "phi(1020) -> KS0 Lambda0" ]
                self.selKSLzLD = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selKS2LL, self.selLambda2DD ])
            elif "LzKS" in name:
                _Q.DecayDescriptors = [ "phi(1020) -> KS0 Lambda0" ]
                self.selKSLzDL = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selLambda2LL, self.selKS2DD ])
            else:
                _Q.DecayDescriptors = [ "phi(1020) -> Lambda0 Lambda0" ]
                _Q.MotherCut = "(CHILDCUT(CHILDCUT(ISLONG, 1), 1)) & (CHILDCUT(CHILDCUT(ISDOWN, 1), 2))"
                self.selLzLzLD = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selLambda2LL, self.selLambda2DD ])
        else:
            if "KSKS" in name: 
                _Q.DecayDescriptors = [ "phi(1020) -> KS0 KS0" ]
                self.selKSKSDD = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selKS2DD ])
            elif "KSLz" in name: 
                _Q.DecayDescriptors = [ "phi(1020) -> KS0 Lambda0" ]
                self.selKSLzDD = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selKS2DD, self.selLambda2DD ])
            else:
                _Q.DecayDescriptors = [ "phi(1020) -> Lambda0 Lambda0" ]
                self.selLzLzDD = Selection (name, Algorithm = _Q, RequiredSelections = [ self.selLambda2DD ])



    def makeHb2V0V0LLh( self, name, config ) :
        """
        Create and store either a Hb+ -> V0(LL) V0 (LL) h+ Selection object
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

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut        = "(BPVDIRA>%s)"                     % config['B_Dira']
        _vtxChi2Cut     = "(VFASPF(VCHI2)<%s)"               % config['B_VtxChi2']
        _fdChi2Cut      = "(BPVVDCHI2>%s)"                   % config['B_FDChi2']
        _ipChi2Cut      = "(MIPCHI2DV(PRIMARY)<%s)"          % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        _B.DecayDescriptors = [ "B+ -> phi(1020) pi+" ]

        if "KSKS" in name:
            self.selH2KSKSLLh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLL, self.pions ])

        elif "LzLz" in name:
            self.selH2LzLzLLh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzLL, self.pions ])

        else:
            self.selH2KSLzLLh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzLL, self.pions ])

    def makeHb2V0V0LDh( self, name, config ) :
        """
        Create and store either a Hb+ -> V0(LL) V0(DD) h+ Selection object
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

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut        = "(BPVDIRA>%s)"                     % config['B_Dira']
        _vtxChi2Cut     = "(VFASPF(VCHI2)<%s)"               % config['B_VtxChi2']
        _fdChi2Cut      = "(BPVVDCHI2>%s)"                   % config['B_FDChi2']
        _ipChi2Cut      = "(MIPCHI2DV(PRIMARY)<%s)"          % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        _B.DecayDescriptors = [ "B+ -> phi(1020) pi+" ]

        if "KSKS" in name:
            self.selH2KSKSLDh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLD, self.pions ])

        elif "LzLz" in name:
            self.selH2LzLzLDh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzLD, self.pions ])

        elif "KSLz" in name:
            self.selH2KSLzLDh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzLD, self.pions ])

        else:
            self.selH2KSLzDLh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzDL, self.pions ])


    def makeHb2V0V0DDh( self, name, config ) :
        """
        Create and store either a Hb+ -> V0(DD) V0(DD) h+ Selection object
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

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut        = "(BPVDIRA>%s)"                     % config['B_Dira']
        _vtxChi2Cut     = "(VFASPF(VCHI2)<%s)"               % config['B_VtxChi2']
        _fdChi2Cut      = "(BPVVDCHI2>%s)"                   % config['B_FDChi2']
        _ipChi2Cut      = "(MIPCHI2DV(PRIMARY)<%s)"          % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        _B = CombineParticles()
        _B.DaughtersCuts = { "pi+" : _daughtersCuts }
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        _B.DecayDescriptors = [ "B+ -> phi(1020) pi+" ]
        if "KSKS" in name:
            self.selH2KSKSDDh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSDD, self.pions ])

        elif "LzLz" in name:
            self.selH2LzLzDDh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzDD, self.pions ])

        else:
            self.selH2KSLzDDh = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzDD, self.pions ])


    def makeHb2V0V0V0LLL( self, name, config ) :
        """
        Create and store either a B -> V0(LL) V0(LL) V0(LL) Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow       = "(AM>(5279-%s)*MeV)"                                                        % config['B_Mlow']
        _massCutHigh      = "(AM<(5279+%s)*MeV)"                                                        % config['B_Mhigh']
        _aptCut           = "(APT>%s*MeV)"                                                              % config['B_APTmin']

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_Dira']
        _vtxChi2Cut       = "(VFASPF(VCHI2)<%s)"                                                        % config['B_VtxChi2']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                                                            % config['B_FDChi2']
        _ipChi2Cut        = "(MIPCHI2DV(PRIMARY)<%s)"                                                   % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        _B = CombineParticles()
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "KSKSKS" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) KS0" ]
            
            self.selH2KSKSKSLLL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLL, self.selKS2LL ])

        elif "KSKSLz" in name:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2KSKSLzLLL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLL, self.selLambda2LL ])

        elif "KSLzLz" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) Lambda0" ]
            
            self.selH2KSLzLzLLL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzLL, self.selLambda2LL ])

        else:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2LzLzLzLLL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzLL, self.selLambda2LL ])


    def makeHb2V0V0V0LLD( self, name, config ) :
        """
        Create and store either a B -> V0(LL) V0(LL) V0(DD) Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow       = "(AM>(5279-%s)*MeV)"                                                        % config['B_Mlow']
        _massCutHigh      = "(AM<(5279+%s)*MeV)"                                                        % config['B_Mhigh']
        _aptCut           = "(APT>%s*MeV)"                                                              % config['B_APTmin']

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_Dira']
        _vtxChi2Cut       = "(VFASPF(VCHI2)<%s)"                                                        % config['B_VtxChi2']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                                                            % config['B_FDChi2']
        _ipChi2Cut        = "(MIPCHI2DV(PRIMARY)<%s)"                                                   % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        _B = CombineParticles()
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "KSKSKS" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) KS0" ]
            
            self.selH2KSKSKSLLD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLL, self.selKS2DD ])

        elif "KSKSLz" in name:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2KSKSLzLLD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLL, self.selLambda2DD ])

        elif "KSLzKS" in name:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) KS0" ]
            
            self.selH2KSKSLzLDL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzLL, self.selKS2DD ])

        elif "KSLzLz" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) Lambda0" ]
            
            self.selH2KSLzLzLLD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzLL, self.selLambda2DD ])

        elif "LzLzKS" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) KS0" ]
            
            self.selH2KSLzLzDLL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzLL, self.selKS2DD ])

        else:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2LzLzLzLLD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzLL, self.selLambda2DD ])


    def makeHb2V0V0V0LDD( self, name, config ) :
        """
        Create and store either a B -> V0(LL) V0(DD) V0(DD) Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow       = "(AM>(5279-%s)*MeV)"                                                        % config['B_Mlow']
        _massCutHigh      = "(AM<(5279+%s)*MeV)"                                                        % config['B_Mhigh']
        _aptCut           = "(APT>%s*MeV)"                                                              % config['B_APTmin']

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_Dira']
        _vtxChi2Cut       = "(VFASPF(VCHI2)<%s)"                                                        % config['B_VtxChi2']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                                                            % config['B_FDChi2']
        _ipChi2Cut        = "(MIPCHI2DV(PRIMARY)<%s)"                                                   % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        _B = CombineParticles()
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "KSKSKS" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) KS0" ]
            
            self.selH2KSKSKSLDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLD, self.selKS2DD ])

        elif "KSKSLz" in name:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2KSKSLzLDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSLD, self.selLambda2DD ])

        elif "LzKSKS" in name:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) KS0" ]
            
            self.selH2KSKSLzDDL = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzDL, self.selKS2DD ])

        elif "KSLzLz" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) Lambda0" ]
            
            self.selH2KSLzLzLDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzLD, self.selLambda2DD ])

        elif "LzKSLz" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) Lambda0" ]
            
            self.selH2KSLzLzDLD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzDL, self.selLambda2DD ])

        else:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2LzLzLzLDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzLD, self.selLambda2DD ])

    def makeHb2V0V0V0DDD( self, name, config ) :
        """
        Create and store either a B -> V0(DD) V0(DD) V0(DD) Selection object
        Arguments:
        name             : name of the Selection.
        config           : config dictionary
        """

        _massCutLow       = "(AM>(5279-%s)*MeV)"                                                        % config['B_Mlow']
        _massCutHigh      = "(AM<(5279+%s)*MeV)"                                                        % config['B_Mhigh']
        _aptCut           = "(APT>%s*MeV)"                                                              % config['B_APTmin']

        _combCuts = _massCutLow+'&'+_massCutHigh+'&'+_aptCut

        _diraCut          = "(BPVDIRA>%s)"                                                              % config['B_Dira']
        _vtxChi2Cut       = "(VFASPF(VCHI2)<%s)"                                                        % config['B_VtxChi2']
        _fdChi2Cut        = "(BPVVDCHI2>%s)"                                                            % config['B_FDChi2']
        _ipChi2Cut        = "(MIPCHI2DV(PRIMARY)<%s)"                                                   % config['B_IPCHI2wrtPV']

        _motherCuts = _diraCut +'&'+_vtxChi2Cut +'&'+_fdChi2Cut+'&'+_ipChi2Cut

        _B = CombineParticles()
        _B.CombinationCut = _combCuts
        _B.MotherCut = _motherCuts

        if "KSKSKS" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) KS0" ]
            
            self.selH2KSKSKSDDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSDD, self.selKS2DD ])

        elif "KSKSLz" in name:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2KSKSLzDDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSKSDD, self.selLambda2DD ])

        elif "KSLzLz" in name:
            _B.DecayDescriptors = [ "B0 -> phi(1020) Lambda0" ]
            
            self.selH2KSLzLzDDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selKSLzDD, self.selLambda2DD ])

        else:
            _B.DecayDescriptors = [ "Lambda_b0 -> phi(1020) Lambda0" ]
            
            self.selH2LzLzLzDDD = Selection (name, Algorithm = _B, RequiredSelections = [ self.selLzLzDD, self.selLambda2DD ])


