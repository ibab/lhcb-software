__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '27/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = (
    'Bs2ChicPhi_ChicPiPiPiPiConf',
    'makeTightKaons',
    'makeTightPions',
    'makePhi2KK',
    'makeRho2PiPiForChic',
    'makeChic2RhoRho',
    'makeBs2ChicPhi_Chic2PiPiPiPi'
    )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


default_name = "Bs2ChicPhi_Chic2PiPiPiPi"


config_params = {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #phi parameters
    'K_PT'                   : 650.,
    'K_TRCHI2'               : 3.,
    'K_IPCHI2'               : 6.5,
    #pi parameters
    'pi_PT'                  : 650., 
    'pi_TRCHI2'              : 3.,
    'pi_IPCHI2'              : 6.5,
    #phi parameters
    'phi_VDZ'                : 0., 
    'phi_PT'                 : 1000.,  
    'phi_IPCHI2'             : 9., 
    'phi_VCHI2_VDOF'         : 6.,
    #rho (for chi_c decay) parameters
    'rho_chic_VDZ'           : 0.,
    'rho_chic_VCHI2_VDOF'    : 6.,
    #chi_c parameters
    'chic_VDZ'               : 0.,    
    'chic_PT'                : 1000.,
    'chic_IPCHI2'            : 6.,
    'chic_VCHI2_VDOF'        : 7.,
    #Bs parameters
    'Bs_VDZ'                  : 0.,
    'Bs_DIRA'                 : 0.999,
    'Bs_IPCHI2'               : 9.,
    'Bs_VCHI2_VDOF'           : 9.
    }




class Bs2ChicPhi_Chic2PiPiPiPiConf(LineBuilder) :
    
    
    __configuration_keys__ = (
        #K (for phi decay) parameters
        'K_PT',
        'K_TRCHI2',
        'K_IPCHI2',
        #pi parameters	
        'pi_PT',
        'pi_TRCHI2',        
        'pi_IPCHI2', 
        #phi parameters
        'phi_VDZ',        
        'phi_PT',
        'phi_IPCHI2',
        'phi_VCHI2_VDOF',
        #rho (for chi_c decay) parameters
        'rho_chic_VDZ',
        'rho_chic_VCHI2_VDOF',                
        #chi_c parameters
        'chic_VDZ', 
        'chic_PT',
        'chic_IPCHI2',
        'chic_VCHI2_VDOF',                     
        #Bs parameters
        'Bs_VDZ',
        'Bs_DIRA',
        'Bs_IPCHI2',
        'Bs_VCHI2_VDOF',   
        'Prescale',
        'Postscale',
        )
    
    

    def __init__(self, name, config) :

        
        LineBuilder.__init__(self, name, config)

        
        prescaled_name = name + 'Nominal'
	

	
	self.selKaons = makeKaons (
            'KaonFor'+prescaled_name,
            K_PT = config['K_PT'],
            K_TRCHI2 = config['K_TRCHI2'],
            K_IPCHI2 = config['K_IPCHI2']
            )
	    
	    
	self.selPions = makePions (
            'PionFor'+prescaled_name,
            pi_PT = config['pi_PT'],
            pi_TRCHI2 = config['pi_TRCHI2'],
            pi_IPCHI2 = config['pi_IPCHI2']
            )
        

        self.selPhi2KK = makePhi2KK (
            'PhiFor'+prescaled_name,
	    kaons = self.selKaons,	
            phi_VDZ = config['phi_VDZ'],
            phi_PT = config['phi_PT'],
            phi_IPCHI2 = config['phi_IPCHI2'],
            phi_VCHI2_VDOF = config['phi_VCHI2_VDOF']
            )
        
        
        self.selRho2PiPiForChic = makeRho2PiPiForChic (
            'RhoForChicFor'+prescaled_name,
	    pions = self.selPions,
            rho_chic_VDZ = config['rho_chic_VDZ'],
            rho_chic_VCHI2_VDOF = config['rho_chic_VCHI2_VDOF']
            )

        
        self.selChic2RhoRho = makeChic2RhoRho (
            'ChicFor'+prescaled_name, 
            rhoForChicSel = self.selRho2PiPiForChic,
            chic_VDZ = config['chic_VDZ'],
            chic_PT = config['chic_PT'],
            chic_IPCHI2 = config['chic_IPCHI2'],
            chic_VCHI2_VDOF = config['chic_VCHI2_VDOF']
            )
        
        
        self.selBs2ChicPhi_Chic2PiPiPiPi = makeBs2ChicPhi_Chic2PiPiPiPi (
            prescaled_name,
            phiSel = self.selPhi2KK,
            chicSel = self.selChic2RhoRho, 
            Bs_VDZ = config['Bs_VDZ'],
            Bs_DIRA = config['Bs_DIRA'],
            Bs_IPCHI2 = config['Bs_IPCHI2'],
            Bs_VCHI2_VDOF = config['Bs_VCHI2_VDOF']
            )
        
        
        self.prescaled_line = StrippingLine (
            prescaled_name+"Line",
            prescale = config['Prescale'],
            postscale = config['Postscale'],
            algos = [ self.selBs2ChicPhi_Chic2PiPiPiPi ]
            )
        

        self.registerLine(self.prescaled_line)
        



