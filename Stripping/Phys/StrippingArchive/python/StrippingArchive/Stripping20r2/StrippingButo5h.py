'''
Module for construction of Bu->5h from:   
   Exclusive 5pi line
   Exclusive K4pi line

Exported symbols (use python help!):
   - Buto5hBuilder
   - make5pi_excl  
   - makeK4pi_excl
'''

__author__ = ['Luiz Gustavo de Oliveira', 'Alvaro Gomes', 'Bruno Souza de Paula']
__date__ = '27/Mar/2013'
__version__ = '$Revision: 1.0 $'

__all__ = ('Buto5hBuilder',
           'make5pi_excl', 
           'makeK4pi_excl', 
	   )

# Selecting Cuts 

config_params = {
    'MaxTrSIZE'             : 120 ,    
    '_h_PT'                 : 250. ,
    '_h_IPCHI2'             : 6. ,     
    '_h_TRCHI2DOF'          : 1.7 ,    
    '_h_TRGHP'              : 0.2 , 
    '_5h_DOCA'              : .14 ,    
    '_5h_DIRA'              : .99999 , 
    '_5h_FDCHI2'            : 500. ,   
    '_5h_CHI2'              : 14. ,
    '_5h_PT'                : 1000. ,
    '_5h_PVIPCHI2sum'       : 400. ,
    '_5h_Mmax'              : 5679. ,
    '_5h_Mmin'              : 5079. ,  
    '5pi_exclLinePrescale'  : 1.0,
    '5pi_exclLinePostscale' : 1.0,
    'K4pi_exclLinePrescale' : 1.0,
    'K4pi_exclLinePostscale': 1.0
    }


"""
B+ -> h+ h+ h+ h- h- channels
"""

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles

import StandardParticles

# Selecting Std All No PIDs containers
from StandardParticles import StdAllNoPIDsKaons as StdNoPIDsKaons
from StandardParticles import StdAllNoPIDsPions as StdNoPIDsPions

from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter as VoidFilter

default_name = "Buto5h"

class Buto5hBuilder(LineBuilder) :
    __configuration_keys__ = ('MaxTrSIZE',
			      '_h_PT',             
			      '_h_IPCHI2',         
			      '_h_TRCHI2DOF',       
                              '_h_TRGHP',
			      '_5h_DOCA',        
			      '_5h_DIRA',        
			      '_5h_FDCHI2',      
			      '_5h_CHI2',        
                              '_5h_PT',
			      '_5h_PVIPCHI2sum', 
			      '_5h_Mmax',         
			      '_5h_Mmin',     
			      '5pi_exclLinePrescale',  
			      '5pi_exclLinePostscale', 
			      'K4pi_exclLinePrescale', 
			      'K4pi_exclLinePostscale'
)
    def __init__(self, name, config) :

      LineBuilder.__init__(self, name, config)

      _5pi_exclName = name + '_5pi_excl'
      _K4pi_exclName = name + '_K4pi_excl'
      
				    

      self.sel5pi = make5pi_excl( '5piexclFor' + _5pi_exclName + 'Sel', 
			   _h_PT           = config['_h_PT'],
			   _h_IPCHI2       = config['_h_IPCHI2'],
			   _h_TRCHI2DOF    = config['_h_TRCHI2DOF'], 
                           _h_TRGHP        = config['_h_TRGHP'],
			   _5h_DOCA	 = config['_5h_DOCA'],        
			   _5h_DIRA	 = config['_5h_DIRA'],        
			   _5h_FDCHI2	 = config['_5h_FDCHI2'],      
			   _5h_CHI2        = config['_5h_CHI2'], 
                           _5h_PT        = config['_5h_PT'],
			   _5h_PVIPCHI2sum = config['_5h_PVIPCHI2sum'], 
			   _5h_Mmax     = config['_5h_Mmax'],         
			   _5h_Mmin     = config['_5h_Mmin'])     

      self.selK4pi = makeK4pi_excl( 'K4piexclFor' + _K4pi_exclName + 'Sel', 
			   _h_PT           = config['_h_PT'],
			   _h_IPCHI2       = config['_h_IPCHI2'],
			   _h_TRCHI2DOF    = config['_h_TRCHI2DOF'], 
                           _h_TRGHP        = config['_h_TRGHP'],
			   _5h_DOCA	 = config['_5h_DOCA'],        
			   _5h_DIRA	 = config['_5h_DIRA'],        
			   _5h_FDCHI2	 = config['_5h_FDCHI2'],      
			   _5h_CHI2        = config['_5h_CHI2'], 
                           _5h_PT        = config['_5h_PT'],
			   _5h_PVIPCHI2sum = config['_5h_PVIPCHI2sum'], 
			   _5h_Mmax    = config['_5h_Mmax'],         
			   _5h_Mmin    = config['_5h_Mmin'] )    

      self.gECFilter = globalEventCutFilter(name + 'GlobalEVventCutFilter', MaxTrSIZE = config['MaxTrSIZE'])
      
      self.algos5pi = []
      if self.gECFilter != None  : self.algos5pi.append(self.gECFilter)

      self.algos5pi.append(self.sel5pi)
      
      self.algosK4pi = []
      if self.gECFilter != None : self.algosK4pi.append(self.gECFilter)

      self.algosK4pi.append(self.selK4pi)	

      self.line5pi_excl = StrippingLine( _5pi_exclName + 'Line',
                                      prescale   = config['5pi_exclLinePrescale'],
                                      postscale  = config['5pi_exclLinePostscale'],
                                      algos      = self.algos5pi)
   
     
      self.lineK4pi_excl = StrippingLine( _K4pi_exclName + 'Line',
                                      prescale   = config['K4pi_exclLinePrescale'],
                                      postscale  = config['K4pi_exclLinePostscale'],
                                      algos      = self.algosK4pi)

      self.registerLine(self.line5pi_excl)
      self.registerLine(self.lineK4pi_excl)

