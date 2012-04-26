__author__ = ['Marcin Chrzaszcz','Katarzyna Senderowska']
__date__ = '06/02/2012'
__version__ = '$Revision: 1.1 $'

__all__ = (
    'B2ChicKConf',
    'makePhi2KKForChic',
    'makeRho2PiPiForChic',
    'makeChic2RhoPhi',
    'makeB2ChicK'
    )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


name = "B2ChicK"


config_params = {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #K parameters
    'K_PT'                   : 650.,
    'K_TRCHI2'               : 3.,
    'K_IPCHI2'               : 6.5,
    #pi parameters
    'pi_PT'                  : 650., 
    'pi_TRCHI2'              : 3.,
    'pi_IPCHI2'              : 6.5,
    #rho (for chi_c decay) parameters
    'rho_chic_VDZ'           : 0.,
    'rho_chic_VCHI2_VDOF'    : 6.,
    #phi (for chi_c decay) parameters
    'phi_chic_VDZ'           : 0.,
    'phi_chic_VCHI2_VDOF'    : 6.,
    #chi_c parameters
    'chic_VDZ'               : 0.,    
    'chic_PT'                : 1000.,
    'chic_IPCHI2'            : 6.,
    'chic_VCHI2_VDOF'        : 7.,
    #B parameters
    'B_VDZ'                 : 0.,
    'B_DIRA'                : 0.999,
    'B_IPCHI2'              : 9.,
    'B_VCHI2_VDOF'          : 9.
    }




class B2ChicKConf(LineBuilder) :
    
    
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
        #rho (for chi_c decay) parameters
        'rho_chic_VDZ',
        'rho_chic_VCHI2_VDOF',      
        #phi (for chi_c decay) parameters
        'phi_chic_VDZ',
        'phi_chic_VCHI2_VDOF',             
        #eta_c parameters
        'chic_VDZ', 
        'chic_PT',
        'chic_IPCHI2',
        'chic_VCHI2_VDOF',                     
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



        self.selPhi2KKForChic = makePhi2KKForChic (
            'PhiForChicFor'+prescaled_name,
            kaons = self.selKaons,
            phi_chic_VDZ = config['phi_chic_VDZ'],
            phi_chic_VCHI2_VDOF = config['phi_chic_VCHI2_VDOF']
            )

        
        self.selRho2PiPiForChic = makeRho2PiPiForChic (
            'RhoForChicFor'+prescaled_name,
            pions = self.selPions,
            rho_chic_VDZ = config['rho_chic_VDZ'],
            rho_chic_VCHI2_VDOF = config['rho_chic_VCHI2_VDOF']
            )


        self.selChic2RhoPhi = makeChic2RhoPhi (
            'ChicFor'+prescaled_name,
            phiForChicSel = self.selPhi2KKForChic, 
            rhoForChicSel = self.selRho2PiPiForChic,
            chic_VDZ = config['chic_VDZ'],
            chic_PT = config['chic_PT'],
            chic_IPCHI2 = config['chic_IPCHI2'],
            chic_VCHI2_VDOF = config['chic_VCHI2_VDOF']
            )

        
        self.selB2ChicK = makeB2ChicK (
            prescaled_name,  
            chicSel = self.selChic2RhoPhi, 
            kaons = self.selKaons,
            B_VDZ = config['B_VDZ'],
            B_DIRA = config['B_DIRA'],
            B_IPCHI2 = config['B_IPCHI2'],
            B_VCHI2_VDOF = config['B_VCHI2_VDOF']
            )


        self.prescaled_line = StrippingLine (
            prescaled_name+"Line",
            prescale = config['Prescale'],
            postscale = config['Postscale'],
            algos = [ self.selB2ChicK ]
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



def makePhi2KKForChic (
    name,
    kaons,
    phi_chic_VDZ,
    phi_chic_VCHI2_VDOF
    ):
    
            
    _phi_chic_combinationCuts = "(AM>400.*MeV) & (AM<3413.*MeV)" %locals()
    _phi_chic_motherCuts = "(BPVVDZ>%(phi_chic_VDZ)s) & (VFASPF(VCHI2/VDOF)<%(phi_chic_VCHI2_VDOF)s)" %locals()
            
    _phi2KK_chic = CombineParticles( 
        DecayDescriptor = "phi(1020) -> K+ K-",
        CombinationCut = _phi_chic_combinationCuts,
        MotherCut = _phi_chic_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _phi2KK_chic,
        RequiredSelections = [kaons]
        )



def makeRho2PiPiForChic(
    name,
    pions,
    rho_chic_VDZ,
    rho_chic_VCHI2_VDOF,
    ):
    

    _rho_chic_combinationCuts = "(AM>400.*MeV) & (AM<3413.*MeV)" %locals()
    _rho_chic_motherCuts = "(BPVVDZ>%(rho_chic_VDZ)s) & (VFASPF(VCHI2/VDOF)<%(rho_chic_VCHI2_VDOF)s)"  %locals()
    
    _rho2pipi_chic = CombineParticles (
        DecayDescriptor = "rho(770)0 -> pi+ pi-" ,
        CombinationCut = _rho_chic_combinationCuts,
        MotherCut = _rho_chic_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _rho2pipi_chic,
        RequiredSelections = [pions]
        )



def makeChic2RhoPhi (
    name,
    phiForChicSel,
    rhoForChicSel,
    chic_VDZ,
    chic_PT,
    chic_IPCHI2,
    chic_VCHI2_VDOF,
    ):
    
    _chic_combinationCuts = "(ADAMASS('chi_c0(1P)')<100.*MeV) & (APT>%(chic_PT)s*MeV)" %locals()
    _chic_motherCuts = "(BPVVDZ>%(chic_VDZ)s) & (MIPCHI2DV(PRIMARY)>%(chic_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(chic_VCHI2_VDOF)s)" %locals()
    
    _chic2phirho = CombineParticles (
        DecayDescriptor = "chi_c0(1P) -> rho(770)0 phi(1020)",  
        CombinationCut = _chic_combinationCuts,
        MotherCut = _chic_motherCuts
        )
            
    return Selection (
        name,
        Algorithm = _chic2phirho,
        RequiredSelections = [phiForChicSel, rhoForChicSel]
        )

        
   
def makeB2ChicK (
    name,
    chicSel,
    kaons,
    B_VDZ,
    B_DIRA,
    B_IPCHI2,
    B_VCHI2_VDOF,
    ):
    
    
    _B_combinationCuts = "(ADAMASS('B+')<300.*MeV)" %locals()
    _B_motherCuts = "(BPVVDZ>%(B_VDZ)s) & (BPVDIRA>%(B_DIRA)s) & (MIPCHI2DV(PRIMARY)<%(B_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(B_VCHI2_VDOF)s)" %locals()
    
    _B = CombineParticles ( 
        DecayDescriptor = "[B0 -> chi_c0(1P) K+]cc",
        CombinationCut = _B_combinationCuts, 
        MotherCut = _B_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _B,
        RequiredSelections = [kaons, chicSel]
        )
  
