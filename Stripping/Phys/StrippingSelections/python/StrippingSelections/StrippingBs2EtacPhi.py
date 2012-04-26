__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '27/02/2011'
__version__ = '$Revision: 1.1 $'


__all__ = (
    'Bs2EtacPhiConf',
    'makeTightKaons',
    'makeTightPions',
    'makePhi2KK',
    'makePhi2KKForEtac',
    'makeRho2PiPiForEtac',
    'makeEtac2RhoPhi',
    'makeBs2EtacPhi'
    )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder



name = 'Bs2EtacPhi'


config_params = {
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,
    #K parameters
    'K_PT'                    : 650.,
    'K_TRCHI2'                : 3.,
    'K_IPCHI2'                : 6.75,
    #pi parameters
    'pi_PT'                   : 650., 
    'pi_TRCHI2'               : 3.,
    'pi_IPCHI2'               : 6.75,
    #phi parameters
    'phi_VDZ'                 : 0., 
    'phi_PT'                  : 1000., 
    'phi_IPCHI2'              : 6.,
    'phi_VCHI2_VDOF'          : 6.,
    #rho (for eta_c decay) parameters
    'rho_etac_VDZ'            : 0.,
    'rho_etac_VCHI2_VDOF'     : 6.,
    #phi (for eta_c decay) parameters
    'phi_etac_VDZ'            : 0.,
    'phi_etac_VCHI2_VDOF'     : 6.,
    #eta_c parameters
    'etac_VDZ'                : 0.,    
    'etac_PT'                 : 1000.,
    'etac_IPCHI2'             : 6., 
    'etac_VCHI2_VDOF'         : 7.,
    #Bs parameters
    'Bs_VDZ'                  : 0.,
    'Bs_DIRA'                 : 0.999,
    'Bs_IPCHI2'               : 9.,
    'Bs_VCHI2_VDOF'           : 9.
    }




