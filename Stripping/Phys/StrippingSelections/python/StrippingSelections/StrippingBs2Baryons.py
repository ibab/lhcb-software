# $Id: StrippingBs2Baryons.py,v 1.2 2014-08-20 $
'''
Stripping lines for baryonic Bs decays.
The following decays are considered:
   Bs0 -> Lambda_c+ p~- K+ pi-
   Bs0 -> Lambda_c+ p~- KS0
   Bs0 -> Lambda_c+ Lambda~0  pi-
   Bs0 -> Lambda0 Lambda~0
   B0/Bs0 -> Lambda_c+ p~- pi+ pi-
with:
   Lambda_c+ -> p+ K- pi+
   KS0 -> pi+ pi-
   Lambda0 -> p+ pi-

where KS0 and Lambda0 are from StdLoose{Lambda,KS}{DD,LL}
while final tracks are from StdLoose{Kaons,Pions,Protons}
'''

__author__ = ["yanxi zhang"]
__date__ = '20/Aug/2014'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'Bs0Builder'               ## LineBuilder class specialization
	, 'default_name'            ## Default name for Bs0Builder object
	, "default_config"
	, 'makeLc'                  ## Lambda_c+ selection
	, 'makeBs'                  ## Bs0 selection
	, 'filterKaons'
	, 'filterPions'
	, 'filterProtons'
	, "makeKSLL"
	, "makeLambdaLL"
	, "makeKSDD"
	, "makeLambdaDD"
	)



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLoosePions, StdAllLooseKaons, StdAllLooseProtons
from StandardParticles import StdLoosePions
from CommonParticles.StdLooseLambda import StdLooseLambdaLL, StdLooseLambdaDD
from GaudiKernel.SystemOfUnits import MeV, GeV, mm, ns
from Configurables import TisTosParticleTagger


default_name = "Bs0"


default_config = {
	'NAME' : 'BaryonicBsDecays'
	,'WGs'  : ['BandQ']
	,'BUILDERTYPE':'Bs0Builder'
	,'CONFIG'  : {
	   'Pi_InputList'               : 'Phys/StdLoosePions/Particles'
	   , 'K_InputList'              : 'Phys/StdLooseKaons/Particles'
	   , 'P_InputList'              : 'Phys/StdLooseProtons/Particles'
	   , 'Lc_Daug_TRCHI2DOF_Max'       :   5.0
	   , 'Lc_Daug_PT_Min'              : 250.0*MeV
	   , 'Lc_Daug_P_Min'               :   2.0*GeV
	   , 'Lc_K_ProbNNk_Min'            :   0.05
	   , 'Lc_Pi_ProbNNpi_Min'          :   0.01
	   , 'Lc_P_ProbNNp_Min'            :   0.1
	   , 'Lc_ADMASS_HalfWin'           :  30.0*MeV
	   , 'Lc_APT_Min'                  :   1.0*GeV
	   , 'Lc_ADOCAMAX_Max'             :   0.5*mm
	   , 'Lc_VCHI2_Max'                :  15.0
	   , 'Lc_BPVVDCHI2_Min'            :  16.0
	   , 'Lc_BPVDIRA_Min'              :   0.99
	   , 'Lc_BPVIPCHI2_Min'          :   4 
	   , 'Bs0_Daug_TRCHI2DOF_Max'      :   5.0
	   , 'Bs0_Daug_P_Min'              :   2.0*GeV
	   , 'Bs0_Daug_PT_Min'             : 250.0*MeV
	   , 'Bs0_Daug_MIPCHI2DV_Min'      :   4.0
	   , 'Bs0_Pi_ProbNNpi_Min'         :   0.01
	   , 'Bs0_K_ProbNNk_Min'           :   0.05
	   , 'Bs0_P_ProbNNp_Min'           :   0.1
	   , 'Bs0_APT_Min'                :   2.0*GeV 
	   , 'Bs0_ADOCAMAX_Max'           :   0.5*mm
	   , 'Bs0_AM_Max'                 :   700*MeV
	   , 'Bs0_BPVDIRA_Min'            :   0.99
	   , 'Bs0_BPVVDCHI2_Min'          :   16
	   , 'Bs0_VtxChi2_NDF_Max'        :   6
	   , 'Bs0_BPVIPCHI2_Max'          :   16 
	   , 'Bs0_ADOCAMAX_Long_Max'        : 5*mm 
	   , 'Bs0_BPVDIRA_Long_Min'         : 0.9
	   , 'Bs0_BPVIPCHI2_Long_Max'       : 25
	   , 'Bs0_VtxChi2_NDF_Long_Max'     : 16
	   , 'Bs0_BPVVDCHI2_Long_Min'       : 4
	   ,'Lambda_DD_MassWindow'     : 30.0
	   ,'Lambda_DD_VtxChi2'        : 25.0
	   ,'Lambda_DD_FDChi2'         : 100.0
	   ,'Lambda_LL_MassWindow'     : 25.0
	   ,'Lambda_LL_VtxChi2'        : 25.0
	   ,'Lambda_LL_FDChi2'         : 100.0
	   ,'KS_DD_MassWindow'     : 30.0
	   ,'KS_DD_VtxChi2'        : 25.0
	   ,'KS_DD_FDChi2'         : 100.0
	   ,'KS_LL_MassWindow'     : 25.0
	   ,'KS_LL_VtxChi2'        : 25.0
	   ,'KS_LL_FDChi2'         : 100.0
	   }
	,'STREAMS':['Bhadron']
	}




