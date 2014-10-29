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
  'BUILDERTYPE'       :       'B2KstTauXConf',
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
  'B2KstTauTau_LinePrescale'               : 1,
  'B2KstTauTau_LinePostscale'              : 1,
  'B2KstTauMu_LinePrescale'                : 1,
  'B2KstTauMu_LinePostscale'               : 1,
  'B2KstTauTau_SameSign_LinePrescale'      : 0.5,
  'B2KstTauTau_SameSign_LinePostscale'     : 1,
  'B2KstTauMu_SameSign_LinePrescale'       : 0.5,
  'B2KstTauMu_SameSign_LinePostscale'      : 1,
  },
  'STREAMS'     : ['Bhadron']
  }
  


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdTightMuons
from StandardParticles import StdLoosePions
from StandardParticles import StdNoPIDsKaons


class B2KstTauXConf(LineBuilder) :
  
  """
    Builder for B->K*TauTau, B->K*TauMu
    """

  TauTau_Line    = None
  TauTau_SS_Line = None
  TauMu_Line     = None
  TauMu_SS_Line  = None
 
  __configuration_keys__ = (    'PT_MU'                ,
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
                                'B2KstTauTau_LinePrescale'            ,
                                'B2KstTauTau_LinePostscale'            ,
                                'B2KstTauMu_LinePrescale'              ,
                                'B2KstTauMu_LinePostscale'             ,
                                'B2KstTauTau_SameSign_LinePrescale'    ,
                                'B2KstTauTau_SameSign_LinePostscale'   ,
                                'B2KstTauMu_SameSign_LinePrescale'     ,
                                'B2KstTauMu_SameSign_LinePostscale'    
                            )
  
  def __init__(self, name, config):

    LineBuilder.__init__(self, name, config)
    #
    selKstar        = self._makeKstar(      name    = "Kstar"+name,
                                      config  = config)
    rawTau          = DataOnDemand("Phys/StdTightDetachedTau3pi/Particles")
    
    selMuons        = self._makeMuons(      name    = "MuonsFor"+name,
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

    self.TauTau_Line    = StrippingLine(name+"_TauTau_Line",
                                           #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauTau_LinePrescale'],
                                        postscale   = config['B2KstTauTau_LinePostscale'],
                                        MDSTFlag = True,
                                        selection = selB2KstTauTau
                                        )

    self.TauTau_SS_Line = StrippingLine(name+"_TauTau_SameSign_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauTau_SameSign_LinePrescale'],
                                        postscale   = config['B2KstTauTau_SameSign_LinePostscale'],
                                        MDSTFlag = True,
                                        selection   = selB2KstTauTauSS
                                        )
    self.TauMu_Line     = StrippingLine(name+"_TauMu_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauMu_LinePrescale'],
                                        postscale   = config['B2KstTauMu_LinePostscale'],
                                        MDSTFlag = True,
                                        selection   = selB2KstTauMu                                      
                                        )
   
    self.TauMu_SS_Line  = StrippingLine(name+"_TauMu_SameSign_Line",
                                        #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                        prescale    = config['B2KstTauMu_SameSign_LinePrescale'],
                                        postscale   = config['B2KstTauMu_SameSign_LinePostscale'],
                                        MDSTFlag = True,
                                        selection   = selB2KstTauMuSS
                                        )
    #

    self.registerLine( self.TauTau_Line )
    self.registerLine( self.TauMu_Line )
    self.registerLine( self.TauTau_SS_Line )
    self.registerLine( self.TauMu_SS_Line )
  
  
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

  def _makeKstar(self, name, config) :
             
     _combcut = " (AM  > "           + config['MASS_LOW_Kst']    + "*MeV) & "\
                " (AM  < "           + config['MASS_HIGH_Kst']   + "*MeV)   "

     
     _bcut = " (PT > "             + config['PT_Kst']   + "*MeV) &"\
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
  

