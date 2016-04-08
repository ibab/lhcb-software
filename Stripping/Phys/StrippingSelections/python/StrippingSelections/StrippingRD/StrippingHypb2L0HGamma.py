
__author__ = 'Luis Miguel Garcia Martin, luis.miguel.garcia.martin@cern.ch'
__date__ = '08/04/2016'
__version__ = '1.0'
__description__ = '[Xi_b- -> (Xi- -> (Lambda0 -> p+ pi-) pi-) gamma], [Omega_b- -> (Omega- -> (Lambda0 -> p+ pi-) K-) gamma]'
__all__ = ('Hypb2L0HGammaConf',
		   'default_config', )
		   
from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, TisTosParticleTagger
from PhysSelPython.Wrappers      import Selection, MergedSelection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils        import LineBuilder
from StandardParticles import StdLooseAllPhotons

default_config = {
		'NAME'			:	'Hypb2L0HGamma',
		'WGs'			:	['RD'],
		'BUILDERTYPE'	:	'Hypb2L0HGammaConf',
		'CONFIG'		:	{'Prescale'				:	1,
		                     'Photon_PT_Min'        :   2500.0,
                               #Tracks
                             'TRCHI2DOF_MAX'        :   4.,
                             'TRACK_IPCHI2_MIN'     :   16.,            #
#                            'VCHI2PDOF_MAX'        :   16.0,
                             'Ghost_Prob'           :   0.4,
								#Xi
							 'CombMassWinXi'		:	60.,
							 'MassWinXi'			:	30.,
							 'MinPTXi'				:	500.,
							 'MinPXi'				:	10000.,
#							'BPVLTIME_MIN'         :   2.0 ,        proper time cut
#                           'BPVDIRA_MIN'           :   0.93,		proper time cut
#                           'BPVVDCHI2_MIN'         :   0.0,		Proper time cut
							 	#Xib
							 'CombMassWinXib'		:	800.,
							 'MinPTXib'				:	500.,
							 'MinPXib'				:	15000.,
							 'MTDOCACHI2_MAX'       :   15.,     #
							 	#Omega
							 'CombMassWinOmega'		:	120.,
							 'MassWinOmega' 		:	70.,
							 'MinPTO'				:	1000.,
							 'MinPO'				:	10000.,
							 	#Omegab
							 'CombMassWinOmegab'	:	1000.,
							 'MinPTOb'				:	1000.,
							 'MinPOb'				:	15000.,
							 
		                      'TISTOSLinesDict': {#'Hlt2Topo(2|3|4)Body.*Decision%TOS':0,
	                              #'Hlt2Topo(2|3|4)Body.*Decision%TIS':0,
	                              #'Hlt2IncPhi.*Decision%TOS':0,
	                              #'Hlt2IncPhi.*Decision%TIS':0,
	                              #'Hlt2RadiativeTopo.*Decision%TOS':0, ## Cut based raditive topological
	                              #'Hlt2RadiativeTopo.*Decision%TIS':0, ## Cut based raditive topological
	                              #'Hlt2TopoRad.*Decision%TOS':0, ## BBDT based radiative topological
	                              #'Hlt2TopoRad.*Decision%TIS':0, ## BBDT based radiative topological
	                              #'Hlt2Bs2PhiGamma.*Decision%TOS':0,
	                              #'Hlt2Bs2PhiGamma.*Decision%TIS':0,
	                              #'Hlt2Bd2KstGamma.*Decision%TOS':0,
	                              #'Hlt2Bd2KstGamma.*Decision%TIS':0
	                              #'Hlt2Radiative.*Decision%TOS':0,
	                              #'Hlt2Radiative.*Decision%TIS':0,
	                              #'Hlt2Radiative.*Decision%TUS':0,
	                              # """This is equivalent to:
	                              # 'Hlt2RadiativeInc.*Decision%TOS':0, ## Run II inclusive radiative lines (BBDT based)
	                              # 'Hlt2RadiativeInc.*Decision%TIS':0,
	                              # 'Hlt2RadiativeBs2PhiGamma.*Decision%TOS':0, ## Run II exclusive radiative lines
	                              # 'Hlt2RadiativeBs2PhiGamma.*Decision%TIS':0,
	                              # 'Hlt2RadiativeBd2KstGamma.*Decision%TOS':0,
	                              # 'Hlt2RadiativeBd2KstGamma.*Decision%TIS':0,
	                              # 'Hlt2RadiativeB2GammaGamma.*Decision%TOS':0,
	                              # 'Hlt2RadiativeB2GammaGamma.*Decision%TIS':0,
	                              #'Hlt2RadiativeLb2L0Gamma.*Decision%TOS':0,
	                              #'Hlt2RadiativeLb2L0Gamma.*Decision%TIS':0,
	                              #'Hlt2RadiativeHypb2L0HGamma.*Decision%TOS':0,
	                              #'Hlt2RadiativeHypb2L0HGamma.*Decision%TIS':0,
	                              #'Hlt2RadiativeInc.*Decision%TIS':0,
	                              #'Hlt2RadiativeInc.*Decision%TOS':0,
	                              'L0Photon.*Decision%TOS':0,
	                              'L0Electron.*Decision%TOS':0,
	                              #'L0Photon.*Decision%TIS':0,
	                              #'L0Electron.*Decision%TIS':0,
	                              }
							},
		'STREAMS'		:	['Leptonic'],
		}

