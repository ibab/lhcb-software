 # $Id: StrippingBu3hFrom2h.py,v 2.0 2011-02-15 samba team $
'''
Module for construction of Bu->hhh from:   
   two-body (allowing an extra low momentum third hadron afterwards)
   three-body

Exported symbols (use python help!):
   - Bu3hFrom2hBuilder
   - make2hLoose  
   - make2h
   - make3h

Parameters:

    	'MaxTrSIZE':  None,         ## GEC  maximim Rec/Track/Best TrSIZE
        '_h_PT': 700. ,             ## tracks min PT
        '_h_P': 5000. ,             ## tracks min P  
        '_h_IPCHI2': 16. ,          ## min tracks PI wrt OWNPV
        '_h_TRCHI2DOF': 3.0 ,       ## max tracks CHI2DOF
        '_2hLoose_DOCA': .12 ,      ## max two track DOCA
        '_2h_PTmax': 2000. ,        ## min PT of the 2h higests PT track 
        '_2h_PTsum':4000.  ,        ## min of 2h tracks PT sum 
        '_2h_PVIPCHI2sum': 100. ,   ## min of the 2h tracks IP wrt best 2h PV
        '_2h_FDCHI2': 150. ,        ## min 2h FDCHI2 wrt best 2h PV 
        '_2h_DIRA': 0.99 ,          ## min cos angle between 2h momentum and PV decayV  direction
        '_2h_CORRMmax': 6000. ,     ## max corrected mass for 2h candidate
        '_2h_CORRMmin': 4000. ,     ## min corrected mass for 2h candidate 
        '_2h_CharmVetoIPCHI2': 16. ,## charm veto: (Mhh>_2h_CharmVetoM||IP>_2h_CharmVetoIPCHI2)
        '_2h_CharmVetoM': 2500. ,   ##
	'_2h_TRKCHIDOFmin': 3. ,    ## max tracks CHI2DOF for the track with smalest CHI2DOF
        '_2h_Mmin': 3500. ,         ## min 2h mass
        '_2h_PVDOCAmin': 1.0 ,      ## min value of the 2h doca wrt any PV
        '_2h_CHI2': 14.0 ,          ## max 2h vertex CHI2
        '_2h_PTRANS': 4000. ,       ## 2h transverse mom wrt the PV decayV  direction
        '_3h_DOCA': .14 ,           ## max DOCA between h and 2h 
        '_3h_PTmax': 2000 ,         ## min PT of the 3h higests PT track
        '_3h_DIRA': .9998 ,         ## min cos angle between 3h momentum and PV decayV  direction   
        '_3h_FDCHI2': 150. ,        ## min 3h FDCHI2 wrt best 3h PV  
        '_3h_PVDOCAmin': 1.0 ,      ## min value of the 3h doca wrt any PV
        '_3h_CHI2': 14.0 ,          ## max 3h vertex CHI2 
        '_3h_IP': 0.1 ,             ## max 3h IP wrt best 3h PV
        '_3h_PT': 1000. ,           ## min 3h PT   
        '_3h_PTsum':4250.  ,        ## min of 3h tracks PT sum 
        '_3h_PVIPCHI2sum': 150. ,   ## min of the 3h tracks IP wrt best 3h PV
        '_3h_Charge': 1 ,           ## 3h tracks charge sum ==+-1
        '_3h_CORRMmax': 7000. ,     ## max corrected mass for 3h candidate  
        '_3h_CORRMmin': 4000. ,     ## min corrected mass for 3h candidate   
        '_3h_Mmax': 6000. ,         ## max 3h mass        
        '_3h_Mmin': 4000. ,         ## min 3h mass     
        '2hLinePrescale': 1. ,
        '2hLinePostscale': 1 ,
        '3hLinePrescale': 1 ,
        '3hLinePostscale': 1}

'''

__author__ = ['Jussara Miranda, Juan Otalora and Andre Massafferri']
__date__ = '15/02/2011'
__version__ = '$Revision: 1.0 $'

__all__ = ('Bu3hFrom2hBuilder',
           'make2hLoose', 
           'make2h', 
           'make3h')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StandardParticles                     import StdLoosePions

