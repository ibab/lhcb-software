__author__ = ['V. Gligorov, Ch. Elsasser, G. Mancinelli, J. Serrano']
__date__ = '24/05/2010,15/01/2014'
__version__ = '$Revision: 1.2 $'
# Stripping line by V. Gligorov for B->mu tau, B->tau tau, B->D mu and B->D D
# Strongly modified for Stripping20 by Ch. Elsasser
# added Dpi mode by G. Mancinelli
# Strongly modified for S20r2 by G. Mancinelli and J. Serrano

"""
  B->TauTau, B->TauMu
  """

config_params =  {
  'PT_HAD_ALL_FINAL_STATE'        : '250',  # MeV
  'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
  'IPCHI2_HAD_ALL_FINAL_STATE'    : '16',    # dimensionless
  'TRACKCHI2_HAD_ALL_FINAL_STATE' : '3',    # dimensionless
  'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.3',    # dimensionless
  #
  'PT_MU'                         : '1000', # MeV
  'P_MU'                          : '6000', # MeV 
  'IPCHI2_MU'                     : '16',   # MeV
  'TRACKCHI2_MU'                  : '3',    # dimensionless
  'TRGHOPROB_MU'                  : '0.3', # dimensionless

  #
  'PT_B_TT'                       : '1900', # MeV
  'PT_B_TT_HIGH'                  : '2000',# MeV 
  'PT_B_TM'                       : '1900', # MeV
  'PT_B_TM_HIGH'                  : '5000', # MeV  
  'VCHI2_B'                       :   '90', # dimensionless
  'FDCHI2_B'                      : '225',  # dimensionless
  'FD_B'                          : '90',   # mm
  'FD_B_MU'                       : '35',   # mm
  'DIRA_B'                        : '0.99', # dimensionless
  'MASS_LOW_B'                    : '2000', # MeV  
  'MASS_HIGH_B'                   : '7000', # MeV 
  'MCOR_LOW_B'                    :    '0', # MeV
  'MCOR_HIGH_B'                   :'10000', # MeV 
 
 
  #
  'PT_B_CHILD_BEST'               : '2000', # MeV
  'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
  'PT_B_TAU_CHILD_BEST'           : '4000', # MeV
  'IPCHI2_B_TAU_CHILD_BEST'       : '150',   # dimensionless
  'IPCHI2_B_TAU_CHILD_WORSE'       : '16',   # dimensionless
  'PT_B_PIONS_TOTAL'              :  '7000',# MeV
  'PT_B_MU_PIONS_TOTAL'           :  '2500',# MeV
  'IPCHI2_B_TAU_MU'               :  '50',
  'IPCHI2_B_MU'                   :  '200',
  'B_TAUPI_2NDMINIPS'             :  '20',  # dimensionless
  'FDCHI2_TAU'                    :  '4000',
  'VCHI2_B_TAU_MU'                : '12',
  #
  'MASS_LOW_D'                    : '1750', # MeV
  'MASS_HIGH_D'                   : '2080', # MeV
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
  'MASS_LOW_TAU'                    : '400', # MeV
  'MASS_HIGH_TAU'                   : '2100', # MeV
  #
  'B2TauTau_TOSLinePrescale'      : 1,
  'B2TauTau_TOSLinePostscale'     : 1,
  'B2DD_LinePrescale'             : 1,
  'B2DD_LinePostscale'            : 1,
  'B2TauMu_TOSLinePrescale'       : 1,
  'B2TauMu_TOSLinePostscale'      : 1,
  'B2DMu_LinePrescale'            : 1.,
  'B2DMu_LinePostscale'           : 1,
  'B2DPi_LinePrescale'            : 1,
  'B2DPi_LinePostscale'           : 1,
  'B2TauTau_SameSign_TOSLinePrescale'      : 0.5,
  'B2TauTau_SameSign_TOSLinePostscale'     : 1,
  'B2DD_SameSign_LinePrescale'             : 0.5,
  'B2DD_SameSign_LinePostscale'            : 1,
  'B2TauMu_SameSign_TOSLinePrescale'       : 0.5,
  'B2TauMu_SameSign_TOSLinePostscale'      : 1,
  'B2DMu_SameSign_LinePrescale'            : 1.,
  'B2DMu_SameSign_LinePostscale'           : 1,
  'B2DPi_SameSign_LinePrescale'            : 0.5,
  'B2DPi_SameSign_LinePostscale'           : 1,
                    }

