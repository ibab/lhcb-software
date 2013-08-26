__author__ = ['V. Gligorov, Ch. Elsasser']
__date__ = '24/05/2010,23/08/2012'
__version__ = '$Revision: 1.2 $'
# Stripping line by V. Gligorov for B->mu tau, B->tau tau, B->D mu and B->D D
# Strongly modified for Stripping20 by Ch. Elsasser
# added Dpi mode by G. Mancinelli

"""
  B->TauTau, B->TauMu
  """

config_params =  {  'PT_HAD_ALL_FINAL_STATE'        : '200',  # MeV
  'PTMAX_HAD_ALL_FINAL_STATE'     : '500',  # MeV
  'P_HAD_ALL_FINAL_STATE'         : '2000', # MeV
  'IPCHI2_HAD_ALL_FINAL_STATE'    : '9',    # dimensionless
  'TRACKCHI2_HAD_ALL_FINAL_STATE' : '4',    # dimensionless
  'PID_HAD_ALL_FINAL_STATE'       : '5',    # dimensionless
  'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.35', # dimensionless
  #
  'PT_MU'                         : '1200', # MeV
  'P_MU'                          : '6000', # MeV 
  'IPCHI2_MU'                     : '16',   # MeV
  'TRACKCHI2_MU'                  : '4',    # dimensionless
  'TRGHOPROB_MU'                  : '0.35', # dimensionless
  #
  'PT_B_TT'                       : '5000', # MeV
  'PT_B_TT_HIGH'                  : '10000',# MeV 
  'PT_B_TM'                       : '2000', # MeV
  'PT_B_TM_HIGH'                  : '7500', # MeV  
  'VCHI2_B'                       : '100',  # dimensionless
  'FDCHI2_B'                      : '144',  # dimensionless
  'DIRA_B'                        : '0.99', # dimensionless
  'MASS_LOW_B'                    : '2000', # MeV  
  'MASS_HIGH_B'                   : '7000', # MeV (old value: 5750)
  'MCOR_LOW_B'                    : '4000', # MeV
  'MCOR_HIGH_B'                   :'10000', # MeV (old value: 7000)
  'MCOR_LOW_SB'                   : '2000', # MeV
  'MCOR_HIGH_SB'                  : '4000', # MeV 
  'MIPCHI2_B'                     : '150',  # dimensionless 
  'MIPCHI2_B_HIGH'                : '36',   # dimensionless  
  #
  'PT_TAU'                        : '1250', # MeV
  'VCHI2_TAU'                     : '12',   # dimensionless
  'IPCHI2_TAU'                    : '9',    # dimensionless
  'FDCHI2_TAU'                    : '16',   # dimensionless
  'FDRHO_TAU'                     : '0.1',  # mm
  'FDZ_TAU'                       : '2.0',  # mm
  'DOCA_TAU'                      : '0.4',  # mm
  'DIRA_TAU'                      : '0.98', # dimensionless
  'MASS_LOW_TAU'                  : '500',  # MeV
  'MASS_HIGH_TAU'                 : '1800', # MeV
  #
  'PT_DIPI'                       : '200',  # MeV
  'VCHI2_DIPI'                    : '20',   # dimensionless
  'IPCHI2_DIPI'                   : '1',    # dimensionless
  'FDCHI2_DIPI'                   : '9',    # dimensionless
  'FDRHO_DIPI'                    : '0.05', # mm
  'FDZ_DIPI'                      : '1.0',  # mm
  'DOCA_DIPI'                     : '0.4',  # mm
  'DIRA_DIPI'                     : '0.9',  # dimensionless
  #
  'PT_B_CHILD_BEST'               : '1800', # MeV
  'P_B_CHILD_BEST'                : '10000',# MeV
  'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
  'PT_B_TAU_CHILD_BEST'           : '3000', # MeV
  'IPCHI2_B_TAU_CHILD_BEST'       : '16',   # dimensionless
  #
  'MASS_LOW_D'                    : '1800', # MeV
  'MASS_HIGH_D'                   : '2030', # MeV  
  #
  'B2TauTau_TOSLinePrescale'          : 1,
  'B2TauTau_TOSLinePostscale'         : 1,
  'B2TauTau_SB_TOSLinePrescale'          : 0.05,
  'B2TauTau_SB_TOSLinePostscale'         : 1,
  'B2DD_TOSLinePrescale'              : 1,
  'B2DD_TOSLinePostscale'             : 1,
  'B2TauMu_TOSLinePrescale'           : 1,
  'B2TauMu_TOSLinePostscale'          : 1,
  'B2TauMu_SB_TOSLinePrescale'          : 0.05,
  'B2TauMu_SB_TOSLinePostscale'         : 1,
  'B2DMu_TOSLinePrescale'             : 0.2,
  'B2DMu_TOSLinePostscale'            : 1,
  'B2DPi_TOSLinePrescale'             : 0.1,
  'B2DPi_TOSLinePostscale'            : 1,
  'B2TauTau_TISLinePrescale'          : 1,
  'B2TauTau_TISLinePostscale'         : 1,
  'B2DD_TISLinePrescale'              : 1,
  'B2DD_TISLinePostscale'             : 1,
  'B2TauMu_TISLinePrescale'           : 1,
  'B2TauMu_TISLinePostscale'          : 1,
  'B2DMu_TISLinePrescale'             : 0.2,
  'B2DMu_TISLinePostscale'            : 1.,
  'B2DPi_TISLinePrescale'             : 0.1,
  'B2DPi_TISLinePostscale'            : 1.}