from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from Configurables import LoKi__VoidFilter as VoidFilter

default_name = "Bu3hFrom2hBuilder"

class Bu3hFrom2hBuilder(LineBuilder) :
    __configuration_keys__ = ('MaxTrSIZE',
                              '_h_PT',             
                              '_h_P',               
                              '_h_IPCHI2',         
                              '_h_TRCHI2DOF',       
                              '_2hLoose_DOCA',      
                              '_2h_PTmax',         
                              '_2h_PTsum',         
                              '_2h_PVIPCHI2sum',    
                              '_2h_FDCHI2',         
                              '_2h_DIRA',           
                              '_2h_CORRMmax',       
                              '_2h_CORRMmin',       
                              '_2h_CharmVetoIPCHI2',
                              '_2h_CharmVetoM',
			      '_2h_TRKCHIDOFmin',
                              '_2h_Mmin', 
                              '_2h_PVDOCAmin',
                              '_2h_CHI2',          
                              '_2h_PTRANS',          
                              '_3h_DOCA',            
                              '_3h_PTmax',          
                              '_3h_DIRA',            
                              '_3h_FDCHI2',         
                              '_3h_PVDOCAmin',      
                              '_3h_CHI2',           
                              '_3h_IP',             
                              '_3h_PT',              
                              '_3h_PTsum',              
                              '_3h_PVIPCHI2sum',              
                              '_3h_Charge',         
                              '_3h_CORRMmax',       
                              '_3h_CORRMmin',       
                              '_3h_Mmax',                 
                              '_3h_Mmin',             
                              '2hLinePrescale',
                              '2hLinePostscale',
                              '3hLinePrescale',
                              '3hLinePostscale')

    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        _2hName = name + '2h'
        _3hName = name + '3h'
	
	self.sel2hLoose = make2hLoose('2hLooseFor' + _2hName + 'Sel', 
                                      _h_PT         = config['_h_PT'],
                                      _h_P          = config['_h_P'],
                                      _h_IPCHI2     = config['_h_IPCHI2'],
                                      _h_TRCHI2DOF  = config['_h_TRCHI2DOF'], 
                                      _2hLoose_DOCA = config['_2hLoose_DOCA'])
				      
        self.sel2h = make2h('2hFor' + _2hName + 'Sel', 
                             Sel2hLoose = self.sel2hLoose, 
                             _2h_PTmax           = config['_2h_PTmax'],
                             _2h_PTsum           = config['_2h_PTsum'],
                             _2h_PVIPCHI2sum     = config['_2h_PVIPCHI2sum'],
                             _2h_FDCHI2          = config['_2h_FDCHI2'],
                             _2h_DIRA            = config['_2h_DIRA'],
                             _2h_CORRMmax        = config['_2h_CORRMmax'],
                             _2h_CORRMmin        = config['_2h_CORRMmin'],
                             _2h_CharmVetoIPCHI2 = config['_2h_CharmVetoIPCHI2'],
                             _2h_CharmVetoM      = config['_2h_CharmVetoM'],
                             _2h_TRKCHIDOFmin    = config['_2h_TRKCHIDOFmin'],
                             _2h_Mmin            = config['_2h_Mmin'],
                             _2h_PVDOCAmin       = config['_2h_PVDOCAmin'],
                             _2h_CHI2            = config['_2h_CHI2'],       
                             _2h_PTRANS          = config['_2h_PTRANS'])       

	self.sel3h = make3h( '3hFor' + _3hName + 'Sel', 
                             Sel2hLoose = self.sel2hLoose,
                             _h_PT           = config['_h_PT'],
                             _h_P            = config['_h_P'],
                             _h_IPCHI2       = config['_h_IPCHI2'],
                             _h_TRCHI2DOF    = config['_h_TRCHI2DOF'], 
                             _3h_DOCA        = config['_3h_DOCA'],
                             _3h_PTmax       = config['_3h_PTmax'],
                             _3h_DIRA        = config['_3h_DIRA'],
                             _3h_FDCHI2      = config['_3h_FDCHI2'],
                             _3h_PVDOCAmin   = config['_3h_PVDOCAmin'],
                             _3h_CHI2        = config['_3h_CHI2'],
                             _3h_IP          = config['_3h_IP'],
                             _3h_PT          = config['_3h_PT'],
                             _3h_PTsum       = config['_3h_PTsum'],
                             _3h_PVIPCHI2sum = config['_3h_PVIPCHI2sum'],
                             _3h_Charge      = config['_3h_Charge'],
                             _3h_CORRMmax    = config['_3h_CORRMmax'],
                             _3h_CORRMmin    = config['_3h_CORRMmin'],
                             _3h_Mmin        = config['_3h_Mmin'],
                             _3h_Mmax        = config['_3h_Mmax'])

	self.gECFilter = globalEventCutFilter(name + 'GlobalEventCutFilter', MaxTrSIZE = config['MaxTrSIZE'])
	
        self.algos2h = []
        if self.gECFilter != None  : self.algos2h.append(self.gECFilter)

	self.algos2h.append(self.sel2h)
	
        self.algos3h = []
        if self.gECFilter != None : self.algos3h.append(self.gECFilter)

        self.algos3h.append(self.sel3h)	
	
        self.line2h = StrippingLine( _2hName + 'Line',
                                     prescale   = config['2hLinePrescale'],
                                     postscale  = config['2hLinePostscale'],
                                     algos      = self.algos2h)
        
	self.line3h = StrippingLine( _3hName + 'Line',
                                     prescale   = config['3hLinePrescale'],
                                     postscale  = config['3hLinePostscale'],
                                     algos      = self.algos3h)

        self.registerLine(self.line2h)
        self.registerLine(self.line3h)

