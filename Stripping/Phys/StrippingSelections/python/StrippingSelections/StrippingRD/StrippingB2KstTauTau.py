__author__ = [' G. Mancinelli, J. Serrano']
__date__ = '15/08/2014'
__version__ = '$Revision: 0.1 $'
# Stripping line for B->K*tautau

"""
  B->K*TauTau, B->K*TauMu
  """
__all__ = (
  'B2KstTauXConf'
  ,  'default_config'
  )

default_config =  {
  'NAME'              : 'B2KstTauTau',
  'BUILDERTYPE'       : 'B2KstTauXConf',
  'WGs'    : [ 'RD' ],
  'CONFIG'    : {
  
  'PT_MU'                         : '1000', # MeV
  'TRACKCHI2_MU'                  : '3',    # dimensionless
  #
  'VCHI2_B'                       : '100', # dimensionless
  'VCHI2_B_Mu'                    : '150', # dimensionless
  'FDCHI2_B'                      : '80',
  'FD_B_Mu'                       : '3',
  'MASS_LOW_B'                    : '2000', # MeV  
  'MASS_HIGH_B'                   : '10000', # MeV  
  #
  'MASS_LOW_Kst'                  : '700', # MeV
  'MASS_HIGH_Kst'                 : '1100', # MeV
  'VCHI2_Kst'                     : '15',
  'PT_Kst'                        : '1000',
  #
  'MASS_LOW_D'                    : '1840', # MeV
  'MASS_HIGH_D'                   : '1900', # MeV
  'MASS_LOW_Ds'                   : '1938', # MeV
  'MASS_HIGH_Ds'                  : '1998', # MeV
  'APT_D'                         : '800',  #MeV
  'AMAXDOCA_D'                    : '0.2',  # mm
  'MaxPT_D'                       : '800',  #MeV
  'PT_D'                          : '1000',  #MeV
  'DIRA_D'                        : '0.99',
  'VCHI2_D'                       : '16',
  'FDCHI2_D'                      : '16',
  'VDRHOmin_D'                    : '0.1',  #mm
  'VDRHOmax_D'                    : '7.0',  #mm
  'VDZ_D'                         : '5.0',  #mm
  #


  'MINIPCHI2_K_KMM'	: '4',#'16'	,
  'MINIPCHI2_PI_KMM'	: '4',#'16'	,
  'DOCAMAX_KST_KMM'	: '0.15',#'0.15',	
  'MINIPCHI2_KST_KMM'	: '3',#'9'	,
  'FDCHI2OWNPV_KST_KMM'	: '120'	,
  'MINIPCHI2_MU_KMM'	: '4',#'16'	,
  'B_COSDIRA_KMM'  	: '0.999',
  'VCHI2_B_Mu_KMM'                    : '100', # dimensionless
  'FD_B_Mu_KMM'                       : '3',
  'FD_Kst_Mu_KMM'                       : '3',
  'MASS_LOW_B_KMM'                    : '1500', # MeV  
  'MASS_HIGH_B_KMM'                   : '10000', # MeV  
  'PT_B_KMM'                        : '2000',
  'PT_MU_KMM'                         : '800', # MeV



  
  'PT_HAD_ALL_FINAL_STATE'        : '250',  # MeV
  'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
  'IPCHI2_HAD_ALL_FINAL_STATE'    : '16',    # dimensionless
  'TRACKCHI2_HAD_ALL_FINAL_STATE' : '3',    # dimensionless
  'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.3',    # dimensionless
  #
  'B2KstTauTau_LinePrescale'               : 1,
  'B2KstTauTau_LinePostscale'              : 1,
  'B2KstTauMu_LinePrescale'                : 1,
  'B2KstTauMu_LinePostscale'               : 1,
  'B2KstTauTau_SameSign_LinePrescale'      : 1,
  'B2KstTauTau_SameSign_LinePostscale'     : 1,
  'B2KstTauMu_SameSign_LinePrescale'       : 0.5,
  'B2KstTauMu_SameSign_LinePostscale'      : 1,
  'B2KstMuMu_LinePrescale'                : 1,
  'B2KstMuMu_LinePostscale'               : 1,
  'B2KstMuMu_SameSign_LinePrescale'         : 1,
  'B2KstMuMu_SameSign_LinePostscale'        : 1,
  'B2DDSL_LinePrescale'                    : 1,
  'B2DDSL_LinePostscale'                   : 1,
  'RelatedInfoTools'      : [
  #1
  { "Type" : "RelInfoBKsttautauMuonIsolationBDT"
    ,"RecursionLevel" : 0
    , "Variables" : [
       'BKSTTAUTAUMUONISOBDTFIRSTVALUETAUP', 'BKSTTAUTAUMUONISOBDTSECONDVALUETAUP','BKSTTAUTAUMUONISOBDTTHIRDVALUETAUP',
       'BKSTTAUTAUMUONISOBDTFIRSTVALUETAUM', 'BKSTTAUTAUMUONISOBDTSECONDVALUETAUM','BKSTTAUTAUMUONISOBDTTHIRDVALUETAUM']
    , "Location"  : "B2KstTauTau_MuonIsolationBDT"  
    },
  { "Type" : "RelInfoBKsttautauTauIsolationBDT"
    ,"RecursionLevel" : 0
    , "Variables" : [
     'BKSTTAUTAUTAUISOBDTFIRSTVALUETAUP', 'BKSTTAUTAUTAUISOBDTSECONDVALUETAUP','BKSTTAUTAUTAUISOBDTTHIRDVALUETAUP',
     'BKSTTAUTAUTAUISOBDTFIRSTVALUETAUM', 'BKSTTAUTAUTAUISOBDTSECONDVALUETAUM','BKSTTAUTAUTAUISOBDTTHIRDVALUETAUM',
     'BKSTTAUTAUTAUISOBDTFIRSTVALUEKST', 'BKSTTAUTAUTAUISOBDTSECONDVALUEKST','BKSTTAUTAUTAUISOBDTTHIRDVALUEKST']
    , "Location"  : "B2KstTauTau_TauIsolationBDT"  
    },
    
  { "Type" : "RelInfoBKsttautauTrackIsolationBDT" 
    ,"RecursionLevel" : 0
    , "Variables" : [
      'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIM','BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIM','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIM',
      'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP1','BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP1','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP1',
      'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP2','BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP2','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP2',
      'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIP','BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIP','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIP',
      'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM1','BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM1','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM1',
      'BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM2','BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM2','BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM2',
      'BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTK','BKSTTAUTAUTRKISOBDTSECONDVALUEKSTK','BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTK',
      'BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTPI','BKSTTAUTAUTRKISOBDTSECONDVALUEKSTPI','BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTPI']
    , "Location"  : "B2KstTauTau_TrackIsolationBDT"  
    },  
  { "Type" : "RelInfoBstautauCDFIso" 
    ,"RecursionLevel" : 0
    #    , "Variables" : ['BKSTTAUTAUCDFISO']
    , "Location"  : "B2KstTauTau_CDFIso"  
    }
  
  ]
  },
  'STREAMS'     : ['Bhadron']
  }
  
  

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdTightMuons
from StandardParticles import StdLoosePions
from StandardParticles import StdLooseKaons
from StandardParticles import StdNoPIDsKaons