class Bs0Builder(LineBuilder) :
   __configuration_keys__ = (
	   'Pi_InputList'               
	   , 'K_InputList'              
	   , 'P_InputList'              
	   , 'Lc_Daug_TRCHI2DOF_Max'       
	   , 'Lc_Daug_PT_Min'              
	   , 'Lc_Daug_P_Min'               
	   , 'Lc_K_ProbNNk_Min'            
	   , 'Lc_Pi_ProbNNpi_Min'          
	   , 'Lc_P_ProbNNp_Min'            
	   , 'Lc_ADMASS_HalfWin'           
	   , 'Lc_APT_Min'                  
	   , 'Lc_ADOCAMAX_Max'             
	   , 'Lc_VCHI2_Max'                
	   , 'Lc_BPVVDCHI2_Min'            
	   , 'Lc_BPVDIRA_Min'              
	   , 'Lc_BPVIPCHI2_Min'
	   , 'Bs0_Daug_TRCHI2DOF_Max'    
	   , 'Bs0_Daug_P_Min'            
	   , 'Bs0_Daug_PT_Min'           
	   , 'Bs0_Daug_MIPCHI2DV_Min'    
	   , 'Bs0_Pi_ProbNNpi_Min'       
	   , 'Bs0_K_ProbNNk_Min'         
	   , 'Bs0_P_ProbNNp_Min'         
	   , 'Bs0_APT_Min'               
	   , 'Bs0_ADOCAMAX_Max'          
	   , 'Bs0_AM_Max'                
	   , 'Bs0_BPVDIRA_Min'           
	   , 'Bs0_ADOCAMAX_Long_Max'          
	   , 'Bs0_BPVDIRA_Long_Min'           
	   , 'Bs0_BPVIPCHI2_Long_Max'         
	   , 'Bs0_VtxChi2_NDF_Long_Max'       
	   , 'Bs0_BPVVDCHI2_Long_Min'         
	   , 'Bs0_BPVVDCHI2_Min'         
	   , 'Bs0_VtxChi2_NDF_Max'       
	   , 'Bs0_BPVIPCHI2_Max'         
	   ,'Lambda_DD_MassWindow'
	   ,'Lambda_DD_VtxChi2'   
	   ,'Lambda_DD_FDChi2'    
	   ,'Lambda_LL_MassWindow'
	   ,'Lambda_LL_VtxChi2'   
	   ,'Lambda_LL_FDChi2'    
	   ,'KS_DD_MassWindow'    
	   ,'KS_DD_VtxChi2'       
	   ,'KS_DD_FDChi2'        
	   ,'KS_LL_MassWindow'    
	   ,'KS_LL_VtxChi2'       
	   ,'KS_LL_FDChi2'        
	   )
   ## Possible parameters and default values copied from the definition
   ##   of StrippingLine
   def _strippingLine ( self,
	   name             ,   # the base name for the Line
	   prescale  = 1.0  ,   # prescale factor
	   postscale = 1.0    ,   # postscale factor
	   selection = None) : # other configuration parameters
	if (prescale > 0) and (postscale > 0) : # {
	   line = StrippingLine( name,
		   prescale        = prescale,
		   postscale       = postscale,
		   selection       = selection )
	   self.registerLine(line)
	   return line
	else : 
	   return False

   def __init__(self, name, config) :
	LineBuilder.__init__(self, name, config)
	# Pick up standard kaons, pions, protons
	# Filter them for use as daughter particles:
	self.dauPi = filterPions(name+'FilteredPions',
		Bs0_Daug_TRCHI2DOF_Max = config['Bs0_Daug_TRCHI2DOF_Max'],
		Bs0_Daug_P_Min = config['Bs0_Daug_P_Min'],
		Bs0_Daug_PT_Min = config['Bs0_Daug_PT_Min'],
		Bs0_Pi_ProbNNpi_Min = config['Bs0_Pi_ProbNNpi_Min'],
		Bs0_Daug_MIPCHI2DV_Min = config['Bs0_Daug_MIPCHI2DV_Min'],
		Bs0_Pi_Input = config['Pi_InputList']
		)
	self.dauK = filterKaons(name+'FilteredKaons',
		Bs0_Daug_TRCHI2DOF_Max = config['Bs0_Daug_TRCHI2DOF_Max'],
		Bs0_Daug_P_Min = config['Bs0_Daug_P_Min'],
		Bs0_Daug_PT_Min = config['Bs0_Daug_PT_Min'],
		Bs0_K_ProbNNk_Min = config['Bs0_K_ProbNNk_Min'],
		Bs0_Daug_MIPCHI2DV_Min = config['Bs0_Daug_MIPCHI2DV_Min'],
		Bs0_K_Input = config['K_InputList']
		)
	self.dauP = filterProtons(name+'FilteredProtons',
		Bs0_Daug_TRCHI2DOF_Max = config['Bs0_Daug_TRCHI2DOF_Max'],
		Bs0_Daug_P_Min = config['Bs0_Daug_P_Min'],
		Bs0_Daug_PT_Min = config['Bs0_Daug_PT_Min'],
		Bs0_P_ProbNNp_Min = config['Bs0_P_ProbNNp_Min'],
		Bs0_Daug_MIPCHI2DV_Min = config['Bs0_Daug_MIPCHI2DV_Min'],
		Bs0_P_Input = config['P_InputList']
		)

	# Pick up standard Lambdac -> p K- pi+ then filter it to reduce rate:
	self.filterLc = makeLc(name+'FilterLc',
		Lc_K_Input = config['K_InputList'],
		Lc_P_Input = config['P_InputList'],
		Lc_Pi_Input = config['Pi_InputList'],
		Lc_Daug_TRCHI2DOF_Max 		=config['Lc_Daug_TRCHI2DOF_Max'],
		Lc_Daug_P_Min 			=config['Lc_Daug_P_Min'],
		Lc_Daug_PT_Min 			=config['Lc_Daug_PT_Min'],
		Lc_K_ProbNNk_Min			=config['Lc_K_ProbNNk_Min'],
		Lc_Pi_ProbNNpi_Min		=config['Lc_Pi_ProbNNpi_Min'],
		Lc_P_ProbNNp_Min  		=config['Lc_P_ProbNNp_Min'],
		Lc_ADOCAMAX_Max			=config['Lc_ADOCAMAX_Max'],
		Lc_APT_Min				=config['Lc_APT_Min'],
		Lc_VCHI2_Max			=config['Lc_VCHI2_Max'],
		Lc_ADMASS_HalfWin			=config['Lc_ADMASS_HalfWin'],
		Lc_BPVVDCHI2_Min			=config['Lc_BPVVDCHI2_Min'],
		Lc_BPVDIRA_Min			=config['Lc_BPVDIRA_Min'],
		Lc_BPVIPCHI2_Min			=config['Lc_BPVIPCHI2_Min']
		)

	# Pick up standard Lambda and KS0
	# Filter them for use as daughter particles:
	self.KSLL = makeKSLL(name+"FiltedKSLL", 
		KS_LL_MassWindow = config['KS_LL_MassWindow'],
		KS_LL_VtxChi2    = config['KS_LL_VtxChi2'],
		KS_LL_FDChi2     = config['KS_LL_FDChi2']
		)
	self.KSDD = makeKSDD(name+"FiltedKSDD", 
		KS_DD_MassWindow = config['KS_DD_MassWindow'],
		KS_DD_VtxChi2    = config['KS_DD_VtxChi2'],
		KS_DD_FDChi2     = config['KS_DD_FDChi2']
		)
	self.LambdaLL = makeLambdaLL(name+"FiltedLambdaLL", 
		Lambda_LL_MassWindow = config['Lambda_LL_MassWindow'],
		Lambda_LL_VtxChi2    = config['Lambda_LL_VtxChi2'],
		Lambda_LL_FDChi2     = config['Lambda_LL_FDChi2']
		)
	self.LambdaDD = makeLambdaDD(name+"FiltedLambdaDD", 
		Lambda_DD_MassWindow = config['Lambda_DD_MassWindow'],
		Lambda_DD_VtxChi2    = config['Lambda_DD_VtxChi2'],
		Lambda_DD_FDChi2     = config['Lambda_DD_FDChi2']
		)

	## Some generic cuts for Bs0.
	## Vertex chi2 cut depends on number of daughters:
	##      (2 dau => 1 NDF; 3 dau => 3 NDF; 4 dau => 5 NDF)
	_strCutComb = "(ADAMASS(5.1*GeV)<1.1*%(Bs0_AM_Max)s)" \
		"& (APT>%(Bs0_APT_Min)s)" \
		"& (ADOCAMAX('')<%(Bs0_ADOCAMAX_Max)s)" % config
	_strCutMoth  = "(ADMASS(5.1*GeV)<%(Bs0_AM_Max)s)" \
		"& (BPVVDCHI2 > %(Bs0_BPVVDCHI2_Min)s)" \
		"& (BPVIPCHI2()< %(Bs0_BPVIPCHI2_Max)s)" \
		"& (BPVDIRA > %(Bs0_BPVDIRA_Min)s)" % config
	_strCutComb_Long = "(ADAMASS(5.1*GeV)<1.1*%(Bs0_AM_Max)s)" \
		"& (APT>%(Bs0_APT_Min)s)" \
		"& (ADOCAMAX('')<%(Bs0_ADOCAMAX_Long_Max)s)" % config
	_strCutMoth_Long  = "(ADMASS(5.1*GeV)<%(Bs0_AM_Max)s)" \
		"& (BPVVDCHI2 > %(Bs0_BPVVDCHI2_Long_Min)s)" \
		"& (BPVIPCHI2()< %(Bs0_BPVIPCHI2_Long_Max)s)" \
		"& (BPVDIRA > %(Bs0_BPVDIRA_Long_Min)s)" % config

	_strChi2Moth = "(VFASPF(VCHI2/VDOF)<%(Bs0_VtxChi2_NDF_Max)s)" % config
	_strChi2Moth_Long = "(VFASPF(VCHI2/VDOF)<%(Bs0_VtxChi2_NDF_Long_Max)s)" % config

	_strCutMoth = _strChi2Moth + '&' + _strCutMoth
	_strCutMoth_Long = _strChi2Moth_Long + '&' + _strCutMoth_Long


	#make Bs0 
	"""
	Bs0 -> Lambda_c+ p~- K+ pi-
	Bs0 -> Lambda_c+ p~- KS0
	Bs0 -> Lambda_c+ Lambda~0  pi-
	Bs0 -> Lambda0 Lambda~0
	B0/Bs0 -> Lambda_c+ p~- pi+ pi-
	"""
	self.combineBs00 = makeBs0(name+'CombineBs00', [ self.filterLc, self.dauPi,self.dauP ]
		, '[B_s0 -> Lambda_c+ p~- pi+ pi-]cc', _strCutComb, _strCutMoth)
	self.combineBs01 = makeBs0(name+'CombineBs01', [ self.filterLc, self.dauPi,self.dauP, self.dauK ]
		, '[B_s0 -> Lambda_c+ p~- K+ pi-]cc', _strCutComb, _strCutMoth)
	self.combineBs02LL = makeBs0(name+'CombineBs02LL', [ self.filterLc, self.dauP, self.KSLL]
		, '[B_s0 -> Lambda_c+ p~- KS0]cc', _strCutComb_Long, _strCutMoth_Long)
	self.combineBs02DD = makeBs0(name+'CombineBs02DD', [ self.filterLc, self.dauP, self.KSDD]
		, '[B_s0 -> Lambda_c+ p~- KS0]cc', _strCutComb_Long, _strCutMoth_Long)
	self.combineBs03LL = makeBs0(name+'CombineBs03LL', [ self.filterLc, self.dauPi, self.LambdaLL]
		, '[B_s0 -> Lambda_c+ Lambda~0 pi-]cc', _strCutComb_Long, _strCutMoth_Long)
	self.combineBs03DD = makeBs0(name+'CombineBs03DD', [ self.filterLc, self.dauPi, self.LambdaDD]
		, '[B_s0 -> Lambda_c+ Lambda~0 pi-]cc', _strCutComb_Long, _strCutMoth_Long)
	self.combineBs04LLDD = makeBs0(name+'CombineBs04LLDD', [ self.LambdaLL,self.LambdaDD] , '[B_s0 -> Lambda0 Lambda~0]cc', _strCutComb_Long, _strCutMoth_Long)


	#dau particles, only for tests
	"""
	self.linePions = self._strippingLine(name = name+'PionsForBs0', prescale = 1.0, postscale = 1.0, selection = self.dauPi)
	self.lineKaons = self._strippingLine(name = name+'KaonsForBs0', prescale = 1.0, postscale = 1.0, selection = self.dauK)
	self.lineProtons = self._strippingLine(name = name+'ProtonsForBs0', prescale = 1.0, postscale = 1.0, selection = self.dauP)
	self.lineLc = self._strippingLine(name = name+'LcForBs0', prescale = 1.0, postscale = 1.0, selection = self.filterLc)
	self.lineKSDD = self._strippingLine(name = name+'KSDDForBs0', prescale = 1.0, postscale = 1.0, selection = self.KSDD)
	self.lineKSLL = self._strippingLine(name = name+'KSLLForBs0', prescale = 1.0, postscale = 1.0, selection = self.KSLL)
	self.lineLambdaDD = self._strippingLine(name = name+'LambdaDDForBs0', prescale = 1.0, postscale = 1.0, selection = self.LambdaDD)
	self.lineLambdaLL = self._strippingLine(name = name+'LambdaLLForBs0', prescale = 1.0, postscale = 1.0, selection = self.LambdaLL)
	"""
	# Physics lines
	self.lineBs00 = self._strippingLine(name = name+'Bs0ToLcPPiPi', prescale = 1.0, postscale = 1.0, selection = self.combineBs00)
	self.lineBs01 = self._strippingLine(name = name+'Bs0ToLcPKPi', prescale = 1.0, postscale = 1.0, selection = self.combineBs01)
	self.lineBs02LL = self._strippingLine(name = name+'Bs0ToLcPKSLL', prescale = 1.0, postscale = 1.0, selection = self.combineBs02LL)
	self.lineBs02DD = self._strippingLine(name = name+'Bs0ToLcPKSDD', prescale = 1.0, postscale = 1.0, selection = self.combineBs02DD)
	self.lineBs03LL = self._strippingLine(name = name+'Bs0ToLcLambdaPiLL', prescale = 1.0, postscale = 1.0, selection = self.combineBs03LL)
	self.lineBs03DD = self._strippingLine(name = name+'Bs0ToLcLambdaPiDD', prescale = 1.0, postscale = 1.0, selection = self.combineBs03DD)
	self.lineBs04LLDD = self._strippingLine(name = name+'Bs0ToLambdaLambda', prescale = 1.0, postscale = 1.0, selection = self.combineBs04LLDD)