def makeKaons (
    name,
    K_PT,
    K_TRCHI2,
    K_IPCHI2
    ):

    _code = "(PT>%(K_PT)s) & (TRCHI2DOF<%(K_TRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(K_IPCHI2)s)" %locals()
    _kaonsFilter = FilterDesktop(Code = _code)
    _stdKaons = DataOnDemand(Location = "Phys/StdTightKaons/Particles")

    return Selection (
        name,
        Algorithm = _kaonsFilter,
        RequiredSelections = [_stdKaons]
        )



def makePions(
    name,
    pi_PT,
    pi_TRCHI2,
    pi_IPCHI2
    ):

    _code = "(PT>%(pi_PT)s) & (TRCHI2DOF<%(pi_TRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(pi_IPCHI2)s) & (PIDpi-PIDK>0)" %locals() 
    _pionsFilter = FilterDesktop(Code = _code)
    _stdPions = DataOnDemand(Location = "Phys/StdTightPions/Particles")

    return Selection (
        name,
        Algorithm = _pionsFilter,
        RequiredSelections = [_stdPions]
        )



def makePhi2KK (
    name,
    kaons,
    phi_VDZ,
    phi_PT,
    phi_IPCHI2,
    phi_VCHI2_VDOF
    ):


    _phi_combinationCuts = "(ADAMASS('phi(1020)')<50*MeV) & (APT>%(phi_PT)s)" %locals()
    _phi_motherCuts = "(BPVVDZ>%(phi_VDZ)s) & (VFASPF(VCHI2/VDOF)<%(phi_VCHI2_VDOF)s)" %locals()

    _phi2KK = CombineParticles (
        DecayDescriptor = "[phi(1020) -> K+ K-]cc",
        CombinationCut = _phi_combinationCuts,
        MotherCut = _phi_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _phi2KK,
        RequiredSelections = [kaons]
        )




def makeRho2PiPiForChic (
    name,
    pions,
    rho_chic_VDZ,
    rho_chic_VCHI2_VDOF,
    ):
    

    _rho_chic_combinationCuts = "(AM>400.*MeV) & (AM<3413.*MeV)" %locals()
    _rho_chic_motherCuts = "(BPVVDZ>%(rho_chic_VDZ)s) & (VFASPF(VCHI2/VDOF)<%(rho_chic_VCHI2_VDOF)s)"  %locals()
    
    _rho2pipi_chic = CombineParticles (
        DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc" ,
        CombinationCut = _rho_chic_combinationCuts,
        MotherCut = _rho_chic_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _rho2pipi_chic,
        RequiredSelections = [pions]
        )



def makeChic2RhoRho (
    name,
    rhoForChicSel,
    chic_VDZ,
    chic_PT,
    chic_IPCHI2,
    chic_VCHI2_VDOF,
    ):
    
    _chic_combinationCuts = "(ADAMASS('chi_c0(1P)')<100.*MeV) & (APT>%(chic_PT)s)" %locals()
    _chic_motherCuts = "(BPVVDZ>%(chic_VDZ)s) & (MIPCHI2DV(PRIMARY)>%(chic_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(chic_VCHI2_VDOF)s)" %locals()
    
    _chic2phirho = CombineParticles (
        DecayDescriptor = "chi_c0(1P) -> rho(770)0 rho(770)0",
        CombinationCut = _chic_combinationCuts,
        MotherCut = _chic_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _chic2phirho,
        RequiredSelections = [rhoForChicSel]
        )

        

def makeBs2ChicPhi_Chic2PiPiPiPi (
    name,
    chicSel,
    phiSel,
    Bs_VDZ,
    Bs_DIRA,
    Bs_IPCHI2,
    Bs_VCHI2_VDOF,
    ):
    
    
    _Bs_combinationCuts = "(ADAMASS('B_s0')<300.*MeV)" %locals()
    _Bs_motherCuts = "(BPVVDZ>%(Bs_VDZ)s) & (BPVDIRA>%(Bs_DIRA)s) & (MIPCHI2DV(PRIMARY)<%(Bs_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(Bs_VCHI2_VDOF)s)" %locals()
    
    _Bs = CombineParticles( 
        DecayDescriptor = "B_s0 -> chi_c0(1P) phi(1020)",
        CombinationCut = _Bs_combinationCuts, 
        MotherCut = _Bs_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _Bs,
        RequiredSelections = [phiSel, chicSel]
        )
   