def make5pi_excl(name,
           _h_PT,
           _h_IPCHI2,
           _h_TRCHI2DOF,
           _h_TRGHP,
           _5h_DOCA,
           _5h_DIRA,
           _5h_FDCHI2,
           _5h_CHI2,
           _5h_PT,
           _5h_PVIPCHI2sum,
           _5h_Mmin,
           _5h_Mmax) :

    _daughtersCuts = {"pi+": " (PT > %(_h_PT)s*MeV)  \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRGHP < %(_h_TRGHP)s) " % locals()}
    _combinationCut = "(AM < %(_5h_Mmax)s*MeV) \
                     & (AM > %(_5h_Mmin)s*MeV) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_5h_DOCA)s)" % locals()
    _motherCut = " (BPVDIRA > %(_5h_DIRA)s) \
		 & (BPVVDCHI2 > %(_5h_FDCHI2)s) \
		 & (VFASPF(VCHI2) < %(_5h_CHI2)s) \
                 & (PT > %(_5h_PT)s*MeV) \
		 & (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='pi+') | (ABSID=='pi-')),0.0) > %(_5h_PVIPCHI2sum)s) " \
                 % locals()

    _5pi=CombineParticles()
    _5pi.DecayDescriptors  = ["[B+ -> pi+ pi+ pi+ pi- pi-]cc"]
    _5pi.MotherCut         = _motherCut
    _5pi.CombinationCut    = _combinationCut
    _5pi.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _5pi,
                       RequiredSelections = [StdNoPIDsPions])

def makeK4pi_excl(name,
           _h_PT,
           _h_IPCHI2,
           _h_TRCHI2DOF,
           _h_TRGHP,
           _5h_DOCA,
           _5h_DIRA,
           _5h_FDCHI2,
           _5h_CHI2,
           _5h_PT,
           _5h_PVIPCHI2sum,
           _5h_Mmin,
           _5h_Mmax) :

    _daughtersCuts = {"pi+" : " (PT > %(_h_PT)s*MeV)  \
                              & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                              & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                              & (TRGHP < %(_h_TRGHP)s) " % locals(),
                      "K+"  : " (PT > %(_h_PT)s*MeV)  \
                              & (TRCHI2DOF < %(_h_TRCHI2DOF)s) \
                              & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                              & (TRGHP < %(_h_TRGHP)s) " % locals()}
    _combinationCut = " (AM < %(_5h_Mmax)s*MeV) \
                      & (AM > %(_5h_Mmin)s*MeV) \
		      & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_5h_DOCA)s)" % locals()
    _motherCut = " (BPVDIRA > %(_5h_DIRA)s) \
		 & (BPVVDCHI2 > %(_5h_FDCHI2)s) \
		 & (VFASPF(VCHI2) < %(_5h_CHI2)s) \
                 & (PT > %(_5h_PT)s*MeV) \
		 & (SUMTREE(MIPCHI2DV(PRIMARY),((ABSID=='pi+') | (ABSID=='pi-') | (ABSID=='K+') | (ABSID=='K-')),0.0) > %(_5h_PVIPCHI2sum)s) " \
                 % locals()
    
    _K4pi=CombineParticles()
    _K4pi.DecayDescriptors  = ["[B+ -> K+ pi+ pi+ pi- pi-]cc"]
    _K4pi.MotherCut         = _motherCut
    _K4pi.CombinationCut    = _combinationCut
    _K4pi.DaughtersCuts     = _daughtersCuts

    return Selection ( name,
                       Algorithm = _K4pi,
                       RequiredSelections = [StdNoPIDsPions, StdNoPIDsKaons])

def globalEventCutFilter(name, 
	                 MaxTrSIZE = None 
	                 ) :
  
  if MaxTrSIZE == None : return None
  
  _code = ""
  from Configurables import LoKi__VoidFilter as VoidFilter
  from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
  modules = CoreFactory('CoreFactory').Modules
  for i in ['LoKiTracks.decorators']:
     if i not in modules : modules.append(i)
  if MaxTrSIZE != None : _code += "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(MaxTrSIZE)s )" %locals()

  globalFilter= VoidFilter(name)
  globalFilter.Code = _code
  
  return globalFilter