def make2hLoose(name, 
                _h_PT,
                _h_P,
                _h_IPCHI2,
                _h_TRCHI2DOF,
                _2hLoose_DOCA) :
    _motherCut = "ALL" % locals()
    _combinationCut = "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(_2hLoose_DOCA)s)" % locals()
    _daughtersCuts = {"pi+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s)" % locals()}
    
    
    _2hLoose=CombineParticles()
    _2hLoose.DecayDescriptors  = ["rho(770)0 -> pi+ pi+","rho(770)0 -> pi+ pi-","rho(770)0 -> pi- pi-"]
    _2hLoose.DaughtersCuts     = _daughtersCuts
    _2hLoose.CombinationCut    = _combinationCut
    _2hLoose.MotherCut         = _motherCut

    return Selection ( name,
                       Algorithm = _2hLoose,
                       RequiredSelections = [StdLoosePions])

def make2h(name, 
           Sel2hLoose, 
           _2h_PTmax,
           _2h_PTsum,
           _2h_PVIPCHI2sum,
           _2h_FDCHI2,
           _2h_DIRA,
           _2h_CORRMmax,
           _2h_CORRMmin,
           _2h_CharmVetoIPCHI2,
           _2h_CharmVetoM,
	   _2h_TRKCHIDOFmin,
           _2h_Mmin,
	   _2h_PVDOCAmin,
	   _2h_CHI2,
	   _2h_PTRANS) :
 
    _tightCutFor2h = " (M > %(_2h_Mmin)s*MeV) \
                     & (MAXTREE((('pi+'==ABSID) | ('pi-'==ABSID)),PT) > %(_2h_PTmax)s*MeV) \
		     & (SUMTREE(PT,(('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_2h_PTsum)s*MeV) \
		     & (SUMTREE(MIPCHI2DV(PRIMARY),(('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_2h_PVIPCHI2sum)s) \
		     & (BPVVDCHI2 > %(_2h_FDCHI2)s) \
		     & (BPVDIRA > %(_2h_DIRA)s) \
		     & (BPVCORRM < %(_2h_CORRMmax)s*MeV) \
		     & (BPVCORRM > %(_2h_CORRMmin)s*MeV) \
		     & ((M > %(_2h_CharmVetoM)s*MeV) | (BPVIPCHI2() > %(_2h_CharmVetoIPCHI2)s)) \
		     & (MINTREE((('pi+'==ABSID) | ('pi-'==ABSID)),TRCHI2DOF) < %(_2h_TRKCHIDOFmin)s) \
                     & (VFASPF(VMINVDDV(PRIMARY)) > %(_2h_PVDOCAmin)s) \
                     & (BPVPTFLIGHT < %(_2h_PTRANS)s*MeV) \
		     & (VFASPF(VCHI2) < %(_2h_CHI2)s) " % locals()
    
    
    _2hFilter = FilterDesktop(Code = _tightCutFor2h)

    return Selection (name,
                      Algorithm = _2hFilter,
                      RequiredSelections = [Sel2hLoose])