class Bs2EtacPhiConf(LineBuilder) :
    
    
    __configuration_keys__ = (
        'Prescale',
        'Postscale',
        #K parameters
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
        #rho (for eta_c decay) parameters
        'rho_etac_VDZ',
        'rho_etac_VCHI2_VDOF',    
        #phi (for eta_c decay) parameters
        'phi_etac_VDZ',
        'phi_etac_VCHI2_VDOF',             
        #eta_c parameters
        'etac_VDZ', 
        'etac_PT',
        'etac_IPCHI2',
        'etac_VCHI2_VDOF',                     
        #Bs parameters
        'Bs_VDZ',
        'Bs_DIRA',
        'Bs_IPCHI2',
        'Bs_VCHI2_VDOF'
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
        
        
        self.selPhi2KKForEtac = makePhi2KKForEtac (
            'PhiForEtacFor'+prescaled_name,
	    kaons = self.selKaons,
            phi_etac_VDZ = config['phi_etac_VDZ'],
            phi_etac_VCHI2_VDOF = config['phi_etac_VCHI2_VDOF']
            )
        
        
        self.selRho2PiPiForEtac = makeRho2PiPiForEtac (
            'RhoForEtacFor'+prescaled_name,
	    pions = self.selPions,
            rho_etac_VDZ = config['rho_etac_VDZ'],
            rho_etac_VCHI2_VDOF = config['rho_etac_VCHI2_VDOF']
            )
        
        
        self.selEtac2RhoPhi = makeEtac2RhoPhi (
            'EtacFor'+prescaled_name,
            phiForEtacSel = self.selPhi2KKForEtac, 
            rhoForEtacSel = self.selRho2PiPiForEtac,
            etac_VDZ = config['etac_VDZ'],
            etac_PT = config['etac_PT'],
            etac_IPCHI2 = config['etac_IPCHI2'],
            etac_VCHI2_VDOF = config['etac_VCHI2_VDOF']
            )
        

        self.selBs2EtacPhi = makeBs2EtacPhi (
            prescaled_name,  
            etacSel = self.selEtac2RhoPhi, 
            phiSel = self.selPhi2KK,
            Bs_VDZ = config['Bs_VDZ'],
            Bs_DIRA = config['Bs_DIRA'],
            Bs_IPCHI2 = config['Bs_IPCHI2'],
            Bs_VCHI2_VDOF = config['Bs_VCHI2_VDOF']
            )
        

        self.prescaled_line = StrippingLine (
            prescaled_name+"Line",
            prescale = config['Prescale'],
            postscale = config['Postscale'],
            algos = [ self.selBs2EtacPhi ]
            )

        
        self.registerLine(self.prescaled_line)
        
        
        
	
def makeKaons (
    name,
    K_PT,
    K_TRCHI2,
    K_IPCHI2
    ):

    _code = "(PT>%(K_PT)s) & (TRCHI2DOF<%(K_TRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(K_IPCHI2)s) & (PIDK-PIDpi>4)" %locals()
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

    _code = "(PT>%(pi_PT)s) & (TRCHI2DOF<%(pi_TRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(pi_IPCHI2)s) & (PIDpi-PIDK>4)" %locals() 
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
    

    _phi_combinationCuts = "(ADAMASS('phi(1020)')<50*MeV) & (APT>%(phi_PT)s*MeV)" %locals()
    _phi_motherCuts = "(BPVVDZ>%(phi_VDZ)s) & (MIPCHI2DV(PRIMARY)>%(phi_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(phi_VCHI2_VDOF)s)" %locals()
    
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



def makePhi2KKForEtac (
    name,
    kaons,
    phi_etac_VDZ,
    phi_etac_VCHI2_VDOF
    ):
    

    _phiForEtac_combinationCuts = "(AM>400.*MeV) & (AM<3413.*MeV)" %locals()
    _phiForEtac_motherCuts = "(BPVVDZ>%(phi_etac_VDZ)s) & ((VFASPF(VCHI2/VDOF)<%(phi_etac_VCHI2_VDOF)s))" %locals()
    
    _phi2KKForEtac = CombineParticles (
        DecayDescriptor = "[phi(1020) -> K+ K-]cc",
        CombinationCut = _phiForEtac_combinationCuts,
        MotherCut = _phiForEtac_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _phi2KKForEtac,
        RequiredSelections = [kaons]
        )



def makeRho2PiPiForEtac (
    name,
    pions,	
    rho_etac_VDZ,
    rho_etac_VCHI2_VDOF
    ):
    

    _rho_etac_combinationCuts = "(AM>400.*MeV) & (AM<3413.*MeV)" %locals()
    _rho_etac_motherCuts = "(BPVVDZ>%(rho_etac_VDZ)s) & (VFASPF(VCHI2/VDOF)<%(rho_etac_VCHI2_VDOF)s)"  %locals()
    
    _rho2pipi_etac = CombineParticles (
        DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc" ,
        CombinationCut = _rho_etac_combinationCuts,
        MotherCut = _rho_etac_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _rho2pipi_etac,
        RequiredSelections = [pions]
        )



def makeEtac2RhoPhi (
    name,
    phiForEtacSel,
    rhoForEtacSel,
    etac_VDZ,
    etac_PT,
    etac_IPCHI2,
    etac_VCHI2_VDOF
    ):
    
    _etac_combinationCuts = "(ADAMASS('eta_c(1S)')<100.*MeV) & (APT>%(etac_PT)s*MeV)" %locals()
    _etac_motherCuts = "(BPVVDZ>%(etac_VDZ)s) & (MIPCHI2DV(PRIMARY)>%(etac_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(etac_VCHI2_VDOF)s)" %locals()
    
    _etac2phirho = CombineParticles (
        DecayDescriptor = "eta_c(1S) -> rho(770)0 phi(1020)",
        CombinationCut = _etac_combinationCuts,
        MotherCut = _etac_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _etac2phirho,
        RequiredSelections = [phiForEtacSel, rhoForEtacSel]
        )



def makeBs2EtacPhi (
    name,
    etacSel,
    phiSel,
    Bs_VDZ,
    Bs_DIRA,
    Bs_IPCHI2,
    Bs_VCHI2_VDOF
    ):
    
    _Bs_combinationCuts = "(ADAMASS('B_s0')<300.*MeV)" %locals()
    _Bs_motherCuts = "(BPVVDZ>%(Bs_VDZ)s) & (BPVDIRA>%(Bs_DIRA)s) & (MIPCHI2DV(PRIMARY)<%(Bs_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(Bs_VCHI2_VDOF)s)" %locals()
    
    _Bs = CombineParticles (  
        DecayDescriptor = "B_s0 -> eta_c(1S) phi(1020)",
        CombinationCut = _Bs_combinationCuts, 
        MotherCut = _Bs_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _Bs,
        RequiredSelections = [phiSel, etacSel]
        )