#######################################################################
		
class Hypb2L0HGammaConf(LineBuilder):
	
	__configuration_keys__ = ('Prescale',
	                          'Photon_PT_Min',
							   #Tracks
                             'TRCHI2DOF_MAX',
                             'TRACK_IPCHI2_MIN', 
 #                           'VCHI2PDOF_MAX', 
                             'Ghost_Prob',
								#Xi
							 'CombMassWinXi',
							 'MassWinXi',
							 'MinPTXi',
							 'MinPXi',
	#						 'BPVLTIME_MIN',
	#						 'BPVDIRA_MIN',
	#						 'BPVVDCHI2_MIN',
							 	#Xib
							 'CombMassWinXib',
							 'MinPTXib',
							 'MinPXib',
							 'MTDOCACHI2_MAX',
							 	#Omega
							 'CombMassWinOmega',
							 'MassWinOmega',
							 'MinPTO',
							 'MinPO',
							 	#Omegab
							 'CombMassWinOmegab',
							 'MinPTOb',
							 'MinPOb',
							 'TISTOSLinesDict',
							 )
							  
	def __init__(self, name, config):
		
		LineBuilder.__init__(self, name, config)
		
		PionsL =DataOnDemand('Phys/StdLoosePions/Particles') 
		PionsD =DataOnDemand('Phys/StdNoPIDsDownPions/Particles') 
		Lambda_ll = DataOnDemand('Phys/StdLooseLambdaLL/Particles')
		Lambda_dd = DataOnDemand('Phys/StdLooseLambdaDD/Particles')
		
		photons_filter = FilterDesktop(name = "Photons_filter",
		                               Code="(PT > %(Photon_PT_Min)s*MeV)" % config)
		Photons = Selection("Photons"+name,
                            Algorithm=photons_filter,
                            RequiredSelections=[StdLooseAllPhotons])
		
#############		Making all Xi		#######################		
		self.selXi_LLL = makeXi2L0pi(name 	= "Xi2L0pi_LLL" + name,
							pions 	= PionsL,
							lambdas = Lambda_ll,
							params = config
							) 
							
		self.selXi_DDD = makeXi2L0pi(name 	= "Xi2L0pi_DDD" + name,
							pions 	= PionsD,
							lambdas = Lambda_dd,
							params = config
							) 
							
		self.selXi_DDL = makeXi2L0pi(name 	= "Xi2L0pi_DDL" + name,
							pions 	= PionsL,
							lambdas = Lambda_dd,
							params = config
							) 
							
#		self.mergedXi = MergedSelection("Xi_minus",
#										RequiredSelections=[self.selXi_LLL, self.selXi_DDD, self.selXi_DDL])
				
#############		Making  Xi_b LLL	#######################						
		self.SelXib_LLL = makeXib2XiGamma(name	=	"Xib2XiGamma_LLL",
								 xiSel	=	self.selXi_LLL,
								 photons=	Photons,
								 params = config
								)
								
		self.lineXib2XiGamma_LLL = StrippingLine("StripXib2XiGamma_LLL",
											 prescale = config['Prescale'],
											 selection = tisTosSelection(self.SelXib_LLL, config['TISTOSLinesDict']),
											 MDSTFlag = False,
											 RequiredRawEvents = ["Calo"]
											 )
											 
		self.registerLine(self.lineXib2XiGamma_LLL)
		