__all__ = ('B2XTauConf')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsKaons, StdNoPIDsPions, StdTightMuons
from StandardParticles import StdLoosePions, StdTightPions
from StandardParticles import StdLooseKaons 

default_name = "B2XTau"
HLT_DECISIONS_HAD   = "Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision"
HLT_DECISIONS_MUON  = "Hlt2(TopoMu|SingleMuon).*Decision"

class B2XTauConf(LineBuilder) :
  
  """
    Builder for B->TauTau, B->TauMu
    """

  TauTau_TOSLine    = None
  TauTau_SS_TOSLine = None
  TauMu_TOSLine     = None
  TauMu_SS_TOSLine  = None
  DMu_Line       = None
  DMu_SS_Line    = None
  DPi_Line       = None
  DPi_SS_Line    = None
  DD_Line        = None 
  DD_SS_Line     = None 
  TauTau_piSS_TOSLine = None
  TauMu_piSS_TOSLine = None
 
  __configuration_keys__ = (
                            'PT_HAD_ALL_FINAL_STATE',
                            'P_HAD_ALL_FINAL_STATE',
                            'IPCHI2_HAD_ALL_FINAL_STATE',
                            'TRACKCHI2_HAD_ALL_FINAL_STATE',
                            'TRGHOPROB_HAD_ALL_FINAL_STATE',
                            #
                            'PT_MU',            
                            'P_MU',
                            'IPCHI2_MU',
                            'TRACKCHI2_MU',
                            'TRGHOPROB_MU',
                            #   
                            'PT_B_TT',
                            'PT_B_TT_HIGH',
                            'PT_B_TM',
                            'PT_B_TM_HIGH',
                            'VCHI2_B' ,
                            'FDCHI2_B',
                            'FD_B',
                            'FD_B_MU',
                            'DIRA_B',
                            'MASS_LOW_B',
                            'MASS_HIGH_B',
                            'MCOR_LOW_B',
                            'MCOR_HIGH_B',

                            #
                            'PT_B_CHILD_BEST',
                            'IPCHI2_B_CHILD_BEST',   
                            'PT_B_TAU_CHILD_BEST', 
                            'IPCHI2_B_TAU_CHILD_BEST',
                            'IPCHI2_B_TAU_CHILD_WORSE',
                            'PT_B_PIONS_TOTAL',
                            'B_TAUPI_2NDMINIPS',
                            'PT_B_MU_PIONS_TOTAL' ,    
                            'IPCHI2_B_TAU_MU',
                            'FDCHI2_TAU',
                            'IPCHI2_B_MU',
                            'VCHI2_B_TAU_MU',
                            #
                            'MASS_LOW_D',
                            'MASS_HIGH_D',   
                            'APT_D',
                            'AMAXDOCA_D',
                            'MaxPT_D',
                            'PT_D', 
                            'DIRA_D',
                            'VCHI2_D',
                            'FDCHI2_D',
                            'VDRHOmin_D',
                            'VDRHOmax_D',
                            'VDZ_D',
                            #
                            'MASS_LOW_TAU',
                            'MASS_HIGH_TAU',   
                            #
                            'B2TauTau_TOSLinePrescale',
                            'B2TauTau_TOSLinePostscale',
                            'B2DD_LinePrescale',
                            'B2DD_LinePostscale',
                            'B2TauMu_TOSLinePrescale',
                            'B2TauMu_TOSLinePostscale',
                            'B2DMu_LinePrescale',
                            'B2DMu_LinePostscale',
                            'B2DPi_LinePrescale',
                            'B2DPi_LinePostscale',
                            'B2TauTau_SameSign_TOSLinePrescale',
                            'B2TauTau_SameSign_TOSLinePostscale',
                            'B2DD_SameSign_LinePrescale',
                            'B2DD_SameSign_LinePostscale',
                            'B2TauMu_SameSign_TOSLinePrescale',
                            'B2TauMu_SameSign_TOSLinePostscale',
                            'B2DMu_SameSign_LinePrescale',
                            'B2DMu_SameSign_LinePostscale',
                            'B2DPi_SameSign_LinePrescale',
                            'B2DPi_SameSign_LinePostscale'
                            )
  
  def __init__(self, name, config):
    LineBuilder.__init__(self, name, config)
    #
    selPions        = self._makePions(      name    = "PionsFor"+name, 
                                      config  = config)
    selMuons        = self._makeMuons(      name    = "MuonsFor"+name,
                                      config  = config)
    selPionsForD        = self._makePionsForD(      name    = "PionsDFor"+name, 
                                      config  = config)
    selKaonsForD        = self._makeKaonsForD(      name    = "KaonsDFor"+name, 
                                      config  = config)
    rawTau          = DataOnDemand("Phys/StdLooseDetachedTau3pi/Particles")
    
    selTau          = self._filterTau(name       = name+"_TauFilter",
                                      tauInput   = rawTau,
                                      config     = config)
    selTauSS        = self._makeTauSS(      name    = "TauSSFor"+name,
                                            pionSel = selPions,
                                            config  = config)
    
    selD            = self._makeD(name       = "DFor"+name,
                                  pionSel = selPionsForD,
                                  kaonSel = selKaonsForD,
                                  config     = config)
    #
    selB2TauTau,selB2DD , selB2TauTaupiSS    = self._makeB2XX(   name    = name,
                                                                 tauSel  = selTau,
                                                                 DSel    = selD,
                                                                 tauSelSS  = selTauSS,
                                                                 config  = config)
    selB2TauTauSS,selB2DDSS = self._makeB2XXSS( name    = name,
                                               tauSel  = selTau,
                                               DSel    = selD,    
                                               config  = config)

    selB2TauMu,selB2DMu , selB2TauMupiSS     = self._makeB2XMu(  name    = name,
                                              tauSel  = selTau,
                                              DSel    = selD,                                           
                                              muonSel = selMuons,
                                              tauSelSS  = selTauSS,
                                              config  = config)
    
    selB2DPi     = self._makeB2DPi(  name    = name,
                                     DSel    = selD,
                                     pionSel = selPions,
                                     config  = config)
    
    selB2TauMuSS,selB2DMuSS = self._makeB2XMuSS(name    = name,
                                                tauSel  = selTau,
                                                DSel    = selD,
                                                muonSel = selMuons,
                                                config  = config)
    selB2DPiSS = self._makeB2DPiSS(name    = name,
                                   DSel    = selD,
                                   pionSel = selPions,
                                   config  = config)

    self.TauTau_TOSLine    = StrippingLine(name+"_TauTau_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_TOSLinePrescale'],
                                           postscale   = config['B2TauTau_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
#                                           selection = selB2TauTau
                                           )

    self.TauTau_SS_TOSLine = StrippingLine(name+"_TauTau_SameSign_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_SameSign_TOSLinePrescale'],
                                           postscale   = config['B2TauTau_SameSign_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauTauSS",selB2TauTauSS)
#                                           selection   = selB2TauTauSS
                                           )
    self.TauTau_piSS_TOSLine    = StrippingLine(name+"_TauTau_piSS_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_SameSign_TOSLinePrescale'],
                                           postscale   = config['B2TauTau_SameSign_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauTaupiSS",selB2TauTaupiSS)
#                                           selection = selB2TauTau
                                           )

    self.DD_Line        = StrippingLine(name+"_DD_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DD_LinePrescale'],
                                           postscale   = config['B2DD_LinePostscale'],
#                                           selection   = self._makeTOS(name+"_TOSForDD",selB2DD)
                                           selection   = selB2DD
                                           )
    self.DD_SS_Line     = StrippingLine(name+"_DD_SameSign_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DD_SameSign_LinePrescale'],
                                           postscale   = config['B2DD_SameSign_LinePostscale'],
#                                           selection   = self._makeTOS(name+"_TOSForDDSS",selB2DDSS)
                                           selection   = selB2DDSS
                                           )
    self.TauMu_TOSLine     = StrippingLine(name+"_TauMu_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_TOSLinePrescale'],
                                           postscale   = config['B2TauMu_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauMu",selB2TauMu)
#                                           selection   = selB2TauMu
                                           )
    self.TauMu_piSS_TOSLine     = StrippingLine(name+"_TauMu_piSS_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_TOSLinePrescale'],
                                           postscale   = config['B2TauMu_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauMupiSS",selB2TauMupiSS)
#                                           selection   = selB2TauMu
                                           )
   
    self.TauMu_SS_TOSLine  = StrippingLine(name+"_TauMu_SameSign_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_SameSign_TOSLinePrescale'],
                                           postscale   = config['B2TauMu_SameSign_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauMuSS",selB2TauMuSS)
#                                           selection   = selB2TauMuSS
                                           )
    self.DMu_Line       = StrippingLine(name+"_DMu_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DMu_LinePrescale'],
                                           postscale   = config['B2DMu_LinePostscale'],
                                           selection   = selB2DMu
                                           )
    self.DPi_Line       = StrippingLine(name+"_DPi_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DPi_LinePrescale'],
                                           postscale   = config['B2DPi_LinePostscale'],
                                           selection   = selB2DPi
                                           )
    self.DMu_SS_Line    = StrippingLine(name+"_DMu_SameSign_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DMu_LinePrescale'],
                                           postscale   = config['B2DMu_LinePostscale'],
                                           selection   = selB2DMuSS
                                           )
    self.DPi_SS_Line    = StrippingLine(name+"_DPi_SameSign_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DPi_SameSign_LinePrescale'],
                                           postscale   = config['B2DPi_SameSign_LinePostscale'],
                                           selection   = selB2DPiSS
                                           )
    #
    #

    self.registerLine( self.TauTau_TOSLine )
    self.registerLine( self.DD_Line )
    self.registerLine( self.TauMu_TOSLine )
    self.registerLine( self.DMu_Line )    
    self.registerLine( self.DPi_Line )    
    self.registerLine( self.TauTau_SS_TOSLine )
    self.registerLine( self.DD_SS_Line )
    self.registerLine( self.TauMu_SS_TOSLine )
    self.registerLine( self.DMu_SS_Line )    
    self.registerLine( self.DPi_SS_Line )    
    self.registerLine( self.TauTau_piSS_TOSLine )
    self.registerLine( self.TauMu_piSS_TOSLine )
  
  
  #####################################################
  def _makeB2XX(self, name, tauSel, DSel, tauSelSS, config):
    
    #
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS",
                 "allpi = ((('pi+') == ABSID) | (('K+') == ABSID)) ",
                 "sumpt    = SUMTREE( allpi , PT )"]
    
    _combcut = "(APT > " + config['PT_B_TT']       + "*MeV) & "\
               "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
               "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
               "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
               "(BPVVD          <   "   + config['FD_B']      + ") & "\
               "(PT                >   "   + config['PT_B_TT_HIGH']  + "*MeV) & "\
               "(INGENERATION((PT   >   "   + config['PT_B_TAU_CHILD_BEST']+ "*MeV),1)) & "\
               "(INGENERATION((PT   >   "   + config['PT_B_CHILD_BEST']+ "*MeV),2)) & "\
               "(sumpt >" + config['PT_B_PIONS_TOTAL']+ "*MeV) & "\
               "(max(CHILD(ipsm,1),CHILD(ipsm,2)) > "   + config['B_TAUPI_2NDMINIPS']+") & "\
               "(max(CHILD(MIPCHI2DV(PRIMARY),1),CHILD(MIPCHI2DV(PRIMARY),2)) > "   + config['IPCHI2_B_TAU_CHILD_BEST']+") &  "\
               "(min(CHILD(MIPCHI2DV(PRIMARY),1),CHILD(MIPCHI2DV(PRIMARY),2)) > "   + config['IPCHI2_B_TAU_CHILD_WORSE']+") & "\
               "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    
   
    
    _CombineTau = CombineParticles( DecayDescriptors = ["B0 -> tau+ tau-"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    _CombineD   = CombineParticles( DecayDescriptors = ["B0 -> D+ D-"],
                                   CombinationCut   = _combcut + " & (AM > 5000)",
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    return (Selection(name+"_TauTau",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel ] ),
            Selection(name+"_DD",
                      Algorithm          = _CombineD,
                      RequiredSelections = [ DSel ] ),
            Selection(name+"_TauTau_piSS",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSelSS ] ),)
  #####################################################
  def _makeB2XXSS(self, name, tauSel, DSel, config):
    
    # 
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS",
                 "allpi = ((('pi+') == ABSID) | (('K+') == ABSID))",
                 "sumpt    = SUMTREE( allpi , PT )"]
    
    _combcut = "(APT > " + config['PT_B_TT']       + "*MeV) & "\
               "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
               "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
               "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
               "(BPVVD          <   "   + config['FD_B']      + ") & "\
               "(PT                >   "   + config['PT_B_TT_HIGH']  + "*MeV) & "\
               "(INGENERATION((PT   >   "   + config['PT_B_TAU_CHILD_BEST']+ "*MeV),1)) & "\
               "(INGENERATION((PT   >   "   + config['PT_B_CHILD_BEST']+ "*MeV),2)) & "\
               "(sumpt >" + config['PT_B_PIONS_TOTAL']+ "*MeV) & "\
               "(max(CHILD(ipsm,1),CHILD(ipsm,2)) > "   + config['B_TAUPI_2NDMINIPS']+") & "\
               "(max(CHILD(MIPCHI2DV(PRIMARY),1),CHILD(MIPCHI2DV(PRIMARY),2)) > "   + config['IPCHI2_B_TAU_CHILD_BEST']+") &  "\
               "(min(CHILD(MIPCHI2DV(PRIMARY),1),CHILD(MIPCHI2DV(PRIMARY),2)) > "   + config['IPCHI2_B_TAU_CHILD_WORSE']+") & "\
               "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    
   
    
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 -> tau+ tau+]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ D+]cc"],
                                   CombinationCut   = _combcut + " & (AM > 5000)",
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    return (Selection(name+"_TauTauSS",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel ] ),
            Selection(name+"_DDSS",
                      Algorithm          = _CombineD,
                      RequiredSelections = [ DSel ] ))
  
  #####################################################
  def _makeB2XMu(self, name, tauSel, DSel, muonSel, tauSelSS, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS",
                 "allpi = ((('pi+') == ABSID) | (('K+') == ABSID)) ",
                 "sumpt    = SUMTREE( allpi , PT )"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "((CHILD(BPVVDCHI2,1))   <   "   + config['FDCHI2_TAU']       + ") & "\
      "(MIPCHI2DV(PRIMARY)   <              "   + config['IPCHI2_B_MU']        + ") & "\
      "(BPVVD          <   "   + config['FD_B_MU']      + ") & "\
      "(PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) & "\
      "(sumpt >" + config['PT_B_MU_PIONS_TOTAL']+ "*MeV) & "\
      "((CHILD(VFASPF(VCHI2),1))   <   "   + config['VCHI2_B_TAU_MU']       + ") & "\
      "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV)) &"\
      "((CHILD(MIPCHI2DV(PRIMARY),1)) > "   + config['IPCHI2_B_TAU_MU']+")   "
     
    
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 -> tau+ mu-]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ mu-]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    return (Selection(name+"_TauMu",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel, muonSel ] ),
            Selection(name+"_DMu",
                      Algorithm          = _CombineD,
                      RequiredSelections = [ DSel, muonSel ] ),
            Selection(name+"_TauMu_piSS",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSelSS, muonSel ] ))
  
    #####################################################
  def _makeB2XMuSS(self, name, tauSel, DSel, muonSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS",
                 "allpi = ((('pi+') == ABSID) | (('K+') == ABSID)) ",
                 "sumpt    = SUMTREE( allpi , PT )"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"

    _bcut    = "((CHILD(BPVVDCHI2,1))   <   "   + config['FDCHI2_TAU']       + ") & "\
      "(MIPCHI2DV(PRIMARY)   <              "   + config['IPCHI2_B_MU']        + ") & "\
      "(BPVVD          <   "   + config['FD_B_MU']      + ") & "\
      "(PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) & "\
      "(sumpt >" + config['PT_B_MU_PIONS_TOTAL']+ "*MeV) & "\
      "((CHILD(VFASPF(VCHI2),1))   <   "   + config['VCHI2_B_TAU_MU']       + ") & "\
      "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV)) &"\
      "((CHILD(MIPCHI2DV(PRIMARY),1)) > "   + config['IPCHI2_B_TAU_MU']+")   "
 
 
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 -> tau+ mu+]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ mu+]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    return (Selection(name+"_TauMuSS",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel, muonSel ] ),
            Selection(name+"_DMuSS",
                      Algorithm          = _CombineD,
                      RequiredSelections = [ DSel, muonSel ] ))
  
  #####################################################
  def _makeB2DPi(self, name, DSel, pionSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS",
                 "allpi = ((('pi+') == ABSID) | (('K+') == ABSID))",
                 "sumpt    = SUMTREE( allpi , PT )"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"

    _bcut    = "((CHILD(BPVVDCHI2,1))   <   "   + config['FDCHI2_TAU']       + ") & "\
      "(MIPCHI2DV(PRIMARY)   <              "   + config['IPCHI2_B_MU']        + ") & "\
      "(BPVVD          <   "   + config['FD_B_MU']      + ") & "\
      "(PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) & "\
      "(sumpt >" + config['PT_B_MU_PIONS_TOTAL']+ "*MeV) & "\
      "((CHILD(VFASPF(VCHI2),1))   <   "   + config['VCHI2_B_TAU_MU']       + ") & "\
      "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV)) &"\
      "((CHILD(MIPCHI2DV(PRIMARY),1)) > "   + config['IPCHI2_B_TAU_MU']+")   "
 
    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ pi-]cc"],
                                    CombinationCut   = _combcut + " & (AM > 5000)",
                                    MotherCut        = _bcut,
                                    Preambulo        = preambulo)
    
    return (
      Selection(name+"_DPi",
                Algorithm          = _CombineD,
                RequiredSelections = [ DSel, pionSel ] ))
  
  
  #####################################################
  def _makeB2DPiSS(self, name, DSel, pionSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS",
                 "allpi = ((('pi+') == ABSID) | (('K+') == ABSID)) ",
                 "sumpt    = SUMTREE( allpi , PT )"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"


    _bcut    = "((CHILD(BPVVDCHI2,1))   <   "   + config['FDCHI2_TAU']       + ") & "\
      "(MIPCHI2DV(PRIMARY)   <              "   + config['IPCHI2_B_MU']        + ") & "\
      "(BPVVD          <   "   + config['FD_B_MU']      + ") & "\
      "(PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) & "\
      "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV)) &"\
      "((CHILD(MIPCHI2DV(PRIMARY),1)) > "   + config['IPCHI2_B_TAU_MU']+")   "
 

    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ pi+]cc"],
                                    CombinationCut   = _combcut + " & (AM > 5000)",
                                    MotherCut        = _bcut,
                                    Preambulo        = preambulo)
    
    return (
      Selection(name+"_DPiSS",
                Algorithm          = _CombineD,
                RequiredSelections = [ DSel, pionSel ] ))
  
  #####################################################
  def _filterTau(self, name, tauInput, config) :
    """
      Tau filter
      """
    _cut =       "(PT > 0*MeV) "
    
    _preambulo = [ "c1c2c3 = ((('pi+') == ABSID) | (('K+') == ABSID))" ,
                   "ipsm    = MINTREE( c1c2c3 , MIPCHI2DV(PRIMARY) )"]
    
    _filter   = FilterDesktop( Code = _cut, Preambulo = _preambulo)
