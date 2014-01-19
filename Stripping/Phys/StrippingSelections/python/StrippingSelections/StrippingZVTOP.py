old__author__ = ['G. Mancinelli, J Cogan']
__date__ = '14/01/2014'
__version__ = '$Revision: 1.2 $'
# Stripping lines by G. Mancinelli, J. Cogan using zvtop

"""
  B->TauTau & High Masses
  """

config_params =  {
  'B2TauTau_LinePrescale'      : 1,
  'B2TauTau_LinePostscale'     : 1,
  'High_LinePrescale'          : 1,
  'High_LinePostscale'         : 1,
  'PT_HAD_ALL_FINAL_STATE'        : '1200',  # MeV
  'PTMAX_HAD_ALL_FINAL_STATE'     : '500',#MeV
  'P_HAD_ALL_FINAL_STATE'         : '6000', # MeV
  'IPCHI2_HAD_ALL_FINAL_STATE'    : '40',    # dimensionless
  'TRACKCHI2_HAD_ALL_FINAL_STATE' : '3',    # dimensionless
  'PID_HAD_ALL_FINAL_STATE'       : '5',    # dimensionless
  'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.3',    # dimensionless
  #
  'PT_B_TT'                       : '1900', # MeV
  'PT_B_TT_HIGH'                  : '2000',# MeV 
  'PT_B_TM'                       : '1900', # MeV
  'PT_B_TM_HIGH'                  : '2000', # MeV  
  'VCHI2_B'                       :   '90', # dimensionless
  'FDCHI2_B'                      : '225',  # dimensionless
  'FD_B'                          : '90',   # mm
  'DIRA_B'                        : '0.99', # dimensionless
  'MASS_LOW_B'                    : '2000', # MeV  
  'MASS_HIGH_B'                   : '7000', # MeV (old value: 5750)
  'MCOR_LOW_B'                    :    '0', # MeV
  'MCOR_HIGH_B'                   :'10000', # MeV (old value: 7000)
  'MIPCHI2_B'                     : '16',  # dimensionless 
  'MIPCHI2_B_HIGH'                : '16',   # dimensionless  
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
  'PT_B_CHILD_BEST'               : '2000', # MeV
  'P_B_CHILD_BEST'                :'10000', # MeV
  'IPCHI2_B_CHILD_BEST'           : '16',   # dimensionless
  'PT_B_TAU_CHILD_BEST'           : '4000', # MeV
  'IPCHI2_B_TAU_CHILD_BEST'       : '150',   # dimensionless
  'IPCHI2_B_TAU_CHILD_WORSE'       : '16',   # dimensionless
  'PT_B_PIONS_TOTAL'              :  '7000',# MeV
  'B_TAUPI_2NDMINIPS'             :  '20',  # dimensionless
  'HltFilter'               : "HLT_PASS_RE('Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision')"
}

__all__ = ('ZVTOP_Conf')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsKaons, StdNoPIDsPions
from StandardParticles import StdLoosePions, StdTightPions
from StandardParticles import StdLooseKaons 
from Configurables import TopoTauAlg,TopoVertexTool

default_name = "ZVTOP"
HLT_DECISIONS_HAD   = "Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision"
HLT1_DECISIONS_TIS  = "Hlt1TrackAllL0Decision"
HLT2_DECISIONS_TIS  = "Hlt2(Topo2BodyBBDT|Topo3BodyBBDT|Topo4BodyBBDT).*Decision"