#############		Making  Xi_b DDD   #######################						
		self.SelXib_DDD = makeXib2XiGamma(name	=	"Xib2XiGamma_DDD",
								 xiSel	=	self.selXi_DDD,
								 photons=	Photons,
								 params = config
								)
								
		self.lineXib2XiGamma_DDD = StrippingLine("StripXib2XiGamma_DDD",
											 prescale = config['Prescale'],
											 selection = tisTosSelection(self.SelXib_DDD, config['TISTOSLinesDict']),
											 MDSTFlag = False,
											 RequiredRawEvents = ["Calo"]
											 )
											 
		self.registerLine(self.lineXib2XiGamma_DDD)
		
#############		Making  Xi_b DDL	#######################						
		self.SelXib_DDL = makeXib2XiGamma(name	=	"Xib2XiGamma_DDL",
								 xiSel	=	self.selXi_DDL,
								 photons=	Photons,
								 params = config
								)
								
		self.lineXib2XiGamma_DDL = StrippingLine("StripXib2XiGamma_DDL",
											 prescale = config['Prescale'],
											 selection = tisTosSelection(self.SelXib_DDL, config['TISTOSLinesDict']),
											 MDSTFlag = False,
											 RequiredRawEvents = ["Calo"]
											 )
											 
		self.registerLine(self.lineXib2XiGamma_DDL)
		
	##################################################################################
	##############################	Omega	##########################################
	##################################################################################
	
		KaonsL =DataOnDemand('Phys/StdLooseKaons/Particles') 
		KaonsD =DataOnDemand('Phys/StdLooseDownKaons/Particles') 
	
	#############		Making all Omegas		#######################		
		self.selO_LLL = makeO2L0K(name 	= "O2L0K_LLL" + name,
							kaons 	= KaonsL,
							lambdas = Lambda_ll,
							params = config
							) 
							
		self.selO_DDD = makeO2L0K(name 	= "O2L0K_DDD" + name,
							kaons 	= KaonsD,
							lambdas = Lambda_dd,
							params = config
							) 
							
		self.selO_DDL = makeO2L0K(name 	= "O2L0K_DDL" + name,
							kaons 	= KaonsL,
							lambdas = Lambda_dd,
							params = config
							) 
							
#		self.mergedO = MergedSelection("O_minus",
#										RequiredSelections=[self.selO_LLL, self.selO_DDD, self.selO_DDL])
				
#############		Making  Omega_b		#######################						
		self.SelOb_LLL = makeOb2OGamma(name	=	"Ob2OGamma_LLL",
								 oSel	=	self.selO_LLL,
								 photons=	Photons,
								 params = config
								)
								
		
		self.lineOb2OGamma_LLL = StrippingLine("StripOb2OGamma_LLL",
											 prescale = config['Prescale'],
											 selection = tisTosSelection(self.SelOb_LLL, config['TISTOSLinesDict']),
											 MDSTFlag = False,
											 RequiredRawEvents = ["Calo"]
											 )
											 
		self.registerLine(self.lineOb2OGamma_LLL)
							
#############		Making  Omega_b		#######################						
		self.SelOb_DDD = makeOb2OGamma(name	=	"Ob2OGamma_DDD",
								 oSel	=	self.selO_DDD,
								 photons=	Photons,
								 params = config
								)
								
		
		self.lineOb2OGamma_DDD = StrippingLine("StripOb2OGamma_DDD",
											 prescale = config['Prescale'],
											 selection = tisTosSelection(self.SelOb_DDD, config['TISTOSLinesDict']),
											 MDSTFlag = False,
											 RequiredRawEvents = ["Calo"]
											 )
											 
		self.registerLine(self.lineOb2OGamma_DDD)
							

#############		Making  Omega_b		#######################						
		self.SelOb_DDL = makeOb2OGamma(name	=	"Ob2OGamma_DDL",
								 oSel	=	self.selO_DDL,
								 photons=	Photons,
								 params = config
								)
								
		
		self.lineOb2OGamma_DDL = StrippingLine("StripOb2OGamma_DDL",
											 prescale = config['Prescale'],
											 selection = tisTosSelection(self.SelOb_DDL, config['TISTOSLinesDict']),
											 MDSTFlag = False,
											 RequiredRawEvents = ["Calo"]
											 )
											 
		self.registerLine(self.lineOb2OGamma_DDL)
							






#################################################################################################
#################################################################################################
#################################################################################################