#    _filter   = FilterDesktop( Code = _cut)
    
    return Selection(name,
                     Algorithm = _filter,
                     RequiredSelections = [tauInput]
                     )
  
  #####################################################
  def _makeTauSS(self, name, pionSel, config) :

     preambulo = [ "c1c2c3 = ((('pi+') == ABSID) | (('K+') == ABSID))" ,
                   "ipsm    = MINTREE( c1c2c3 , MIPCHI2DV(PRIMARY) )"]
     
        
     _combcut = " (AM  > "           + config['MASS_LOW_TAU']    + "*MeV) & "\
                " (AM  < "           + config['MASS_HIGH_TAU']   + "*MeV)  & "\
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
  
     
     _CombineTau   = CombineParticles( DecayDescriptors = ["[tau+ -> pi+ pi+ pi+]cc"],
                                       CombinationCut   = _combcut, 
                                       MotherCut        = _bcut + "& (M>500.*MeV) & (M<2000.*MeV)",
                                       Preambulo        = preambulo)    

     return Selection(name,
                      Algorithm          = _CombineTau,
                      RequiredSelections = [pionSel]
                      )
  #####################################################
  def _makeD(self, name, pionSel, kaonSel, config) :

     _preambulo = [ "c1c2c3 = ((('pi+') == ABSID) | (('K+') == ABSID))" ,
                    "ipsm    = MINTREE( c1c2c3 , MIPCHI2DV(PRIMARY) )"]
            
     _combcut = " (AM  > "           + config['MASS_LOW_D']    + "*MeV) & "\
                " (AM  < "           + config['MASS_HIGH_D']   + "*MeV)  & "\
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
  def _makePions(self, name, config) :
    """
      Pion selection for B -> D pi
      """
    _code = self._muFinalStateKinematicCuts(config) 
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
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
  def _makeMuons(self, name, config) :
    """                     
      Muon selection          
      """                     
    _code = self._muFinalStateKinematicCuts(config) +" & (HASMUON) & (PIDmu > 0)"
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,  
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdTightMuons ] )    
  
  #####################################################   
  def _hadFinalStateKinematicCuts(self, config) :
    _code = "(P  > "                    + config['P_HAD_ALL_FINAL_STATE']     + "*MeV) & "\
      "(PT > "                    + config['PT_HAD_ALL_FINAL_STATE']          + "*MeV) & "\
      "(MIPCHI2DV(PRIMARY) > "    + config['IPCHI2_HAD_ALL_FINAL_STATE']      + ")     & "\
      "(TRCHI2DOF < "             + config['TRACKCHI2_HAD_ALL_FINAL_STATE']   + ")     & "\
      "(TRGHOSTPROB < "           + config['TRGHOPROB_HAD_ALL_FINAL_STATE']   + ")     "

    return _code
  
  #####################################################   
  def _muFinalStateKinematicCuts(self, config) :
    _code = "(P  > "                    + config['P_MU']           + "*MeV) & "\
      "(PT > "                    + config['PT_MU']          + "*MeV) & "\
      "(MIPCHI2DV(PRIMARY) > "    + config['IPCHI2_MU']      + ")     & "\
      "(TRCHI2DOF < "             + config['TRACKCHI2_MU']   + ")     & "\
      "(TRGHOSTPROB < "           + config['TRGHOPROB_MU']   + ")      "

    return _code
  
  #####################################################
  # TISTOSSING
  #####################################################
  
  def _makeTISTOSFilter(self,name,specs):
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter
  
  def _makeTOS(self, name, sel):
    ''' TOS filters selections'''
    if ((name.find('TauMu') > -1) or (name.find('DMu') > -1)):
      tisTosFilter = self._makeTISTOSFilter(name,{HLT_DECISIONS_MUON+'%TOS':0})
    else :
      tisTosFilter = self._makeTISTOSFilter(name,{HLT_DECISIONS_HAD+'%TOS':0})
    return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])
  

  