class B2KstTauXConf(LineBuilder) :
  
  """
    Builder for B->K*TauTau, B->K*TauMu
    """

  TauTau_Line    = None
  TauTau_SS_Line = None
  TauMu_Line     = None
  TauMu_SS_Line  = None
  DDSL_Line    = None
  MuMu_Line     = None
  MuMu_SS_Line  = None

  
  __configuration_keys__ = ( 
                                'PT_MU'                ,
                                'TRACKCHI2_MU'          ,
                                #
                                'VCHI2_B'                ,
                                'VCHI2_B_Mu'      ,
                                'FDCHI2_B'               ,
                                'FD_B_Mu'    ,
                                'MASS_LOW_B'             , 
                                'MASS_HIGH_B'            , 
                                #
                                'MASS_LOW_Kst'          ,  
                                'MASS_HIGH_Kst'         ,  
                                'VCHI2_Kst'             ,  
                                'PT_Kst'                ,  
                           # 
                                'MASS_LOW_D',
                                'MASS_HIGH_D',                                
                                'MASS_LOW_Ds',
                                'MASS_HIGH_Ds',   
                                'APT_D',
                                'AMAXDOCA_D',
                                'MaxPT_D',
                                'PT_D' , 
                                'DIRA_D',
                                'VCHI2_D',
                                'FDCHI2_D',
                                'VDRHOmin_D',
                                'VDRHOmax_D',
                                'VDZ_D', 
                                'PT_HAD_ALL_FINAL_STATE',
                                'P_HAD_ALL_FINAL_STATE',
                                'IPCHI2_HAD_ALL_FINAL_STATE',
                                'TRACKCHI2_HAD_ALL_FINAL_STATE',
                                'TRGHOPROB_HAD_ALL_FINAL_STATE',
                                'MINIPCHI2_K_KMM'	,
                                'MINIPCHI2_PI_KMM'	,
                                'DOCAMAX_KST_KMM'       ,	
                                'MINIPCHI2_KST_KMM'	,
                                'FDCHI2OWNPV_KST_KMM'	,
                                'MINIPCHI2_MU_KMM'	,
                                'B_COSDIRA_KMM'         ,
                                'VCHI2_B_Mu_KMM'                  ,
                                'FD_B_Mu_KMM'                     ,
                                'FD_Kst_Mu_KMM'                     ,
                                'MASS_LOW_B_KMM'                  ,  
                                'MASS_HIGH_B_KMM'                 ,   
                                'PT_B_KMM'                        ,
                                'PT_MU_KMM'                       ,                                 
                                'B2KstTauTau_LinePrescale'            ,
                                'B2KstTauTau_LinePostscale'            ,
                                'B2KstTauMu_LinePrescale'              ,
                                'B2KstTauMu_LinePostscale'             ,
                                'B2KstMuMu_LinePrescale'              ,
                                'B2KstMuMu_LinePostscale'             ,
                                'B2KstTauTau_SameSign_LinePrescale'    ,
                                'B2KstTauTau_SameSign_LinePostscale'   ,
                                'B2KstTauMu_SameSign_LinePrescale'     ,
                                'B2KstTauMu_SameSign_LinePostscale'    ,
                                'B2KstMuMu_SameSign_LinePrescale'     ,
                                'B2KstMuMu_SameSign_LinePostscale'    ,
                                'B2DDSL_LinePrescale'              ,
                                'B2DDSL_LinePostscale'             ,
                                'RelatedInfoTools'
                            )
  
  def __init__(self, name, config):

    LineBuilder.__init__(self, name, config)
    #
    selKstar        = self._makeKstar(      name    = "Kstar"+name,
                                      config  = config)
    selKstar_KMM    = self._makeKstar_KMM(      name    = "Kstar_KMM"+name,
                                      config  = config)


    
    rawTau          = DataOnDemand("Phys/StdTightDetachedTau3pi/Particles")

    selPionsForD        = self._makePionsForD(      name    = "PionsDFor"+name, 
                                      config  = config)
    selKaonsForD        = self._makeKaonsForD(      name    = "KaonsDFor"+name, 
                                      config  = config)

    selD            = self._makeD(name       = "DFor"+name,
                                  pionSel = selPionsForD,
                                  kaonSel = selKaonsForD,
                                  config     = config)

    selMuons        = self._makeMuons(      name    = "MuonsFor"+name,
                                            config  = config)
    selMuons_KMM    = self._makeMuons_KMM(      name    = "MuonsKMMFor"+name,
                                            config  = config)
    
    selB2KstTauTau     = self._makeB2XX(   name    = name,
                                           tauSel  = rawTau,
                                           KstarSel  = selKstar, 
                                           config  = config)
    selB2KstTauTauSS = self._makeB2XXSS( name    = name,
                                         tauSel  = rawTau,
                                         KstarSel  = selKstar, 
                                         config  = config)

    selB2KstTauMu    = self._makeB2XMu(  name    = name,
                                         tauSel  = rawTau,
                                         muonSel = selMuons,
                                         KstarSel  = selKstar, 
                                         config  = config)
    
    selB2KstTauMuSS = self._makeB2XMuSS(name    = name,
                                        tauSel  = rawTau,
                                        muonSel = selMuons,
                                        KstarSel  = selKstar, 
                                        config  = config)

    selB2KstMuMu    = self._makeB2XMuMu(  name    = name+"twoMuons",
                                         muonSel_KMM = selMuons_KMM,
                                         KstarSel_KMM  = selKstar_KMM, 
                                         config  = config)
    
    selB2KstMuMuSS = self._makeB2XMuMuSS(name    = name+"twoMuons_SS",
                                        muonSel_KMM = selMuons_KMM,
                                        KstarSel_KMM  = selKstar_KMM, 
                                        config  = config)

    selB2DDSL    = self._makeB2DXMu(  name    = name,
                                         DSel  = selD,
                                         muonSel = selMuons,
                                         KstarSel  = selKstar, 
                                         config  = config)
    
    self.TauTau_Line    = StrippingLine(name+"_TauTau_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauTau_LinePrescale'],
                                        postscale   = config['B2KstTauTau_LinePostscale'],
                                        MDSTFlag = True,
                                        #RelatedInfoTools = config['RelatedInfoTools'],
                                        RelatedInfoTools      = [
                                                  { "Type" : "RelInfoBKsttautauTauIsolationBDT" ,"RecursionLevel" : 0 , "Location"  : "B2KstTauTau_TauIsolationBDT" },
                                                  { "Type" : "RelInfoBKsttautauTrackIsolationBDT","RecursionLevel" : 0, "Location"  : "B2KstTauTau_TrackIsolationBDT"},  
                                                  { "Type" : "RelInfoBstautauCDFIso" ,"RecursionLevel" : 0, "Location"  : "B2KstTauTau_CDFIso"},
                                                  { "Type" : "RelInfoVertexIsolation","RecursionLevel" : 1,
                                                    "Locations" : {"Phys/B2KstTauTau_TauTau_Line" : "BVars_VertexIsoInfo",
                                                                   "Phys/StdTightDetachedTau3pi" : ["TauVars_VertexIsoInfo_0","TauVars_VertexIsoInfo_1"],
                                                                   "Phys/KstarB2KstTauTau":"KstarVars_VtxIsoInfo"
                                                                   }
                                                  }
                                                  ],
                                        selection = selB2KstTauTau
                                        )

    self.TauTau_SS_Line = StrippingLine(name+"_TauTau_SameSign_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauTau_SameSign_LinePrescale'],
                                        postscale   = config['B2KstTauTau_SameSign_LinePostscale'],
                                        MDSTFlag = True,
                                        #RelatedInfoTools = config['RelatedInfoTools'],
                                        RelatedInfoTools      = [
                                                  { "Type" : "RelInfoBKsttautauTauIsolationBDT" ,"RecursionLevel" : 0 , "Location"  : "B2KstTauTau_TauIsolationBDT" },
                                                  { "Type" : "RelInfoBKsttautauTrackIsolationBDT","RecursionLevel" : 0, "Location"  : "B2KstTauTau_TrackIsolationBDT"},  
                                                  { "Type" : "RelInfoBstautauCDFIso" ,"RecursionLevel" : 0, "Location"  : "B2KstTauTau_CDFIso"},
                                                  { "Type" : "RelInfoVertexIsolation","RecursionLevel" : 1,
                                                    "Locations" : {"Phys/B2KstTauTau_TauTau_SS_Line" : "BVars_VertexIsoInfo",
                                                                   "Phys/StdTightDetachedTau3pi" : ["TauVars_VertexIsoInfo_0","TauVars_VertexIsoInfo_1"],
                                                                   "Phys/KstarB2KstTauTau":"KstarVars_VtxIsoInfo"
                                                                   }
                                                  }
                                                  ],
                                        selection   = selB2KstTauTauSS
                                        )

    self.TauMu_Line     = StrippingLine(name+"_TauMu_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauMu_LinePrescale'],
                                        postscale   = config['B2KstTauMu_LinePostscale'],
                                        MDSTFlag = True,
                                        #RelatedInfoTools = config['RelatedInfoTools'],
                                        RelatedInfoTools      = [
                                                  { "Type" : "RelInfoBKsttautauMuonIsolationBDT" , "RecursionLevel" : 0, "Location"  : "B2KstTauTau_MuonIsolationBDT"},
                                                  { "Type" : "RelInfoBKsttautauTauIsolationBDT" ,"RecursionLevel" : 0 , "Location"  : "B2KstTauTau_TauIsolationBDT" },
                                                  { "Type" : "RelInfoBKsttautauTrackIsolationBDT","RecursionLevel" : 0, "Location"  : "B2KstTauTau_TrackIsolationBDT"},  
                                                  { "Type" : "RelInfoBstautauCDFIso" ,"RecursionLevel" : 0, "Location"  : "B2KstTauTau_CDFIso"},
                                                  { "Type" : "RelInfoVertexIsolation","RecursionLevel" : 1,
                                                    "Locations" : {"Phys/B2KstTauTau_TauMu_Line" : "BVars_VertexIsoInfo",
                                                                   "Phys/StdTightDetachedTau3pi" : "TauVars_VertexIsoInfo_0",
                                                                   "Phys/KstarB2KstTauTau":"KstarVars_VtxIsoInfo"
                                                                   }
                                                  }
                                                  ],
                                        selection   = selB2KstTauMu                                      
                                        )
   
    self.TauMu_SS_Line  = StrippingLine(name+"_TauMu_SameSign_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauMu_SameSign_LinePrescale'],
                                        #postscale   = config['B2KstTauMu_SameSign_LinePostscale'],
                                        #RelatedInfoTools = config['RelatedInfoTools'],
                                        RelatedInfoTools      = [
                                                  { "Type" : "RelInfoBKsttautauMuonIsolationBDT" , "RecursionLevel" : 0, "Location"  : "B2KstTauTau_MuonIsolationBDT"},
                                                  { "Type" : "RelInfoBKsttautauTauIsolationBDT" ,"RecursionLevel" : 0 , "Location"  : "B2KstTauTau_TauIsolationBDT" },
                                                  { "Type" : "RelInfoBKsttautauTrackIsolationBDT","RecursionLevel" : 0, "Location"  : "B2KstTauTau_TrackIsolationBDT"},  
                                                  { "Type" : "RelInfoBstautauCDFIso" ,"RecursionLevel" : 0, "Location"  : "B2KstTauTau_CDFIso"},
                                                  { "Type" : "RelInfoVertexIsolation","RecursionLevel" : 1,
                                                    "Locations" : {"Phys/B2KstTauTau_TauMu_SS_Line" : "BVars_VertexIsoInfo",
                                                                   "Phys/StdTightDetachedTau3pi" : "TauVars_VertexIsoInfo_0",
                                                                   "Phys/KstarB2KstTauTau":"KstarVars_VtxIsoInfo"
                                                                   }
                                                  }
                                                  ],
                                        MDSTFlag = True,
                                        selection   = selB2KstTauMuSS
                                        )


    self.MuMu_Line     = StrippingLine(name+"_MuMu_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstMuMu_LinePrescale'],
                                        postscale   = config['B2KstMuMu_LinePostscale'],
                                        MDSTFlag = True,
                                        #RelatedInfoTools = config['RelatedInfoTools'],
                                        RelatedInfoTools      = [
                                                  { "Type" : "RelInfoBKsttautauMuonIsolationBDT" , "RecursionLevel" : 0, "Location"  : "B2KstTauTau_MuonIsolationBDT"},
                                                  { "Type" : "RelInfoBKsttautauTauIsolationBDT" ,"RecursionLevel" : 0 , "Location"  : "B2KstTauTau_TauIsolationBDT" },
                                                  { "Type" : "RelInfoBKsttautauTrackIsolationBDT","RecursionLevel" : 0, "Location"  : "B2KstTauTau_TrackIsolationBDT"},  
                                                  { "Type" : "RelInfoBstautauCDFIso" ,"RecursionLevel" : 0, "Location"  : "B2KstTauTau_CDFIso"},
                                                  { "Type" : "RelInfoVertexIsolation","RecursionLevel" : 1,
                                                    "Locations" : {"Phys/B2KstTauTau_MuMu_Line" : "BVars_VertexIsoInfo",
                                                                   "Phys/Kstar_KMMB2KstTauTau":"KstarVars_VtxIsoInfo"
                                                                   }
                                                    }
                                                  ],
                                        selection   = selB2KstMuMu                                      
                                        )
   
    self.MuMu_SS_Line  = StrippingLine(name+"_MuMu_SameSign_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstMuMu_SameSign_LinePrescale'],
                                        postscale   = config['B2KstMuMu_SameSign_LinePostscale'],
                                        MDSTFlag = True,
                                        #RelatedInfoTools = config['RelatedInfoTools'],
                                        RelatedInfoTools      = [
                                                  { "Type" : "RelInfoBKsttautauMuonIsolationBDT" , "RecursionLevel" : 0, "Location"  : "B2KstTauTau_MuonIsolationBDT"},
                                                  { "Type" : "RelInfoBKsttautauTauIsolationBDT" ,"RecursionLevel" : 0 , "Location"  : "B2KstTauTau_TauIsolationBDT" },
                                                  { "Type" : "RelInfoBKsttautauTrackIsolationBDT","RecursionLevel" : 0, "Location"  : "B2KstTauTau_TrackIsolationBDT"},  
                                                  { "Type" : "RelInfoBstautauCDFIso" ,"RecursionLevel" : 0, "Location"  : "B2KstTauTau_CDFIso"},
                                                  { "Type" : "RelInfoVertexIsolation","RecursionLevel" : 1,
                                                    "Locations" : {"Phys/B2KstTauTau_MuMu_SS_Line" : "BVars_VertexIsoInfo",
                                                                   "Phys/Kstar_KMMB2KstTauTau":"KstarVars_VtxIsoInfo"
                                                                   }
                                                    }
                                                  ],
                                        selection   = selB2KstMuMuSS
                                        )
    
    self.DDSL_Line    = StrippingLine(name+"_DDSL_Line",
                                      #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                      prescale    = config['B2DDSL_LinePrescale'],
                                      postscale   = config['B2DDSL_LinePostscale'],
                                      MDSTFlag = True,
                                      #RelatedInfoTools = config['RelatedInfoTools'],
                                      RelatedInfoTools      = [
                                                  { "Type" : "RelInfoBKsttautauMuonIsolationBDT" , "RecursionLevel" : 0, "Location"  : "B2KstTauTau_MuonIsolationBDT"},
                                                  { "Type" : "RelInfoBKsttautauTauIsolationBDT" ,"RecursionLevel" : 0 , "Location"  : "B2KstTauTau_TauIsolationBDT" },
                                                  { "Type" : "RelInfoBKsttautauTrackIsolationBDT","RecursionLevel" : 0, "Location"  : "B2KstTauTau_TrackIsolationBDT"},  
                                                  { "Type" : "RelInfoBstautauCDFIso" ,"RecursionLevel" : 0, "Location"  : "B2KstTauTau_CDFIso"},
                                                  { "Type" : "RelInfoVertexIsolation","RecursionLevel" : 1,
                                                    "Locations" : {"Phys/B2KstTauTau_DDSL_Line" : "BVars_VertexIsoInfo",
                                                                   "Phys/KstarB2KstTauTau":"KstaVars_VtxIsoInfo",
                                                                   "Phys/DForB2KstTauTau" : "DDSLvars_VtxIsoInfo"
                                                                   }
                                                    }
                                                  ],
                                      selection = selB2DDSL
                                      )

     
    #

    self.registerLine( self.TauTau_Line )
    self.registerLine( self.TauMu_Line )
    self.registerLine( self.MuMu_Line )
    self.registerLine( self.TauTau_SS_Line )
    self.registerLine( self.TauMu_SS_Line )
    self.registerLine( self.MuMu_SS_Line )
    self.registerLine( self.DDSL_Line )
  
  
  #####################################################
  def _makeB2XX(self, name, tauSel, KstarSel, config):
    
    
    _combcut = "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2)  <   "   + config['VCHI2_B']       + ")  &"\
               "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ")  "
   
    
    _CombineTau = CombineParticles( DecayDescriptors = ["B0 -> K*(892)0 tau+ tau-"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut)
    
    return (Selection(name+"_TauTau",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel , KstarSel ] ))


  #####################################################
  def _makeB2XXSS(self, name, tauSel, KstarSel, config):
    
    
    _combcut = "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2)  <   "   + config['VCHI2_B']       + ") & "\
               "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ")  "

    
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 ->  K*(892)0 tau+ tau+]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut)
                                      
    
    return (Selection(name+"_TauTauSS",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel ,KstarSel] ))
  
  #####################################################
  def _makeB2XMu(self, name, tauSel,  muonSel, KstarSel, config):
    
    _combcut = "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"

    _bcut    ="(VFASPF(VCHI2)  <   "   + config['VCHI2_B_Mu']       + ") & "\
               "(BPVVD          >   "   + config['FD_B_Mu']      + ")  "
         
    
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 ->  K*(892)0 tau+ mu-]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut)
    
    return (Selection(name+"_TauMu",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel, muonSel, KstarSel ] ))
  #####################################################
  def _makeB2DXMu(self, name, DSel,  muonSel, KstarSel, config):
    
    _combcut = "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"

    _bcut    ="(VFASPF(VCHI2)  <   "   + config['VCHI2_B_Mu']       + ") & "\
               "(BPVVD          >   "   + config['FD_B_Mu']      + ")  "
         
    
    _CombineD = CombineParticles( DecayDescriptors = ["[B0 ->  K*(892)0 D+ mu-]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut)
    
    return (Selection(name+"_DDSL",
                      Algorithm          = _CombineD,
                      RequiredSelections = [ DSel, muonSel, KstarSel ] ))
  
    #####################################################
  def _makeB2XMuSS(self, name, tauSel, muonSel, KstarSel,config):
    
    
    _combcut = "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    ="(VFASPF(VCHI2)  <   "   + config['VCHI2_B_Mu']       + ") & "\
               "(BPVVD          >   "   + config['FD_B_Mu']      + ")  "
    
 
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 ->  K*(892)0 tau+ mu+]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut)
    
    return (Selection(name+"_TauMuSS",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel, muonSel, KstarSel ] ))