def makeLc(localName,
		Lc_K_Input,
		Lc_P_Input,
		Lc_Pi_Input,
		Lc_Daug_TRCHI2DOF_Max,
		Lc_Daug_P_Min,
		Lc_Daug_PT_Min,
		Lc_K_ProbNNk_Min,
		Lc_Pi_ProbNNpi_Min,
		Lc_P_ProbNNp_Min,
		Lc_ADMASS_HalfWin,
		Lc_ADOCAMAX_Max,
		Lc_APT_Min,
		Lc_VCHI2_Max,
		Lc_BPVVDCHI2_Min,
		Lc_BPVDIRA_Min,
		Lc_BPVIPCHI2_Min,
		):
   ## Pick up standard Lambdac -> p K- pi+
   _LcKaons = DataOnDemand(Location = Lc_K_Input)
   _LcPions = DataOnDemand(Location = Lc_Pi_Input)
   _LcProtons = DataOnDemand(Location = Lc_P_Input)

   ## Daughter cuts
   _strCutAllDaug = "(TRCHI2DOF<%(Lc_Daug_TRCHI2DOF_Max)s)& (PT>%(Lc_Daug_PT_Min)s)& (P>%(Lc_Daug_P_Min)s)" % locals()
   _strCutK  = _strCutAllDaug + '&' + "(HASRICH)&((PROBNNk)>%(Lc_K_ProbNNk_Min)s)" % locals()
   _strCutpi = _strCutAllDaug + '&' + "(HASRICH)&((PROBNNpi)>%(Lc_Pi_ProbNNpi_Min)s)" % locals()
   _strCutp  = _strCutAllDaug + '&' + "(HASRICH)&((PROBNNp)>%(Lc_P_ProbNNp_Min)s)" % locals()

   _daughterCuts = { 'pi+' : _strCutpi, 'K-'  : _strCutK, 'p+'  : _strCutp }

   ## Combination cuts
   _strCutComb = "(ADAMASS('Lambda_c+')<1.1*%(Lc_ADMASS_HalfWin)s)" \
	   "& (ADOCAMAX('')<%(Lc_ADOCAMAX_Max)s)" \
	   "& (APT>%(Lc_APT_Min)s)" % locals()

   _strCutMoth = "(VFASPF(VCHI2) < %(Lc_VCHI2_Max)s)" \
	   "& (ADMASS('Lambda_c+')<%(Lc_ADMASS_HalfWin)s)" \
	   "& (BPVVDCHI2>%(Lc_BPVVDCHI2_Min)s)" \
	   "& (BPVIPCHI2()> %(Lc_BPVIPCHI2_Min)s)" \
	   "& (BPVDIRA>%(Lc_BPVDIRA_Min)s)" % locals()

   _combineLambdac2PKPi = CombineParticles(
	   DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc",
	   DaughtersCuts = _daughterCuts,
	   CombinationCut = _strCutComb,
	   MotherCut = _strCutMoth
	   )

   return Selection ( localName,
	    Algorithm = _combineLambdac2PKPi,
	    RequiredSelections = [ _LcKaons, _LcPions, _LcProtons ] )