def makeXi2L0pi(name, pions, lambdas, params):
	_name = name
	_daughter_cut = {'Lambda0' : 'ALL' , 
	                 'pi-'     : '(MIPCHI2DV(PRIMARY) > %(TRACK_IPCHI2_MIN)s) & (TRCHI2DOF < %(TRCHI2DOF_MAX)s)' %params } 
	_combcut = "(ADAMASS('Xi-') < %(CombMassWinXib)s * MeV)" %params
	
	_mothercut = "(ADMASS('Xi-') < %(MassWinXi)s * MeV) & (PT > %(MinPTXi)s * MeV) & (P > %(MinPXi)s*MeV) & (MAXTREE(TRGHOSTPROB, HASTRACK) < %(Ghost_Prob)s)" %params
	
	_Combine = CombineParticles(name = _name,
								DecayDescriptor = "[Xi- -> Lambda0 pi-]cc",
								 CombinationCut = _combcut,
								 MotherCut = _mothercut
								 )
								 
	return Selection(name + 'L0pi',
					 Algorithm = _Combine,
					 RequiredSelections = [pions, lambdas]
					 )
					 
#################################################################################################
def makeXib2XiGamma(name, xiSel, photons, params):
	_name = name
	
	_combcut = "(ADAMASS('Xi_b-') < %(CombMassWinXib)s * MeV)" %params
	
	_mothercut = "(PT > %(MinPTXib)s * MeV) & (P > %(MinPXib)s*MeV) & (MTDOCACHI2(1) < %(MTDOCACHI2_MAX)s)" %params
	
	_Combine = CombineParticles(name = _name,
								DecayDescriptor = "[Xi_b- -> Xi- gamma]cc",
								CombinationCut = _combcut,
								MotherCut = _mothercut
								)
	_Combine.ParticleCombiners = {'' : 'ParticleAdder'}
	
	return Selection(name + 'XiGamma',
					 Algorithm = _Combine,
					 RequiredSelections = [xiSel, photons]
					 )
	


#################################################################################
#################################################################################
def makeO2L0K(name, kaons, lambdas, params):
	_name = name
	
	_daughter_cut = {'Lambda0' : 'ALL' ,
	              'K-'     : '(MIPCHI2DV(PRIMARY) > %(TRACK_IPCHI2_MIN)s) & (TRCHI2DOF < %(TRCHI2DOF_MAX)s)' %params } 
	              
	_combcut = "(ADAMASS('Omega-') < %(CombMassWinOmega)s * MeV)" %params
	
	_mothercut = "(ADMASS('Omega-') < %(MassWinOmega)s * MeV) & (PT > %(MinPTO)s * MeV) & (P > %(MinPO)s*MeV) & (MAXTREE(TRGHOSTPROB, HASTRACK) < %(Ghost_Prob)s)" %params
	
	_Combine = CombineParticles(name = _name,
								DecayDescriptor = "[Omega- -> Lambda0 K-]cc",
								CombinationCut = _combcut,
								MotherCut = _mothercut
								)
	return Selection(name + 'L0K',
					 Algorithm = _Combine,
					 RequiredSelections = [kaons, lambdas]
					 )
					 
###################################################################################
def makeOb2OGamma(name, oSel, photons, params):
	_name = name

	_combcut = "(ADAMASS('Omega_b-') < %(CombMassWinOmegab)s * MeV)" %params
	
	_mothercut = "(PT > %(MinPTOb)s * MeV) & (P > %(MinPOb)s*MeV) & (MTDOCACHI2(1) < %(MTDOCACHI2_MAX)s)" %params
	
	_Combine = CombineParticles(name = _name,
								DecayDescriptor = "[Omega_b- -> Omega- gamma]cc",
								CombinationCut = _combcut,
								MotherCut = _mothercut
								)
	_Combine.ParticleCombiners = {'' : 'ParticleAdder'}
	
	return Selection(name + 'OGamma',
					 Algorithm = _Combine,
					 RequiredSelections = [oSel, photons]
					 )
					
def makeTISTOSFilter(name,dict_TISTOS):
	specs = dict_TISTOS
	from Configurables import TisTosParticleTagger
	tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
	tisTosFilter.TisTosSpecs = specs
	return tisTosFilter

def tisTosSelection(sel,TISTOSdict):
	tisTosFilter = makeTISTOSFilter(sel.name(),TISTOSdict)
	return Selection(sel.name()+'TISTOS', Algorithm=tisTosFilter, RequiredSelections=[sel])
	 	
		
		
		
		
		
