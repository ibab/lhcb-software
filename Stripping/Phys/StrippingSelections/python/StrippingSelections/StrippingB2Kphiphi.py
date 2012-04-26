
__author__ = ['M.Chrzaszcz']
__date__ = '27/06/2011'
__version__ = '$Revision: 1.1 $'

###############################################################################
#####################     TUTAJ KAZDA CZASTKA BEDZIE REKONSTRUOWAC JAREK NAPISAL DWA ALGORYTMY TAKIE SAME =/ ########

__all__ = (
    'makePhi2KK',
    'B2KPhiPhiConf',
    'makeB2KPhiPhi'   
    )


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder



#name = 'Bs2DsDs'
name = 'B2KPhiPhi'

### TUTAJ SA NIE NAZWY ZMIENNYCH TYLKO WARTOSCI, POZNIEJ URZYJESZ ZMIENNYCH
#    WARTOSCI DLA PLIKU Kphiphi_histos81kk.py
config_params = {
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,
    #K parameters
    'K_PT'                    : 400.,          # tutaj sie zgadza z moim davinci
    'K_IPCHI2'                : 6.25,
    'K_TRCHI2'                : 3.3,       # tautaj doda??em ciecie na track_chi2
    
    #phi1 parameters
    #tylko raz chyba mo??na dac ciecia wiec wybieram ciasniejsze
    'phi_VDZ'                 : 0.,
    'phi_PT'                  : 300,             ## tutaj bez zmian
    'phi_IPCHI2'              : 6.,
    'phi_VCHI2_VDOF'          : 16.,
    'phi_MM_max'              : 1060., # delta masy(ta zmiana)
    'phi_MM_min'              : 980.,

 
    #B parameters
    'B_ADAMASS'              : 500.,
    'B_DIRA'                 : 0.999, 
    'B_IPCHI2'               : 12.25,   # tutaj w offline selection jest 25
    'B_VCHI2_VDOF'           : 16.,
    'B_LTCHI2'               : 64.,
    'B_VDZ'                  : 0.

    }
# Podsumowanie jest dodane ostrzejsze Ci??cie na track <5, teraz chyba b??dzie trzeba napisa?? od pocz??tku DAVINCI i TMVA



class B2KPhiPhiConf(LineBuilder) :
    
    
    __configuration_keys__ = (
        'Prescale',
        'Postscale',
        #K parameters
    	'K_PT',                   
   	'K_TRCHI2',               
   	'K_IPCHI2',                          
        #phi1 parameters
        'phi_VDZ',
        'phi_PT',
        'phi_IPCHI2', 
        'phi_VCHI2_VDOF',
	'phi_MM_max',
	'phi_MM_min',
        #B parameters
        'B_ADAMASS',
        'B_DIRA',
        'B_IPCHI2',
        'B_VCHI2_VDOF',
        'B_LTCHI2',
        'B_VDZ'
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
      
        self.selPhi2KK = makePhi2KK (
            'PhiFor'+prescaled_name,
            kaons = self.selKaons,
            phi_VDZ = config['phi_VDZ'],
            phi_MM_max = config['phi_MM_max'],
	    phi_MM_min = config['phi_MM_min'],
            phi_PT = config['phi_PT'],
            phi_IPCHI2 = config['phi_IPCHI2'],
            phi_VCHI2_VDOF = config['phi_VCHI2_VDOF']
            )
        
 
        self.selB2KPhiPhi = makeB2KPhiPhi (
            prescaled_name,
            kaons = self.selKaons,
            phi2kk = self.selPhi2KK,
            B_ADAMASS = config['B_ADAMASS'],
            B_DIRA = config['B_DIRA'],
            B_IPCHI2 = config['B_IPCHI2'],
            B_VCHI2_VDOF = config['B_VCHI2_VDOF'],
	    B_LTCHI2 = config['B_LTCHI2'],
            B_VDZ = config['B_VDZ']
            )
        

        self.prescaled_line = StrippingLine (
            prescaled_name+"Line",
            prescale = config['Prescale'],
            postscale = config['Postscale'],
            algos = [ self.selB2KPhiPhi ]
            )

        
        self.registerLine(self.prescaled_line)
        
        
        
	
def makeKaons (
    name,
    K_PT,
    K_TRCHI2,
    K_IPCHI2
    ):

    _code = "(PIDK  - PIDpi > -2.  )  &  (PT>%(K_PT)s*MeV) & (TRCHI2DOF<%(K_TRCHI2)s) & (MIPCHI2DV(PRIMARY)>%(K_IPCHI2)s)" %locals()
    _kaonsFilter = FilterDesktop(Code = _code)
    _stdKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")

    return Selection (
        name,
        Algorithm = _kaonsFilter,
        RequiredSelections = [_stdKaons]
        )

	
        
def makePhi2KK (
    name,
    kaons,
    phi_MM_min,
    phi_MM_max,
    phi_VDZ,
    phi_PT,
    phi_IPCHI2,
    phi_VCHI2_VDOF
    ):
    

    _phi1_combinationCuts = "(APT>%(phi_PT)s*MeV) & (AM<%(phi_MM_max)s*MeV)" %locals()
    _phi1_motherCuts = "(BPVVDZ>%(phi_VDZ)s) & (MIPCHI2DV(PRIMARY)>%(phi_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(phi_VCHI2_VDOF)s)" %locals()
    
    _phi2KK_1 = CombineParticles (
        DecayDescriptor = "[phi(1020) -> K+ K-]cc",
        CombinationCut = _phi1_combinationCuts,
        MotherCut = _phi1_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _phi2KK_1,
        RequiredSelections = [kaons]
        )




def makeB2KPhiPhi (
    name,
    kaons,
    phi2kk,
    B_ADAMASS,
    B_VDZ,
    B_DIRA,
    B_IPCHI2,
    B_VCHI2_VDOF,
    B_LTCHI2
    ):
    
    _B_combinationCuts = "(ADAMASS('B+')<%(B_ADAMASS)s*MeV)" %locals()
    _B_motherCuts = "(BPVVDZ>%(B_VDZ)s) & (BPVDIRA>%(B_DIRA)s) & (MIPCHI2DV(PRIMARY)<%(B_IPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(B_VCHI2_VDOF)s)" %locals()
    
    _B = CombineParticles (  
        DecayDescriptor = "[B+ -> K+ phi(1020) phi(1020)]cc",
        CombinationCut = _B_combinationCuts, 
        MotherCut = _B_motherCuts
        )
    
    return Selection (
        name,
        Algorithm = _B,
        RequiredSelections = [kaons, phi2kk]
        )



 
        