def filterKaons(localName,
		Bs0_Daug_TRCHI2DOF_Max,
		Bs0_Daug_P_Min,
		Bs0_Daug_PT_Min,
		Bs0_K_ProbNNk_Min,
		Bs0_Daug_MIPCHI2DV_Min,
		Bs0_K_Input
		):
   # Pick up standard input list
   _stdK = DataOnDemand(Location = Bs0_K_Input)
   # Filter:
   _strCutDauK = "(TRCHI2DOF<%(Bs0_Daug_TRCHI2DOF_Max)s)" \
	   "& (P>%(Bs0_Daug_P_Min)s)" \
	   "& (HASRICH)&(PROBNNk>%(Bs0_K_ProbNNk_Min)s)" \
	   "& (PT>%(Bs0_Daug_PT_Min)s)" \
	   "& (PROBNNghost<0.5)"\
	   "& (MIPCHI2DV(PRIMARY)>%(Bs0_Daug_MIPCHI2DV_Min)s)" %locals() 

   _filterK = FilterDesktop(Code = _strCutDauK)
   return Selection ( localName,
	    Algorithm = _filterK,
	    RequiredSelections = [ _stdK ] )


def filterPions(localName,
		Bs0_Daug_TRCHI2DOF_Max,
		Bs0_Daug_P_Min,
		Bs0_Daug_PT_Min,
		Bs0_Pi_ProbNNpi_Min,
		Bs0_Daug_MIPCHI2DV_Min,
		Bs0_Pi_Input
		):
   # Pick up standard input list
   _stdPi = DataOnDemand(Location = Bs0_Pi_Input)
   # Filter:
   _strCutDauPi = "(TRCHI2DOF<%(Bs0_Daug_TRCHI2DOF_Max)s)" \
	   "& (P>%(Bs0_Daug_P_Min)s)" \
	   "& (HASRICH)&(PROBNNpi>%(Bs0_Pi_ProbNNpi_Min)s)" \
	   "& (PT>%(Bs0_Daug_PT_Min)s)" \
	   "& (PROBNNghost<0.5)"\
	   "& (MIPCHI2DV(PRIMARY)>%(Bs0_Daug_MIPCHI2DV_Min)s)" %locals()

   _filterPi = FilterDesktop(Code = _strCutDauPi)
   return Selection ( localName,
	    Algorithm = _filterPi,
	    RequiredSelections = [ _stdPi ] )