__all__ = ('B2XTauConf')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsKaons, StdNoPIDsPions, StdLooseMuons
from StandardParticles import StdLoosePions, StdTightPions
from StandardParticles import StdLooseKaons 

default_name = "B2XTau"
HLT_DECISIONS_HAD   = "Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision"
HLT_DECISIONS_MUON  = "Hlt2(TopoMu|SingleMuon).*Decision"
HLT1_DECISIONS_TIS  = "Hlt1TrackAllL0Decision"
HLT2_DECISIONS_TIS  = "Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision"

class B2XTauConf(LineBuilder) :
  
  """
    Builder for B->TauTau, B->TauMu
    """
  
  TauTau_TOSLine    = None
  TauTau_SS_TOSLine = None
  TauTau_SB_TOSLine    = None
  TauTau_SS_TOSLine = None
  TauMu_TOSLine     = None
  TauMu_SB_TOSLine    = None
  TauMu_SS_TOSLine  = None
  DMu_TOSLine       = None
  DMu_SS_TOSLine    = None
  DPi_TOSLine       = None
  DPi_SS_TOSLine    = None
  DD_TOSLine        = None 
  DD_SS_TOSLine     = None 
  
  TauTau_TISLine    = None
  TauTau_SS_TISLine = None
  TauMu_TISLine     = None
  TauMu_SS_TISLine  = None
  DMu_TISLine       = None
  DMu_SS_TISLine    = None
  DPi_TISLine       = None
  DPi_SS_TISLine    = None
  DD_TISLine        = None
  DD_SS_TISLine     = None
  
  __configuration_keys__ = (  'PT_HAD_ALL_FINAL_STATE',
                            'PTMAX_HAD_ALL_FINAL_STATE',
                            'P_HAD_ALL_FINAL_STATE',
                            'IPCHI2_HAD_ALL_FINAL_STATE',
                            'TRACKCHI2_HAD_ALL_FINAL_STATE',
                            'PID_HAD_ALL_FINAL_STATE',
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
                            'DIRA_B',
                            'MASS_LOW_B',
                            'MASS_HIGH_B',
                            'MCOR_LOW_B',
                            'MCOR_HIGH_B',
                             'MCOR_LOW_SB',
                            'MCOR_HIGH_SB',
                            'MIPCHI2_B',
                            'MIPCHI2_B_HIGH',
                            #   
                            'PT_TAU',
                            'VCHI2_TAU',
                            'IPCHI2_TAU',
                            'FDCHI2_TAU',
                            'FDRHO_TAU',
                            'FDZ_TAU',
                            'DOCA_TAU',
                            'DIRA_TAU',
                            'MASS_LOW_TAU',
                            'MASS_HIGH_TAU',
                            #
                            'PT_DIPI',
                            'VCHI2_DIPI',
                            'IPCHI2_DIPI',
                            'FDCHI2_DIPI',
                            'FDRHO_DIPI',
                            'FDZ_DIPI',
                            'DOCA_DIPI',
                            'DIRA_DIPI',
                            #
                            'PT_B_CHILD_BEST',
                            'P_B_CHILD_BEST',
                            'IPCHI2_B_CHILD_BEST',   
                            'PT_B_TAU_CHILD_BEST', 
                            'IPCHI2_B_TAU_CHILD_BEST',
                            #
                            'MASS_LOW_D',
                            'MASS_HIGH_D',
                            #
                            'B2TauTau_TOSLinePrescale',
                            'B2TauTau_TOSLinePostscale',
                            'B2TauTau_SB_TOSLinePrescale',
                            'B2TauTau_SB_TOSLinePostscale',
                            'B2DD_TOSLinePrescale',
                            'B2DD_TOSLinePostscale',
                            'B2TauMu_TOSLinePrescale',
                            'B2TauMu_TOSLinePostscale',
                            'B2TauMu_SB_TOSLinePrescale',
                            'B2TauMu_SB_TOSLinePostscale',
                            'B2DMu_TOSLinePrescale',
                            'B2DMu_TOSLinePostscale',
                            'B2DPi_TOSLinePrescale',
                            'B2DPi_TOSLinePostscale',
                            'B2TauTau_TISLinePrescale',
                            'B2TauTau_TISLinePostscale',
                            'B2DD_TISLinePrescale',
                            'B2DD_TISLinePostscale',
                            'B2TauMu_TISLinePrescale',
                            'B2TauMu_TISLinePostscale',
                            'B2DMu_TISLinePrescale',
                            'B2DMu_TISLinePostscale',  
                            'B2DPi_TISLinePrescale',
                            'B2DPi_TISLinePostscale'  
                            )
  
  def __init__(self, name, config):
    LineBuilder.__init__(self, name, config)
    #
    selPions        = self._makePions(      name    = "PionsFor"+name, 
                                      config  = config)
    selKaons        = self._makeKaons(      name    = "KaonsFor"+name,
                                      config  = config)
    selMuons        = self._makeMuons(      name    = "MuonsFor"+name,
                                      config  = config)
    
    selDipionsD     = self._makeDipionsforD(    name    = "DipionsDFor"+name,
                                            pionSel = selPions,
                                            kaonSel = selKaons,
                                            config  = config)
    selDipions      = DataOnDemand("Phys/StdLooseDetachedDipion/Particles")
    #
    
    rawTau          = DataOnDemand("Phys/StdLooseDetachedTau3pi/Particles")
    
    selTau          = self._filterTau(name       = name+"_TauFilter",
                                      tauInput   = rawTau,
                                      config     = config)
    
    selD            = self._makeD(name       = "DFor"+name,
                                  pionSel    = selPions,
                                  dipionSel  = selDipionsD,
                                  config     = config)
    #
    selB2TauTau,selB2DD     = self._makeB2XX(   name    = name,
                                             tauSel  = selTau,
                                             DSel    = selD,    
                                             config  = config)
    selB2TauTauSB     = self._makeB2XXSB(   name    = name,
                                                tauSel  = selTau,
                                                DSel    = selD,    
                                                config  = config)
    selB2TauTauSS,selB2DDSS = self._makeB2XXSS( name    = name,
                                               tauSel  = selTau,
                                               DSel    = selD,    
                                               config  = config)

    selB2TauMu,selB2DMu     = self._makeB2XMu(  name    = name,
                                              tauSel  = selTau,
                                              DSel    = selD,
                                              muonSel = selMuons,
                                              config  = config)
    
    selB2DPi     = self._makeB2DPi(  name    = name,
                                     DSel    = selD,
                                     pionSel = selPions,
                                     config  = config)
    
    selB2TauMuSB         =  self._makeB2XMuSB(  name    = name,
                                              tauSel  = selTau,
                                              DSel    = selD,
                                              muonSel = selMuons,
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
    #
    self.TauTau_TOSLine    = StrippingLine(name+"_TauTau_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_TOSLinePrescale'],
                                           postscale   = config['B2TauTau_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
                                           )
    self.TauTau_SS_TOSLine = StrippingLine(name+"_TauTau_SameSign_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_TOSLinePrescale'],
                                           postscale   = config['B2TauTau_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauTauSS",selB2TauTauSS)
                                           )
    self.TauTau_SB_TOSLine    = StrippingLine(name+"_TauTau_SB_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_SB_TOSLinePrescale'],
                                           postscale   = config['B2TauTau_SB_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauTauSB",selB2TauTauSB)
                                           )

    self.DD_TOSLine        = StrippingLine(name+"_DD_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DD_TOSLinePrescale'],
                                           postscale   = config['B2DD_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForDD",selB2DD)
                                           )
    self.DD_SS_TOSLine     = StrippingLine(name+"_DD_SameSign_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DD_TOSLinePrescale'],
                                           postscale   = config['B2DD_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForDDSS",selB2DDSS)
                                           )
    self.TauMu_TOSLine     = StrippingLine(name+"_TauMu_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_TOSLinePrescale'],
                                           postscale   = config['B2TauMu_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauMu",selB2TauMu)
                                           )
    self.TauMu_SS_TOSLine  = StrippingLine(name+"_TauMu_SameSign_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_TOSLinePrescale'],
                                           postscale   = config['B2TauMu_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauMuSS",selB2TauMuSS)
                                           )
    self.TauMu_SB_TOSLine     = StrippingLine(name+"_TauMu_SB_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_SB_TOSLinePrescale'],
                                           postscale   = config['B2TauMu_SB_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForTauMuSB",selB2TauMuSB)
                                           )
    self.DMu_TOSLine       = StrippingLine(name+"_DMu_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DMu_TOSLinePrescale'],
                                           postscale   = config['B2DMu_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForDMu",selB2DMu)
                                           )
    self.DPi_TOSLine       = StrippingLine(name+"_DPi_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DPi_TOSLinePrescale'],
                                           postscale   = config['B2DPi_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForDPi",selB2DPi)
                                           )
    self.DMu_SS_TOSLine    = StrippingLine(name+"_DMu_SameSign_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DMu_TOSLinePrescale'],
                                           postscale   = config['B2DMu_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForDMuSS",selB2DMuSS)
                                           )
    self.DPi_SS_TOSLine    = StrippingLine(name+"_DPi_SameSign_TOSLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DPi_TOSLinePrescale'],
                                           postscale   = config['B2DPi_TOSLinePostscale'],
                                           selection   = self._makeTOS(name+"_TOSForDPiSS",selB2DPiSS)
                                           )
    #
    self.TauTau_TISLine    = StrippingLine(name+"_TauTau_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_TISLinePrescale'],
                                           postscale   = config['B2TauTau_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForTauTau",selB2TauTau)
                                           )
    self.TauTau_SS_TISLine = StrippingLine(name+"_TauTau_SameSign_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauTau_TISLinePrescale'],
                                           postscale   = config['B2TauTau_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForTauTauSS",selB2TauTauSS)
                                           )
    self.DD_TISLine        = StrippingLine(name+"_DD_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DD_TISLinePrescale'],
                                           postscale   = config['B2DD_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForDD",selB2DD)
                                           )
    self.DD_SS_TISLine     = StrippingLine(name+"_DD_SameSign_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DD_TISLinePrescale'],
                                           postscale   = config['B2DD_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForDDSS",selB2DDSS)
                                           )
    self.TauMu_TISLine     = StrippingLine(name+"_TauMu_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_TISLinePrescale'],
                                           postscale   = config['B2TauMu_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForTauMu",selB2TauMu)
                                           )
    self.TauMu_SS_TISLine  = StrippingLine(name+"_TauMu_SameSign_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2TauMu_TISLinePrescale'],
                                           postscale   = config['B2TauMu_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForTauMuSS",selB2TauMuSS)
                                           )
    self.DMu_TISLine       = StrippingLine(name+"_DMu_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DMu_TISLinePrescale'],
                                           postscale   = config['B2DMu_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForDMu",selB2DMu)
                                           )
    self.DPi_TISLine       = StrippingLine(name+"_DPi_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DPi_TISLinePrescale'],
                                           postscale   = config['B2DPi_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForDPi",selB2DPi)
                                           )
    self.DMu_SS_TISLine    = StrippingLine(name+"_DMu_SameSign_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DMu_TISLinePrescale'],
                                           postscale   = config['B2DMu_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForDMuSS",selB2DMuSS)
                                           )
    self.DPi_SS_TISLine    = StrippingLine(name+"_DPi_SameSign_TISLine",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                           prescale    = config['B2DPi_TISLinePrescale'],
                                           postscale   = config['B2DPi_TISLinePostscale'],
                                           selection   = self._makeTIS(name+"_TISForDPiSS",selB2DPiSS)
                                           )
    #
    self.registerLine( self.TauTau_TOSLine )
    self.registerLine( self.TauTau_SB_TOSLine )
    self.registerLine( self.DD_TOSLine )
    self.registerLine( self.TauMu_TOSLine )
    self.registerLine( self.TauMu_SB_TOSLine )
    self.registerLine( self.DMu_TOSLine )    
    self.registerLine( self.DPi_TOSLine )    
    self.registerLine( self.TauTau_TISLine )
    self.registerLine( self.DD_TISLine )
    self.registerLine( self.TauMu_TISLine )
    self.registerLine( self.DMu_TISLine )
    self.registerLine( self.DPi_TISLine )
    self.registerLine( self.TauTau_SS_TOSLine )
    self.registerLine( self.DD_SS_TOSLine )
    self.registerLine( self.TauMu_SS_TOSLine )
    self.registerLine( self.DMu_SS_TOSLine )    
    self.registerLine( self.DPi_SS_TOSLine )    
    self.registerLine( self.TauTau_SS_TISLine )
    self.registerLine( self.DD_SS_TISLine )
    self.registerLine( self.TauMu_SS_TISLine )
    self.registerLine( self.DMu_SS_TISLine ) 
    self.registerLine( self.DPi_SS_TISLine ) 
  
  
  #####################################################
  def _makeB2XX(self, name, tauSel, DSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > "                    + config['PT_B_TT']       + "*MeV) & "\
      "(AM  > "                    + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "                    + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
      "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
      "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
      "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
      "((PT                >   "   + config['PT_B_TT_HIGH']  + "*MeV) | \
      (BPVIPCHI2()       <   "   + config['MIPCHI2_B_HIGH']+ ")) &"\
      "(INGENERATION((PT   >   "   + config['PT_B_TAU_CHILD_BEST']+ "*MeV),1)) & "\
      "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_TAU_CHILD_BEST'] + "),1)) & "\
      "(INGENERATION((PT   >   "   + config['PT_B_CHILD_BEST']+ "*MeV),2)) & "\
      "(INGENERATION((P    >   "   + config['P_B_CHILD_BEST'] + "*MeV),2)) & "\
      "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_CHILD_BEST'] + "),2)) & "\
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
                      RequiredSelections = [ DSel ] ))
    #####################################################
  def _makeB2XXSB(self, name, tauSel, DSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > "                    + config['PT_B_TT']       + "*MeV) & "\
      "(AM  > "                    + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "                    + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
      "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
      "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
      "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
      "((PT                >   "   + config['PT_B_TT_HIGH']  + "*MeV) | \
      (BPVIPCHI2()       <   "   + config['MIPCHI2_B_HIGH']+ ")) &"\
      "(INGENERATION((PT   >   "   + config['PT_B_TAU_CHILD_BEST']+ "*MeV),1)) & "\
      "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_TAU_CHILD_BEST'] + "),1)) & "\
      "(INGENERATION((PT   >   "   + config['PT_B_CHILD_BEST']+ "*MeV),2)) & "\
      "(INGENERATION((P    >   "   + config['P_B_CHILD_BEST'] + "*MeV),2)) & "\
      "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_CHILD_BEST'] + "),2)) & "\
      "(in_range("+config['MCOR_LOW_SB']+"*MeV,MCOR,"+config['MCOR_HIGH_SB']+"*MeV))"      
    
    _CombineTau = CombineParticles( DecayDescriptors = ["B0 -> tau+ tau-"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    _CombineD   = CombineParticles( DecayDescriptors = ["B0 -> D+ D-"],
                                   CombinationCut   = _combcut + " & (AM > 5000)",
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    return (Selection(name+"_TauTauSB",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel ] ))
    
  #####################################################
  def _makeB2XXSS(self, name, tauSel, DSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > "                    + config['PT_B_TT']       + "*MeV) & "\
      "(AM  > "                    + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "                    + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
      "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
      "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
      "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
      "((PT                >   "   + config['PT_B_TT_HIGH']  + "*MeV) | \
      (BPVIPCHI2()       <   "   + config['MIPCHI2_B_HIGH']+ ")) &"\
      "(INGENERATION((PT   >   "   + config['PT_B_TAU_CHILD_BEST']+ "*MeV),1)) & "\
      "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_TAU_CHILD_BEST'] + "),1)) & "\
      "(INGENERATION((PT   >   "   + config['PT_B_CHILD_BEST']+ "*MeV),2)) & "\
      "(INGENERATION((P    >   "   + config['P_B_CHILD_BEST'] + "*MeV),2)) & "\
      "(INGENERATION((MIPCHI2DV(PRIMARY) >" + config['IPCHI2_B_CHILD_BEST'] + "),2)) & "\
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
  def _makeB2XMu(self, name, tauSel, DSel, muonSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
      "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
      "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
      "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
      "((PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) | \
       (BPVIPCHI2()        <   "   + config['MIPCHI2_B_HIGH']+ "))&"\
      "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    
    
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
                      RequiredSelections = [ DSel, muonSel ] ))
  
    #####################################################
  def _makeB2XMuSB(self, name, tauSel, DSel, muonSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
      "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
      "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
      "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
      "((PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) | \
       (BPVIPCHI2()        <   "   + config['MIPCHI2_B_HIGH']+ "))&"\
      "(in_range("+config['MCOR_LOW_SB']+"*MeV,MCOR,"+config['MCOR_HIGH_SB']+"*MeV))"    
    
    _CombineTau = CombineParticles( DecayDescriptors = ["[B0 -> tau+ mu-]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ mu-]cc"],
                                   CombinationCut   = _combcut,
                                   MotherCut        = _bcut,
                                   Preambulo        = preambulo)
    
    return (Selection(name+"_TauMuSB",
                      Algorithm          = _CombineTau,
                      RequiredSelections = [ tauSel, muonSel ] ))
 
  
  #####################################################
  def _makeB2XMuSS(self, name, tauSel, DSel, muonSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
      "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
      "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
      "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
      "((PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) | \
       (BPVIPCHI2()        <   "   + config['MIPCHI2_B_HIGH']+ "))&"\
      "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    
    
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
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
               "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
               "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
               "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
               "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
               "((PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) | \
               (BPVIPCHI2()        <   "   + config['MIPCHI2_B_HIGH']+ "))&"\
    "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    
    
    
    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ pi-]cc"],
                                    CombinationCut   = _combcut,
                                    MotherCut        = _bcut,
                                    Preambulo        = preambulo)
    
    return (
      Selection(name+"_DPi",
                Algorithm          = _CombineD,
                RequiredSelections = [ DSel, pionSel ] ))
  
  
  #####################################################
  def _makeB2DPiSS(self, name, DSel, pionSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
    
    _combcut = "(APT > " + config['PT_B_TM']       + "*MeV) & "\
      "(AM  > "          + config['MASS_LOW_B']    + "*MeV) & "\
      "(AM  < "          + config['MASS_HIGH_B']   + "*MeV)"
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
               "(BPVDIRA            >   "   + config['DIRA_B']        + ") & "\
               "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
               "(BPVIPCHI2()        <   "   + config['MIPCHI2_B']     + ") & "\
               "((PT                >   "   + config['PT_B_TM_HIGH']  + "*MeV) | \
               (BPVIPCHI2()        <   "   + config['MIPCHI2_B_HIGH']+ "))&"\
    "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    
    
    _CombineD   = CombineParticles( DecayDescriptors = ["[B0 -> D+ pi+]cc"],
                                    CombinationCut   = _combcut,
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
    _cut = "(MAXTREE('pi+'==ABSID,PT)               > " + config['PTMAX_HAD_ALL_FINAL_STATE']      + ") & "\
      "(MINTREE('pi+'==ABSID,PT)                    > " + config['PT_HAD_ALL_FINAL_STATE']         + ") & "\
      "(MINTREE('pi+'==ABSID,P)                     > " + config['P_HAD_ALL_FINAL_STATE']          + ") & "\
      "(MINTREE('pi+'==ABSID,MIPCHI2DV(PRIMARY))    > " + config['IPCHI2_HAD_ALL_FINAL_STATE']     + ") & "\
      "(MAXTREE('pi+'==ABSID,PIDK)                  < " + config['PID_HAD_ALL_FINAL_STATE']        + ") & "\
      "(MAXTREE('pi+'==ABSID,TRGHOSTPROB)           < " + config['TRGHOPROB_HAD_ALL_FINAL_STATE']  + ") & "\
      "(BPVVDCHI2                                   > " + config['FDCHI2_TAU']                     + ") & "\
      "(BPVVDRHO                                    > " + config['FDRHO_TAU']                   + "*mm) & "\
      "(BPVVDZ                                      > " + config['FDZ_TAU']                     + "*mm) & "\
      "(VFASPF(VCHI2PDOF)                           < " + config['VCHI2_TAU']                      + ") & "\
      "(M                                           > " + config['MASS_LOW_TAU']               + "*MeV) & "\
      "(M                                           < " + config['MASS_HIGH_TAU']              + "*MeV) & "\
      "(PT                                          > " + config['PT_TAU']                     + "*MeV)"
    
    _filter   = FilterDesktop( Code = _cut)
    
    return Selection(name,
                     Algorithm = _filter,
                     RequiredSelections = [tauInput]
                     )
  
  #####################################################
  def _makeTau(self, name, pionSel, dipiSel, config) :
    """
      Tau selection
      """
    getcuts = self._hadIntermediateKinematicCuts(config)
    
    _combcut = getcuts[0] +\
      "(AM      > "    + config['MASS_LOW_TAU']         + "*MeV)    & "\
      "(AM      < "    + config['MASS_HIGH_TAU']        + "*MeV)      "
    
    _taucut  = getcuts[1] + "& (MAXTREE('pi+'==ABSID,PT) > " + config['PT_HAD_ALL_FINAL_STATE']  + ")"
    
    _Combine = CombineParticles(DecayDescriptor = "[tau+ -> pi+ rho(770)0]cc ",
                                CombinationCut  = _combcut,
                                MotherCut       = _taucut)   
    
    return Selection(name,
                     Algorithm          = _Combine,
                     RequiredSelections = [ pionSel, dipiSel ] )
  #####################################################
  def _makeD(self, name, pionSel, dipionSel, config) :
    getcuts = self._hadIntermediateKinematicCuts(config)
    
    _combcut = getcuts[0] +\
      "(AM      > "    + config['MASS_LOW_D']         + "*MeV)    & "\
      "(AM      < "    + config['MASS_HIGH_D']        + "*MeV)      "
    
    _dcut  = getcuts[1] + "& (MAXTREE('pi+'==ABSID,PT) > " + config['PT_HAD_ALL_FINAL_STATE']  + ")" 
    
    _Combine = CombineParticles(DecayDescriptors = ["[D+ -> rho(770)0 pi+]cc ",
                                                    "[D+ -> rho(770)0 pi+]cc "],
                                CombinationCut   = _combcut,
                                MotherCut        = _dcut)
    
    return Selection(name,
                     Algorithm          = _Combine,
                     RequiredSelections = [ pionSel, dipionSel  ] )
  #####################################################
  def _makeDipionsforD(self, name, pionSel, kaonSel, config) :
    """
      Dipion selection for D
      """
    
    getcuts = self._hadDiPionKinematicCuts(config)
    
    _combcut = getcuts[0] +\
      "(AM      < "    + '1670'       + "*MeV)      "
    
    _dipicut = getcuts[1]
    
    _Combine = CombineParticles(DecayDescriptors = ["[rho(770)0 -> K+ pi-]cc",
                                                    "rho(770)0 -> K+ K-"],
                                CombinationCut   = _combcut,
                                MotherCut        = _dipicut)   
    
    return Selection(name,
                     Algorithm          = _Combine,
                     RequiredSelections = [ pionSel, kaonSel ] )
  #####################################################
  def _makeDipionsforTau(self, name, pionSel, config) :
    """
      Dipion selection for Tau
      """
    
    getcuts = self._hadDiPionKinematicCuts(config)
    
    _combcut = getcuts[0] +\
      "(AM      < "    + '1900'         + "*MeV)      "
    
    _dipicut = getcuts[1]
    
    _Combine = CombineParticles(DecayDescriptor = "rho(770)0 -> pi+ pi-",
                                CombinationCut  = _combcut,
                                MotherCut       = _dipicut)   
    
    return Selection(name,
                     Algorithm          = _Combine,
                     RequiredSelections = [ pionSel ] )
  #####################################################
  def _makePions(self, name, config) :
    """
      Pion selection
      """
    _code = self._hadFinalStateKinematicCuts(config) +"& ~(ISMUON)"#+ " & (PIDp < 5) & (PIDe < 5)"
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdTightPions ] )
  
  #####################################################
  def _makeKaons(self, name, config) :
    """ 
      Kaon selection
      """
    _code = self._hadFinalStateKinematicCuts(config) +"& ~(ISMUON)"#+ " & ((PIDK-PIDe) > -5)" 
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdLooseKaons ] )
  
  #####################################################
  def _makeMuons(self, name, config) :
    """                     
      Muon selection          
      """                     
    _code = self._muFinalStateKinematicCuts(config) +" & (HASMUON) & (ISMUON) & (PIDmu > 0) & (TRGHOSTPROB<0.5)"
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,  
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdLooseMuons ] )    
  
  #####################################################   
  def _hadIntermediateKinematicCuts(self, config) : 
    _combcode   = "(APT     > "                + config['PT_TAU']       + "*MeV) & "\
      "(AMAXDOCA('') < "           + config['DOCA_TAU']     + "*mm)  & "
    _postvxcode = "(VFASPF(VCHI2PDOF)  < "     + config['VCHI2_TAU']    + ")     & "\
      "(BPVVDCHI2          > "     + config['FDCHI2_TAU']   + ")     & "\
      "(BPVVDRHO           > "     + config['FDRHO_TAU']    + "*mm)  & "\
      "(BPVVDZ             > "     + config['FDZ_TAU']      + "*mm)  & "\
      "(BPVDIRA            > "     + config['DIRA_TAU']     + ")      "
    return [_combcode,_postvxcode]
  
  #####################################################   
  def _hadDiPionKinematicCuts(self, config) : 
    _combcode   = "(APT     > "                + config['PT_DIPI']       + "*MeV) & "\
      "(AMAXDOCA('') < "           + config['DOCA_DIPI']     + "*mm)  & "
    _postvxcode = "(VFASPF(VCHI2PDOF)  < "     + config['VCHI2_DIPI']    + ")     & "\
      "(BPVVDCHI2          > "     + config['FDCHI2_DIPI']   + ")     & "\
      "(BPVVDRHO           > "     + config['FDRHO_DIPI']    + "*mm)  & "\
      "(BPVVDZ             > "     + config['FDZ_DIPI']      + "*mm)  & "\
      "(BPVDIRA            > "     + config['DIRA_DIPI']     + ")      "
    return [_combcode,_postvxcode]
  
  
  #####################################################   
  def _hadFinalStateKinematicCuts(self, config) :
    _code = "(P  > "                    + config['P_HAD_ALL_FINAL_STATE']     + "*MeV) & "\
      "(PT > "                    + config['PT_HAD_ALL_FINAL_STATE']          + "*MeV) & "\
      "(MIPCHI2DV(PRIMARY) > "    + config['IPCHI2_HAD_ALL_FINAL_STATE']      + ")     & "\
      "(TRCHI2DOF < "             + config['TRACKCHI2_HAD_ALL_FINAL_STATE']   + ")     "
    return _code
  
  #####################################################   
  def _muFinalStateKinematicCuts(self, config) :
    _code = "(P  > "                    + config['P_MU']           + "*MeV) & "\
      "(PT > "                    + config['PT_MU']          + "*MeV) & "\
      "(MIPCHI2DV(PRIMARY) > "    + config['IPCHI2_MU']      + ")     & "\
      "(TRCHI2DOF < "             + config['TRACKCHI2_MU']   + ")     & "\
      "(TRGHOSTPROB < "           + config['TRGHOPROB_MU']   + ") "
    return _code
  
  #####################################################
  # TISTOSING
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
  
  def _makeTIS(self, name, sel):
    ''' TIS filters selections'''
    tisTosFilter = self._makeTISTOSFilter(name,
                                          {HLT1_DECISIONS_TIS+'%TIS':0,HLT2_DECISIONS_TIS+'%TIS':0})
    return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])
