__author__ = ['Marcin Chrzaszcz','Katarzyna Senderowska']
__date__ = '06/02/2012'
__version__ = '$Revision: 1.1 $'


__all__ = (
    'B2EtacKstConf',
    'makeKst2KPi',
    'makePhi2KKForEtac',
    'makeRho2PiPiForEtac',
    'makeEtac2RhoPhi',
    'makeB2EtacKst'
    )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder



name = 'B2EtacKst'


config_params = {
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,
    #K parameters
    'K_PT'                    : 650.,
    'K_TRCHI2'                : 3.,
    'K_IPCHI2'                : 6.5,
    #pi parameters
    'pi_PT'                   : 650., 
    'pi_TRCHI2'               : 3.,
    'pi_IPCHI2'               : 6.5,
    #kst parameters
    'kst_VDZ'                 : 0., 
    'kst_PT'                  : 1000., 
    'kst_IPCHI2'              : 6.,
    'kst_VCHI2_VDOF'          : 6.,
    #rho (for eta_c decay) parameters
    'rho_etac_VDZ'            : 0.,
    'rho_etac_VCHI2_VDOF'     : 6.,
    #phi (for eta_c decay) parameters
    'phi_etac_VDZ'            : 0.,
    'phi_etac_VCHI2_VDOF'     : 6.,
    #eta_c parameters
    'etac_VDZ'                : 0.,    
    'etac_PT'                 : 600.,
    'etac_IPCHI2'             : 6., 
    'etac_VCHI2_VDOF'         : 7.,
    #B parameters
    'B_VDZ'                  : 0.,
    'B_DIRA'                 : 0.999,
    'B_IPCHI2'               : 9.,
    'B_VCHI2_VDOF'           : 9.
    }




class B2EtacKstConf(LineBuilder) :
    
    
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
        #kst parameters
        'kst_VDZ',
        'kst_PT',
        'kst_IPCHI2', 
        'kst_VCHI2_VDOF',
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
        #B parameters
        'B_VDZ',
        'B_DIRA',
        'B_IPCHI2',
        'B_VCHI2_VDOF'
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

        
        self.selKst2KPi = makeKst2KPi (
            'KstFor'+prescaled_name,
            kaons = self.selKaons,
            pions = self.selPions,
            kst_VDZ = config['kst_VDZ'],
            kst_PT = config['kst_PT'],
            kst_IPCHI2 = config['kst_IPCHI2'],
            kst_VCHI2_VDOF = config['kst_VCHI2_VDOF']
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
        

        self.selB2EtacKst = makeB2EtacKst (
            prescaled_name,  
            etacSel = self.selEtac2RhoPhi, 
            kstSel = self.selKst2KPi,
            B_VDZ = config['B_VDZ'],
            B_DIRA = config['B_DIRA'],
            B_IPCHI2 = config['B_IPCHI2'],
            B_VCHI2_VDOF = config['B_VCHI2_VDOF']
            )
        

        self.prescaled_line = StrippingLine (
            prescaled_name+"Line",
            prescale = config['Prescale'],
            postscale = config['Postscale'],
            algos = [ self.selB2EtacKst ]
            )

        
        self.registerLine(self.prescaled_line)
        
        
        
	
def makeKaons (
    name,
    K_PT,
    K_TRCHI2,
    K_IPCHI2
    ):

    _code = "(PT>%(K_PT)s) & (TRCHI2DOF<%(K_TRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(K_IPCHI2)s) & (PIDK-PIDpi>3)" %locals()
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

    _code = "(PT>%(pi_PT)s) & (TRCHI2DOF<%(pi_TRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(pi_IPCHI2)s) & (PIDpi-PIDK>3)" %locals() 
    _pionsFilter = FilterDesktop(Code = _code)
    _stdPions = DataOnDemand(Location = "Phys/StdTightPions/Particles")

    return Selection (
        name,
        Algorithm = _pionsFilter,
        RequiredSelections = [_stdPions]
        )

	
	
        
def makeKst2KPi(
    name,
    kaons,
    pions,
    kst_VDZ,
    kst_PT,
    kst_IPCHI2,
    kst_VCHI2_VDOF
    ):
    

    _kst_combinationCuts = "(ADAMASS('K*(892)0')<150*MeV) & (APT>%(kst_PT)s*MeV)" %locals()
    _kst_motherCuts = "(BPVVDZ>%(kst_VDZ)s) & (MIPCHI2DV(PRIMARY)>%(kst_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(kst_VCHI2_VDOF)s)" %locals()
    
    _kst2KK = CombineParticles (
        DecayDescriptor = "[K*(892)0 -> K+ pi-]cc",
        CombinationCut = _kst_combinationCuts,
        MotherCut = _kst_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _kst2KK,
        RequiredSelections = [kaons, pions]
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
        DecayDescriptor = "phi(1020) -> K+ K-",
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
        DecayDescriptor = "rho(770)0 -> pi+ pi-" ,
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



def makeB2EtacKst (
    name,
    etacSel,
    kstSel,
    B_VDZ,
    B_DIRA,
    B_IPCHI2,
    B_VCHI2_VDOF
    ):
    
    _B_combinationCuts = "(ADAMASS('B0')<300.*MeV)" %locals()
    _B_motherCuts = "(BPVVDZ>%(B_VDZ)s) & (BPVDIRA>%(B_DIRA)s) & (MIPCHI2DV(PRIMARY)<%(B_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(B_VCHI2_VDOF)s)" %locals()
    
    _B = CombineParticles (  
        DecayDescriptor = "B0 -> eta_c(1S) K*(892)0",
         CombinationCut = _B_combinationCuts, 
        MotherCut = _B_motherCuts
        )

    return Selection (
        name,
        Algorithm = _B,
        RequiredSelections = [kstSel, etacSel]
        )