def filterProtons(localName,
		Bs0_Daug_TRCHI2DOF_Max,
		Bs0_Daug_P_Min,
		Bs0_Daug_PT_Min,
		Bs0_P_ProbNNp_Min,
		Bs0_Daug_MIPCHI2DV_Min,
		Bs0_P_Input
		):
   # Pick up standard input list
   _stdP = DataOnDemand(Location = Bs0_P_Input)
   # Filter:
   _strCutDauP  = "(TRCHI2DOF<%(Bs0_Daug_TRCHI2DOF_Max)s)" \
	   "& (P>%(Bs0_Daug_P_Min)s)" \
	   "& (HASRICH)&(PROBNNp>%(Bs0_P_ProbNNp_Min)s)" \
	   "& (PT>%(Bs0_Daug_PT_Min)s)" \
	   "& (PROBNNghost<0.5)"\
	   "& (MIPCHI2DV(PRIMARY)>%(Bs0_Daug_MIPCHI2DV_Min)s)" %locals() 

   _filterP  = FilterDesktop(Code = _strCutDauP)
   return Selection ( localName,
	    Algorithm = _filterP,
	    RequiredSelections = [ _stdP ] )


def makeBs0(localName, inputSelections, decay, cutComb, cutMoth) :
   _combineBs0 = CombineParticles( DecayDescriptor = decay, CombinationCut = cutComb, MotherCut = cutMoth )
   return Selection ( localName, Algorithm = _combineBs0, RequiredSelections = inputSelections)