class ZVTOP_Conf(LineBuilder) :
  
  """
    Builder for ZVTOP
    """
  
  TauTau_Line    = None
  High_Line    = None
  
  __configuration_keys__ = ('B2TauTau_LinePrescale',
                            'B2TauTau_LinePostscale',
                            'High_LinePrescale',
                            'High_LinePostscale',
                            'PT_HAD_ALL_FINAL_STATE',
                            'PTMAX_HAD_ALL_FINAL_STATE',
                            'P_HAD_ALL_FINAL_STATE',
                            'IPCHI2_HAD_ALL_FINAL_STATE',
                            'TRACKCHI2_HAD_ALL_FINAL_STATE',
                            'PID_HAD_ALL_FINAL_STATE',
                            'TRGHOPROB_HAD_ALL_FINAL_STATE',
                            #   
                            'PT_B_TT',
                            'PT_B_TT_HIGH',
                            'PT_B_TM',
                            'PT_B_TM_HIGH',
                            'VCHI2_B' ,
                            'FDCHI2_B',
                            'FD_B',
                            'DIRA_B',
                            'MASS_LOW_B',
                            'MASS_HIGH_B',
                            'MCOR_LOW_B',
                            'MCOR_HIGH_B',
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
                            'PT_B_CHILD_BEST',
                            'P_B_CHILD_BEST',
                            'IPCHI2_B_CHILD_BEST',   
                            'PT_B_TAU_CHILD_BEST', 
                            'IPCHI2_B_TAU_CHILD_BEST',
                            'IPCHI2_B_TAU_CHILD_WORSE',
                            'PT_B_PIONS_TOTAL',
                            'B_TAUPI_2NDMINIPS',
                            'HltFilter'          
                            )
  
  def __init__(self, name, config):
    LineBuilder.__init__(self, name, config)
    
    
    selPions        = self._makePions(      name    = "PionsFor"+name, 
                                            config  = config)
    
    rawPions =     DataOnDemand("Phys/StdLoosePions/Particles")
    
    #
    incTauVertices = self._makeTauVertices(name=name+"_IncTopoVtxFor",
                                           pionInput   = selPions,
                                           config = config)
    
    #
    selTau          = self._filterTau(name       = name+"_TauFilter",
                                      tauInput   = incTauVertices,
                                      config     = config)
    
    #
    selHigh          = self._filterHigh(name       = name+"_HighFilter",
                                        tauInput   = incTauVertices,
                                        config     = config)
    
    #
    selB2TauTau     = self._makeB2TauTau(   name    = name,
                                            tauSel  = selTau,
                                            config  = config)
    #
    self.TauTau_Line    = StrippingLine(name+"_TauTau_Line",
                                        HLT = config['HltFilter'],
                                        #    self.TauTau_Line    = StrippingLine(name+"_TauTau_Line",
                                        prescale    = config['B2TauTau_LinePrescale'],
                                        postscale   = config['B2TauTau_LinePostscale'],
                                        #                                           selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
                                        selection   = selB2TauTau
                                        )
    
    self.High_Line    = StrippingLine(name+"_High_Line",
                                      #HLT         = " HLT_PASS_RE('"+HLT_DECISIONS+"') ",
                                      prescale    = config['High_LinePrescale'],
                                      postscale   = config['High_LinePostscale'],
                                      #                                           selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
                                      selection   = selHigh
                                      )
    
    #
    self.registerLine( self.TauTau_Line )
    self.registerLine( self.High_Line )
    
  #####################################################
  def _makePions(self, name, config) :
    """
    Pion selection
    """
    _code = "(MIPCHI2DV(PRIMARY) >25) & (TRGHOSTPROB < 0.3) "
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdLoosePions ] )
  
  #####################################################
  def _filterHigh(self, name, tauInput, config) :
    """
    Tau filter
    """
    
    _cut = "(M > 50000) & (BPVVDZ > 1.0*mm)"
    
    
    #    _filter   = FilterDesktop( Code = _cut, Preambulo = _preambulo)
    _filter   = FilterDesktop( Code = _cut)
    
    return Selection(name,
                     Algorithm = _filter,
                     RequiredSelections = [tauInput]
                     )
  
  #####################################################
  def _filterTau(self, name, tauInput, config) :
    """
    Tau filter
    """
    #    _cut =       "(PT>1000.*MeV) & (M>500.*MeV) & (M<2000.*MeV) & (BPVDIRA>0.99) & (VFASPF(VCHI2) < 16) & (BPVVDCHI2>16) & (BPVVDRHO>0.1*mm) & (BPVVDRHO<7.0*mm) & (BPVVDZ>5.0*mm)"#" & (AMAXDOCA('')<0.2*mm) & (ANUM(PT > 800*MeV))
    
    _cut = " (BPVVDZ>5.0*mm) &  (BPVDIRA>0.99)"
    
    _preambulo = [ "c1c2c3 = ('pi+') == ABSID" ,
                   "ipsm    = MINTREE( c1c2c3 , MIPCHI2DV(PRIMARY) )"]
    
    _filter   = FilterDesktop( Code = _cut, Preambulo = _preambulo)
    #    _filter   = FilterDesktop( Code = _cut)
    
    return Selection(name,
                     Algorithm = _filter,
                     RequiredSelections = [tauInput]
                     )
  
  #####################################################
  def _makeTauVertices(self, name, pionInput, config) :
    """
    Run IncTopovertices search
    """
    _localTopoTauAlg = TopoTauAlg('TopoTauFor'+name)
    _localTopoTauAlg.addTool(TopoVertexTool)
    _localTopoTauAlg.TopoVertexTool.VertexFunctionToolName = "VertexFunctionToolRootMini"
    _localTopoTauAlg.TopoVertexTool.VertexFunctionToolType = "VertexFunctionToolRootMini"
    _localTopoTauAlg.TopoVertexTool.TwoTracksVtxChi2Max        = 10.
    _localTopoTauAlg.TopoVertexTool.TwoTracksVtxVfMin =  0.1
    _localTopoTauAlg.TopoVertexTool.TwoTracksVtxVfRatioMin =  0.8
    _localTopoTauAlg.TopoVertexTool.TrackVtxChi2Max            = 20.
    _localTopoTauAlg.TopoVertexTool.MaxFinderStep              = 0.01
    _localTopoTauAlg.TopoVertexTool.MaxFinderMinGradientMag    = 0.1
    _localTopoTauAlg.TopoVertexTool.MaxFinderMaxIteration      = 8000
    _localTopoTauAlg.TopoVertexTool.MaxFinderMinStep           = 0.01
    _localTopoTauAlg.TopoVertexTool.MaxFinderMaxjump           = 5
    _localTopoTauAlg.TopoVertexTool.ResolverCut                = 0.35
    _localTopoTauAlg.TopoVertexTool.ResolverMinStep            = 0.001
    _localTopoTauAlg.TopoVertexTool.ResolverMaxIteration       = 8