#####################################################
  def _makeB2XMuMu(self, name, muonSel_KMM, KstarSel_KMM, config):
    
    _combcut = "(AM  > " + config['MASS_LOW_B_KMM']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B_KMM']   + "*MeV)"

    _bcut    = " (PT > "             + config['PT_B_KMM']   + "*MeV) &"\
               "(VFASPF(VCHI2)  <   "   + config['VCHI2_B_Mu_KMM']       + ") & "\
               "(BPVVD          >   "   + config['FD_B_Mu_KMM']      + ") & "\
               " (BPVDIRA >"         + config['B_COSDIRA_KMM']   + ") "
      
    
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 ->  K*(892)0 mu+ mu-]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut)
    
    return (Selection(name+"_MuMu",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ muonSel_KMM, KstarSel_KMM ] ))
  
#####################################################
  def _makeB2XMuMuSS(self, name, muonSel_KMM, KstarSel_KMM, config):
    
    _combcut = "(AM  > " + config['MASS_LOW_B_KMM']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B_KMM']   + "*MeV)"

    _bcut    = " (PT > "             + config['PT_B_KMM']   + "*MeV) &"\
               "(VFASPF(VCHI2)  <   "   + config['VCHI2_B_Mu_KMM']       + ") & "\
               "(BPVVD          >   "   + config['FD_B_Mu_KMM']      + ") & "\
               " (BPVDIRA >"         + config['B_COSDIRA_KMM']   + ") "
      
    
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 ->  K*(892)0 mu+ mu+]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut)
    
    return (Selection(name+"_MuMu",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ muonSel_KMM, KstarSel_KMM ] ))
  
    #####################################################

  def _makeKstar(self, name, config) :
             
     _combcut = " (AM  > "           + config['MASS_LOW_Kst']    + "*MeV) & "\
                " (AM  < "           + config['MASS_HIGH_Kst']   + "*MeV)   "

     
     _bcut = " (PT > "             + config['PT_Kst']   + "*MeV) &"\
               "(BPVVD          >   "   + config['FD_Kst_Mu_KMM']      + ") & "\
             " (VFASPF(VCHI2)  <   "   + config['VCHI2_Kst']       + ")  "



     _daucut = { 'pi+' : '( (TRCHI2DOF < 4) & (PROBNNpi>0.5))',
                 'K-' : '( (TRCHI2DOF < 4) & (PROBNNk>0.2))' }

     
     _CombineKst   = CombineParticles( DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc"],
                                       CombinationCut   = _combcut, 
                                       MotherCut        = _bcut ,
                                       DaughtersCuts    = _daucut
                                       )    

     return Selection(name,
                      Algorithm          = _CombineKst,
                      RequiredSelections = [StdLoosePions,StdNoPIDsKaons]
                      )

  
  #####################################################

  def _makeKstar_KMM(self, name, config) :
             
     _combcut = " (AM  > "           + config['MASS_LOW_Kst']    + "*MeV) & "\
                " (AM  < "           + config['MASS_HIGH_Kst']   + "*MeV) &  "\
		" (AMAXDOCA('')  <"  + config['DOCAMAX_KST_KMM']   + "*mm)"

     
     _bcut = " (PT > "             + config['PT_Kst']   + "*MeV) &"\
             " (VFASPF(VCHI2)  <   "   + config['VCHI2_Kst']       + ") &  "\
	     " (MIPCHI2DV(PRIMARY) > "+config['MINIPCHI2_KST_KMM'] +") &"\
 	     " (BPVVDCHI2 > "      + config['FDCHI2OWNPV_KST_KMM']   + ") "


     _daucut = { 'pi+' : '( (TRCHI2DOF < 4) & (PROBNNpi>0.5) & (MIPCHI2DV(PRIMARY) > '+config['MINIPCHI2_PI_KMM']  +'))',
                 'K-' : '( (TRCHI2DOF < 4) & (PROBNNk>0.2)  & (MIPCHI2DV(PRIMARY) > '+config['MINIPCHI2_K_KMM']  +'))' }

     
     _CombineKst_KMM   = CombineParticles( DecayDescriptors = ["[K*(892)0 -> K+ pi-]cc"],
                                       CombinationCut   = _combcut, 
                                       MotherCut        = _bcut ,
                                       DaughtersCuts    = _daucut
                                       )    

     return Selection(name,
                      Algorithm          = _CombineKst_KMM,
                      RequiredSelections = [StdLoosePions,StdNoPIDsKaons]
                      )

  #####################################################


  def _makeMuons(self, name, config) :
    """                     
      Muon selection          
      """                     
    _code =     "(PT > "                    + config['PT_MU']          + "*MeV) & "\
            "(TRCHI2DOF < "             + config['TRACKCHI2_MU']   + ")      "
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,  
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdTightMuons ] )    

  #####################################################



   
  def _makeMuons_KMM(self, name, config) :
    """                     
      Muon selection          
      """                     
    _code =     "(PT > "                    + config['PT_MU_KMM']          + "*MeV) & "\
            "(TRCHI2DOF < "             + config['TRACKCHI2_MU']   + ")      &" \
	    "(	MIPCHI2DV(PRIMARY) > "+config['MINIPCHI2_MU_KMM']  +")  "
    
    _Filter_KMM = FilterDesktop(Code = _code)
    
    return Selection(name,  
                     Algorithm          = _Filter_KMM,
                     RequiredSelections = [ StdTightMuons ] )    

  #####################################################
 
  def _makePionsForD(self, name, config) :
    """
      Pion selection for B -> DD
      """
    _code = self._hadFinalStateKinematicCuts(config) +" & (PROBNNpi > 0.55)"
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
  #####################################################
  def _makeKaonsForD(self, name, config) :
    """
      Kaon selection for B -> DD
      """
    _code = self._hadFinalStateKinematicCuts(config)  
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdLooseKaons ] )
  
   #####################################################
  def _makeD(self, name, pionSel, kaonSel, config) :

     _preambulo = [ "c1c2c3 = ((('pi+') == ABSID) | (('K+') == ABSID))" ,
                    "ipsm    = MINTREE( c1c2c3 , MIPCHI2DV(PRIMARY) )"]
            
     _combcut = " (((AM  > "           + config['MASS_LOW_D']    + "*MeV) & "\
                " (AM  < "           + config['MASS_HIGH_D']   + "*MeV))  | "\
                " ((AM  > "           + config['MASS_LOW_Ds']    + "*MeV) & "\
                " (AM  < "           + config['MASS_HIGH_Ds']   + "*MeV)))  & "\
                " (APT  > "          + config['APT_D']   + "*MeV)  & "\
                " (AMAXDOCA('')  <"  + config['AMAXDOCA_D']   + "*mm)  & "\
                " (ANUM(PT > "       + config['MaxPT_D']   + "*MeV) >= 1) "
     
     _bcut = " (PT > "             + config['PT_D']   + "*MeV) &"\
             " (BPVDIRA >"         + config['DIRA_D']   + ") &"\
             " (VFASPF(VCHI2) < "  + config['VCHI2_D']   + ")&"\
             " (BPVVDCHI2 > "      + config['FDCHI2_D']   + ") &"\
             " (BPVVDRHO >  "      + config['VDRHOmin_D']   + "*mm) &"\
             " (BPVVDRHO <  "      + config['VDRHOmax_D']   + "*mm) &"\
             " (BPVVDZ >  "        + config['VDZ_D']   + "*mm) "
  
     
     _CombineD   = CombineParticles( DecayDescriptors = ["[D+ -> pi+ K- pi+]cc","[D+ -> K+ K- pi+]cc"],
                                     CombinationCut   = _combcut, 
                                     MotherCut        = _bcut+"& (M>1800.*MeV) & (M<2030.*MeV)" ,
                                     Preambulo        = _preambulo)
    

     return Selection(name,
                      Algorithm          = _CombineD,
                      RequiredSelections = [pionSel, kaonSel]
                      )

  #####################################################   
  def _hadFinalStateKinematicCuts(self, config) :
    _code = "(P  > "                    + config['P_HAD_ALL_FINAL_STATE']     + "*MeV) & "\
      "(PT > "                    + config['PT_HAD_ALL_FINAL_STATE']          + "*MeV) & "\
      "(MIPCHI2DV(PRIMARY) > "    + config['IPCHI2_HAD_ALL_FINAL_STATE']      + ")     & "\
      "(TRCHI2DOF < "             + config['TRACKCHI2_HAD_ALL_FINAL_STATE']   + ")     & "\
      "(TRGHOSTPROB < "           + config['TRGHOPROB_HAD_ALL_FINAL_STATE']   + ")     "

    return _code
  