def makeLambdaLL(localName, Lambda_LL_MassWindow, Lambda_LL_VtxChi2,Lambda_LL_FDChi2):
   # define all the cuts
   _massCut    = "(ADMASS('Lambda0')<%s*MeV)" % Lambda_LL_MassWindow
   _vtxCut     = "(VFASPF(VCHI2)<%s)"         % Lambda_LL_VtxChi2
   _fdCut      = "(BPVVDCHI2>%s)"             % Lambda_LL_FDChi2
   _allCuts = _massCut+'&'+_vtxCut+'&'+_fdCut
   _stdLambdaLL = DataOnDemand(Location = "Phys/StdLooseLambdaLL/Particles")
   _filterLambdaLL = FilterDesktop( Code = _allCuts )
   return Selection( localName, Algorithm = _filterLambdaLL, RequiredSelections = [_stdLambdaLL] )

def makeLambdaDD(localName, Lambda_DD_MassWindow, Lambda_DD_VtxChi2,Lambda_DD_FDChi2):
   # define all the cuts
   _massCut    = "(ADMASS('Lambda0')<%s*MeV)" % Lambda_DD_MassWindow
   _vtxCut     = "(VFASPF(VCHI2)<%s)"         % Lambda_DD_VtxChi2
   _fdCut      = "(BPVVDCHI2>%s)"             % Lambda_DD_FDChi2
   _code = "(P>4000.*MeV)&(PT>300.*MeV)&(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>0.03*ns)&CHILDCUT((MIPCHI2DV(PRIMARY)>60.),1)&CHILDCUT((MIPCHI2DV(PRIMARY)>60.),2)"
   _allCuts = _massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_code
   _stdLambdaDD = DataOnDemand(Location = "Phys/StdLooseLambdaDD/Particles")
   _filterLambdaDD = FilterDesktop( Code = _allCuts )
   return Selection( localName, Algorithm = _filterLambdaDD, RequiredSelections = [_stdLambdaDD] )

