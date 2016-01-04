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
  'B2TauTauSS_LinePrescale'      : 1,
  'B2TauTauSS_LinePostscale'     : 1,
  'High_LinePrescale'          : 1,
  'High_LinePostscale'         : 1,
  'PT_HAD_ALL_FINAL_STATE'        : '1200',  # MeV
  'PTMAX_HAD_ALL_FINAL_STATE'     : '500',#MeV
  'P_HAD_ALL_FINAL_STATE'         : '6000', # MeV
  'IPCHI2_HAD_ALL_FINAL_STATE'    : '40',    # dimensionless
  'TRACKCHI2_HAD_ALL_FINAL_STATE' : '4',    # dimensionless
  'PID_HAD_ALL_FINAL_STATE'       : '5',    # dimensionless
  'TRGHOPROB_HAD_ALL_FINAL_STATE' : '0.4',    # dimensionless
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
  #
  'RelatedInfoTools'      : [
  #1
  { "Type" : "RelInfoBstautauMuonIsolationBDT"
    , "Variables" : ['BSTAUTAUMUONISOBDTFIRSTVALUE', 'BSTAUTAUMUONISOBDTSECONDVALUE','BSTAUTAUMUONISOBDTTHIRDVALUE']
     , "Location"  : "MuonIsolationBDT"  
    },
  #2
  { "Type" : "RelInfoBstautauMuonIsolation"
    , "Variables" : ['BSTAUTAUMUONISOFIRSTVALUE', 'BSTAUTAUMUONISOSECONDVALUE']
     , "Location"  : "MuonIsolation"  
    },
  #3
  { "Type" : "RelInfoBstautauTauIsolationBDT"
    , "Variables" : ['BSTAUTAUTAUISOBDTFIRSTVALUETAUP', 'BSTAUTAUTAUISOBDTSECONDVALUETAUP','BSTAUTAUTAUISOBDTTHIRDVALUETAUP','BSTAUTAUTAUISOBDTFIRSTVALUETAUM', 'BSTAUTAUTAUISOBDTSECONDVALUETAUM','BSTAUTAUTAUISOBDTTHIRDVALUETAUM']
     , "Location"  : "TauIsolationBDT"  
    },
  #4
  { "Type" : "RelInfoBstautauTauIsolation"
    , "Variables" : ['BSTAUTAUTAUISOFIRSTVALUETAUP', 'BSTAUTAUTAUISOSECONDVALUETAUP','BSTAUTAUTAUISOFIRSTVALUETAUM', 'BSTAUTAUTAUISOSECONDVALUETAUM']
     , "Location"  : "TauIsolation"  
    },
  #5

 { "Type" : "RelInfoBstautauTrackIsolationBDT" 
    , "Variables" : ['BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIM', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIM','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIM','BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP1', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP1','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP1','BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP2', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP2','BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP2','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIP','BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIP','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIP','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM1','BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM1','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM1','BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM2', 'BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM2','BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM2']
     , "Location"  : "TrackIsolationBDT"  
    },

  #6
  { "Type" : "RelInfoBstautauTrackIsolation" 
    , "Variables" : ['BSTAUTAUTRACKISOFIRSTVALUETAUPPIM', 'BSTAUTAUTRACKISOFIRSTVALUETAUPPIP1','BSTAUTAUTRACKISOFIRSTVALUETAUPPIP2', 'BSTAUTAUTRACKISOFIRSTVALUETAUMPIP','BSTAUTAUTRACKISOFIRSTVALUETAUMPIM1', 'BSTAUTAUTRACKISOFIRSTVALUETAUMPIM2']
     , "Location"  : "TrackIsolation"  
    },
  #7
  { "Type" : "RelInfoBstautauCDFIso" 
    #, "Variables" : ['BSTAUTAUCDFISO']
     , "Location"  : "CDFIso"  
    },
  #8
  { "Type" : "RelInfoBstautauZVisoBDT" 
    , "Variables" : ['ZVISOTAUP','ZVISOTAUM']
    , "Location"  : "ZVisoBDT"
    }
  ]
}