##     _localTopoTauAlg.TopoVertexTool.VertexFunctionToolName = "VertexFunctionToolStarMax"
##     _localTopoTauAlg.TopoVertexTool.VertexFunctionToolType = "VertexFunctionToolStarMax"
##     _localTopoTauAlg.TopoVertexTool.TwoTracksVtxChi2Max        = 1.5
##     _localTopoTauAlg.TopoVertexTool.TwoTracksVtxVfMin =  0.4
##     _localTopoTauAlg.TopoVertexTool.TwoTracksVtxVfRatioMin =  0.7
##     _localTopoTauAlg.TopoVertexTool.TrackVtxChi2Max            = 12.
##     _localTopoTauAlg.TopoVertexTool.MaxFinderStep              = 0.01
##     _localTopoTauAlg.TopoVertexTool.MaxFinderMinGradientMag    = 0.1
##     _localTopoTauAlg.TopoVertexTool.MaxFinderMaxIteration      = 8000
##     _localTopoTauAlg.TopoVertexTool.MaxFinderMinStep           = 0.01
##     _localTopoTauAlg.TopoVertexTool.MaxFinderMaxjump           = 5
##     _localTopoTauAlg.TopoVertexTool.ResolverCut                = 0.4
##     _localTopoTauAlg.TopoVertexTool.ResolverMinStep            = 0.001
##     _localTopoTauAlg.TopoVertexTool.ResolverMaxIteration       = 8
    
    _localTopoTauAlg.VFParticlesInputLocation = "Phys/StdAllNoPIDsPions/Particles"
    _localTopoTauAlg.TopoVertexTool.DistanceCalculatorToolType = "LoKi::DistanceCalculator"
    _localTopoTauAlg.TopoVertexTool.DistanceCalculatorToolName = "DistanceCalculatorTool"
    _localTopoTauAlg.TopoVertexTool.TrackVertexerToolType      = "TrackVertexer"
    _localTopoTauAlg.TopoVertexTool.TrackVertexerToolName      = "TrackVertexerTool"
#    _localTopoTauAlg.cut_ips = 40.    
#    _localTopoTauAlg.cut_ips_VF = 20.      
#    _localTopoTauAlg.cut_ghost = 0.3
    _localTopoTauAlg.cut_ntrk = 100
    
    return Selection(name,
                     Algorithm          = _localTopoTauAlg,
                     RequiredSelections = [ pionInput ] )
  
  
  #####################################################   
  def _makeB2TauTau(self, name, tauSel, config):
    
    preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                 "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS",
                 "allpi = ('pi+') == ABSID ",
                 "sumpt    = SUMTREE( allpi , PT )"]
    
    _combcut = "(APT > " + config['PT_B_TT']       + "*MeV) & "\
               "(AM  > " + config['MASS_LOW_B']    + "*MeV) & "\
               "(AM  < " + config['MASS_HIGH_B']   + "*MeV)"
    #    _bcut    = "(PT  > 0)  "              
    
    _bcut    = "(VFASPF(VCHI2PDOF)  <   "   + config['VCHI2_B']       + ") & "\
               "(BPVVDCHI2          >   "   + config['FDCHI2_B']      + ") & "\
               "(BPVVD          <   "   + config['FD_B']      + ") & "\
               "(PT                >   "   + config['PT_B_TT_HIGH']  + "*MeV) & "\
               "(in_range("+config['MCOR_LOW_B']+"*MeV,MCOR,"+config['MCOR_HIGH_B']+"*MeV))"    

    """
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
    """    
    _CombineTau = CombineParticles( DecayDescriptors = ["B0 -> tau+ tau-"],
                                    CombinationCut   = _combcut,
                                    MotherCut        = _bcut,
                                    Preambulo        = preambulo)
    
    return Selection(name+"_TauTau",
                     Algorithm          = _CombineTau,
                     RequiredSelections = [ tauSel ] )
  
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
    tisTosFilter = self._makeTISTOSFilter(name,{HLT_DECISIONS_HAD+'%TOS':0})
    return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])
  
  def _makeTIS(self, name, sel):
    ''' TIS filters selections'''
    tisTosFilter = self._makeTISTOSFilter(name,
                                          {HLT1_DECISIONS_TIS+'%TIS':0,HLT2_DECISIONS_TIS+'%TIS':0})
    return Selection(name, Algorithm=tisTosFilter, RequiredSelections=[sel])
  
  