def make3h(name,
           Sel2hLoose,
           _h_PT,
           _h_P,
           _h_IPCHI2,
           _h_TRCHI2DOF,
           _3h_DOCA,
           _3h_PTmax,
           _3h_DIRA,
           _3h_FDCHI2,
           _3h_PVDOCAmin,
           _3h_CHI2,
           _3h_IP,
           _3h_PT,
           _3h_PTsum,
           _3h_PVIPCHI2sum,
           _3h_Charge,
           _3h_CORRMmax,
           _3h_CORRMmin,
           _3h_Mmin,
           _3h_Mmax) :

    _daughtersCuts = {"pi+" : "(PT > %(_h_PT)s*MeV) \
                             & (P > %(_h_P)s*MeV) \
                             & (MIPCHI2DV(PRIMARY) > %(_h_IPCHI2)s) \
                             & (TRCHI2DOF < %(_h_TRCHI2DOF)s)" % locals()}
    _combinationCut = "(AM < %(_3h_Mmax)s*MeV) \
                     & (AM > %(_3h_Mmin)s*MeV) \
		     & (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(_3h_DOCA)s)" % locals()
    _motherCut = "(MAXTREE((('pi+'==ABSID) | ('pi-'==ABSID)),PT) > %(_3h_PTmax)s*MeV) \
                & (BPVDIRA > %(_3h_DIRA)s) \
		& (BPVVDCHI2 > %(_3h_FDCHI2)s) \
		& (VFASPF(VMINVDDV(PRIMARY)) > %(_3h_PVDOCAmin)s) \
		& (VFASPF(VCHI2) < %(_3h_CHI2)s) \
		& (MIPDV(PRIMARY) < %(_3h_IP)s) \
		& (PT > %(_3h_PT)s*MeV) \
		& (SUMTREE(PT,(('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PTsum)s*MeV) \
		& (SUMTREE(MIPCHI2DV(PRIMARY),(('pi+'==ABSID) | ('pi-'==ABSID)),0.0) > %(_3h_PVIPCHI2sum)s) \
		& (abs(CHILD(1,SUMQ) + CHILD(2,Q))== %(_3h_Charge)s ) \
		& (BPVCORRM < %(_3h_CORRMmax)s * MeV)& (BPVCORRM > %(_3h_CORRMmin)s*MeV)" % locals()

   
    _3h=CombineParticles()
    _3h.DecayDescriptors  = ["B+ -> rho(770)0 pi+","B+ -> rho(770)0 pi-"]
    _3h.MotherCut         = _motherCut
    _3h.CombinationCut    = _combinationCut
    _3h.DaughtersCuts     = _daughtersCuts


    return Selection ( name,
                       Algorithm = _3h,
                       RequiredSelections = [Sel2hLoose, StdLoosePions])

def globalEventCutFilter(name, 
	                 MaxTrSIZE = None 
	                 ) :
  
  if MaxTrSIZE == None : return None
  
  from Configurables import LoKi__VoidFilter as VoidFilter
  from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
  modules = CoreFactory('CoreFactory').Modules
  for i in ['LoKiTracks.decorators']:
     if i not in modules : modules.append(i)
  
  _code = ""
  if MaxTrSIZE != None : _code += "TrSOURCE('Rec/Track/Best') >> (TrSIZE < %(MaxTrSIZE)s )" % locals()
  globalFilter= VoidFilter(name)
  globalFilter.Code = _code
  
  return globalFilter