__all__ = ('ZVTOP_Conf',
           'default_config')

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

default_config = {'NAME'       : 'ZVTOP',
                  'BUILDERTYPE': 'ZVTOP_Conf',
                  'CONFIG'     : config_params,
                  'WGs'    : [ 'RD' ],
                  'STREAMS'     : ['Bhadron']
                  }

class ZVTOP_Conf(LineBuilder) :
  
  """
    Builder for ZVTOP
    """
  
  TauTau_Line    = None
  High_Line    = None
  
  __configuration_keys__ = ('B2TauTau_LinePrescale',
                            'B2TauTau_LinePostscale',
                            'B2TauTauSS_LinePrescale',
                            'B2TauTauSS_LinePostscale',
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
                            #
                            'RelatedInfoTools'
                            )
  
  def __init__(self, name, config):
    LineBuilder.__init__(self, name, config)
    
    
    self.selPions        = self._makePions(      name    = "PionsFor"+name, 
                                            config  = config)
    
    self.selPions_high        = self._makePions_high(      name    = "PionsHighFor"+name, 
                                            config  = config)
    
    self.rawPions =     DataOnDemand("Phys/StdLoosePions/Particles")
    
    #
    self.incTauVertices = self._makeTauVertices(name=name+"_IncTopoVtxFor",
                                           pionInput   = self.selPions,
                                           config = config)
    
    self.incTauVertices_high = self._makeTauVertices(name=name+"_IncTopoVtxHighFor",
                                           pionInput   = self.selPions_high,
                                           config = config)
    
    #
    self.selTau          = self._filterTau(name       = name+"_TauFilter",
                                      tauInput   = self.incTauVertices,
                                      config     = config)
    
    #
    self.selHigh          = self._filterHigh(name       = name+"_HighFilter",
                                        tauInput   = self.incTauVertices_high,
                                        config     = config)
    
    #
    self.selB2TauTau,self.selB2TauTauSS     = self._makeB2TauTau(   name    = name,
                                                          tauSel  = self.selTau,
                                                          config  = config)
    #
    self.TauTau_Line    = StrippingLine(name+"_TauTau_Line",
                                        #    self.TauTau_Line    = StrippingLine(name+"_TauTau_Line",
                                        prescale    = config['B2TauTau_LinePrescale'],
                                        postscale   = config['B2TauTau_LinePostscale'],
                                        #                                           selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
                                        MDSTFlag = True,
                                        RelatedInfoTools = [
                                             { "Type" : "RelInfoBstautauTauIsolationBDT", "Location"  : "TauIsolationBDT" },
                                             { "Type" : "RelInfoBstautauTauIsolation",  "Location"  : "TauIsolation"  },
                                             { "Type" : "RelInfoBstautauTrackIsolationBDT" ,  "Location"  : "TrackIsolationBDT" },
                                             { "Type" : "RelInfoBstautauTrackIsolation" , "Location"  : "TrackIsolation"},
                                             { "Type" : "RelInfoBstautauCDFIso" , "Location"  : "CDFIso"  },
                                             { "Type" : "RelInfoBstautauZVisoBDT" , "Location"  : "ZVisoBDT"},
                                             { "Type" : "RelInfoVertexIsolation", "Location" : "BVars_VertexIsoInfo",
                                                 "DaughterLocations" : {
                                                 "[B0 -> ^tau+ tau-]CC" : "TauVars_VertexIsoInfo_0",
                                                 "[B0 -> tau+ ^tau-]CC" : "TauVars_VertexIsoInfo_1"
                                               }
                                             }
                                            ],
                                        selection   = self.selB2TauTau
                                        )

    self.TauTauSS_Line  = StrippingLine(name+"_TauTauSS_Line",
                                        #    self.TauTau_Line    = StrippingLine(name+"_TauTau_Line",
                                        prescale    = config['B2TauTauSS_LinePrescale'],
                                        postscale   = config['B2TauTauSS_LinePostscale'],
                                        #                                           selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
                                        MDSTFlag = True,
                                        RelatedInfoTools = [
                                             { "Type" : "RelInfoBstautauTauIsolationBDT", "Location"  : "TauIsolationBDT" },
                                             { "Type" : "RelInfoBstautauTauIsolation",  "Location"  : "TauIsolation"  },
                                             { "Type" : "RelInfoBstautauTrackIsolationBDT" ,  "Location"  : "TrackIsolationBDT" },
                                             { "Type" : "RelInfoBstautauTrackIsolation" , "Location"  : "TrackIsolation"},
                                             { "Type" : "RelInfoBstautauCDFIso" , "Location"  : "CDFIso"  },
                                             { "Type" : "RelInfoBstautauZVisoBDT" , "Location"  : "ZVisoBDT"},
                                             { "Type" : "RelInfoVertexIsolation", "Location" : "BVars_VertexIsoInfo",
                                                 "DaughterLocations" : {
                                                 "[B0 -> ^tau+ tau+]CC" : "TauVars_VertexIsoInfo_0",
                                                 "[B0 -> tau+ ^tau+]CC" : "TauVars_VertexIsoInfo_1"
                                               }
                                             }
                                            ],
                                        selection   = self.selB2TauTauSS
                                        ) 
    
    self.High_Line    = StrippingLine(name+"_High_Line",
                                      prescale    = config['High_LinePrescale'],
                                      postscale   = config['High_LinePostscale'],
                                      #                                           selection   = self._makeTOS(name+"_TOSForTauTau",selB2TauTau)
                                      MDSTFlag = True,
                                        RelatedInfoTools = [
                                             { "Type" : "RelInfoBstautauTauIsolationBDT", "Location"  : "TauIsolationBDT" },
                                             { "Type" : "RelInfoBstautauTauIsolation",  "Location"  : "TauIsolation"  },
                                             { "Type" : "RelInfoBstautauTrackIsolationBDT" ,  "Location"  : "TrackIsolationBDT" },
                                             { "Type" : "RelInfoBstautauTrackIsolation" , "Location"  : "TrackIsolation"},
                                             { "Type" : "RelInfoBstautauCDFIso" , "Location"  : "CDFIso"  },
                                             { "Type" : "RelInfoBstautauZVisoBDT" , "Location"  : "ZVisoBDT"},
                                             { "Type" : "RelInfoVertexIsolation", "Location" :  "Vars_VertexIsoInfo"}
                                            ],
                                      selection   = self.selHigh
                                      )
    
    #
    self.registerLine( self.TauTau_Line )
    self.registerLine( self.TauTauSS_Line )
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
  def _makePions_high(self, name, config) :
    """
    Pion selection
    """
    _code = "(MIPCHI2DV(PRIMARY) >25) & (TRGHOSTPROB < 0.3) "
    
    _Filter = FilterDesktop(Code = _code)
    
    return Selection(name,
                     Algorithm          = _Filter,
                     RequiredSelections = [ StdNoPIDsPions ] )
  
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
    _CombineTauOS = CombineParticles( DecayDescriptors = ["B0 -> tau+ tau-"],
                                      CombinationCut   = _combcut,
                                      MotherCut        = _bcut,
                                      Preambulo        = preambulo)
    _CombineTauSS = CombineParticles( DecayDescriptors = ["[B0 -> tau+ tau+]cc"],
                                      CombinationCut   = _combcut,
                                      MotherCut        = _bcut,
                                      Preambulo        = preambulo)
    
    selTauTauOS = Selection(name+"_TauTau",
                            Algorithm          = _CombineTauOS,
                            RequiredSelections = [ tauSel ] )
    
    selTauTauSS = Selection(name+"_TauTauSS",
                            Algorithm          = _CombineTauSS,
                            RequiredSelections = [ tauSel ] )

    return selTauTauOS,selTauTauSS
  