def makeKSLL(localName, KS_LL_MassWindow, KS_LL_VtxChi2,KS_LL_FDChi2):
   # define all the cuts
   _massCut    = "(ADMASS('KS0')<%s*MeV)" % KS_LL_MassWindow
   _vtxCut     = "(VFASPF(VCHI2)<%s)"         % KS_LL_VtxChi2
   _fdCut      = "(BPVVDCHI2>%s)"             % KS_LL_FDChi2
   _allCuts = _massCut+'&'+_vtxCut+'&'+_fdCut
   _stdKSLL = DataOnDemand(Location = "Phys/StdLooseKsLL/Particles")
   _filterKSLL = FilterDesktop( Code = _allCuts )
   return Selection( localName, Algorithm = _filterKSLL, RequiredSelections = [_stdKSLL] )

def makeKSDD(localName, KS_DD_MassWindow, KS_DD_VtxChi2,KS_DD_FDChi2):
   # define all the cuts
   _massCut    = "(ADMASS('KS0')<%s*MeV)" % KS_DD_MassWindow
   _vtxCut     = "(VFASPF(VCHI2)<%s)"         % KS_DD_VtxChi2
   _fdCut      = "(BPVVDCHI2>%s)"             % KS_DD_FDChi2
   _code = "(P>4000.*MeV)&(PT>500.*MeV)&(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>0.01*ns)&CHILDCUT((MIPCHI2DV(PRIMARY)>30.),1)&CHILDCUT((MIPCHI2DV(PRIMARY)>30.),2)"
   _allCuts = _massCut+'&'+_vtxCut+'&'+_fdCut+'&'+_code
   _stdKSDD = DataOnDemand(Location = "Phys/StdLooseKsDD/Particles")
   _filterKSDD = FilterDesktop( Code = _allCuts )
   return Selection( localName, Algorithm = _filterKSDD, RequiredSelections = [_stdKSDD] )

