'''
Betac -> phi p lines

Adapted to current stripping framework by Y. Shcheglov.
'''

__author__ = ['Yury Shcheglov', 'Alexey Dzyuba']
__date__ = '12/04/2012 modified 21/05/2012  by NRS  modified 4/12/2012  by Y. Shcheglov'
__version__ = '$Revision: 2.0 $'

__all__ = ( 'StrippingBetac2PhiPConf',
            'makePhi2KK', 
            'makeBetac2PhiP',
            'default_config' )


from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from GaudiKernel.SystemOfUnits import MeV, mm, ns
from LHCbKernel.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsProtons 
        
class StrippingBetac2PhiPConf(LineBuilder): 

    __configuration_keys__ = (   
     'Daug_TRCHI2DOF_MAX'
    , 'Daug_P_MIN' 
    , 'Daug_PT_MIN'
    , 'Daug_MIPDV'              
    , 'Proton_PIDpi_MIN'
    , 'Proton_PIDK_MIN'  
    , 'Phi_WIN'
    , 'Phi_PT'              
    , 'Betac_AM_MIN'
    , 'Betac_AM_MAX'
    , 'Betac_BPVDIRA_MIN'
    , 'Betac_VCHI2VDOF_MAX'
    , 'Betac_BPVLTIME_MIN'
    , 'Betac_BPVLTIME_MAX'
    , 'Hlt2IncPhiDecision'   
    , 'PrescaleBetac2PhiP'
    , 'PostscaleBetac2PhiP'
                             )

    def __init__(self, name, config) : 

        LineBuilder.__init__(self, name, config)

        phi2KK_name        = name + 'Phi2KK'
        Betac2PhiP_name = name + 'Betac2PhiP'

        self.inProtons = StdNoPIDsProtons 
        
        self.selPhi2KK =  makePhi2KK( name = phi2KK_name
                            , Phi_Win      = config['Phi_WIN'] 
                            , Phi_PT       = config['Phi_PT']  
                           )

        self.selBetac2PhiP = makeBetac2PhiP( name = Betac2PhiP_name 
                                , inputSel             = [ self.selPhi2KK, self.inProtons]
                                , Daug_P_MIN           = config['Daug_P_MIN']
                                , Daug_PT_MIN          = config['Daug_PT_MIN']
                                , Daug_MIPDV           = config['Daug_MIPDV']
                                , Proton_TRCHI2DOF_MAX = config['Daug_TRCHI2DOF_MAX']
                                , Proton_PIDpi_MIN     = config['Proton_PIDpi_MIN']
                                , Proton_PIDK_MIN      = config['Proton_PIDK_MIN']
                                , Comb_AM_MIN          = config['Betac_AM_MIN']
                                , Comb_AM_MAX          = config['Betac_AM_MAX']
                                , Betac_BPVDIRA_MIN   = config['Betac_BPVDIRA_MIN']
                                , Betac_BPVLTIME_MIN  = config['Betac_BPVLTIME_MIN']
                                , Betac_BPVLTIME_MAX  = config['Betac_BPVLTIME_MAX']
                                , Betac_VCHI2VDOF_MAX = config['Betac_VCHI2VDOF_MAX']
                          )
        
        self.line_Betac2PhiP = StrippingLine( Betac2PhiP_name + 'Line', 
                                           HLT                   = config['Hlt2IncPhiDecision'], 
                                           prescale              = config['PrescaleBetac2PhiP'],
                                           postscale             = config['PostscaleBetac2PhiP'],
                                           algos                 = [ self.selBetac2PhiP ]
                                         )
        self.registerLine(self.line_Betac2PhiP)

   

def makePhi2KK(   name
                , Phi_Win
                , Phi_PT
               ) : 

    _code      = "(ADMASS('phi(1020)') < %(Phi_Win)s) & (PT > %(Phi_PT)s)" % locals()

    _phi       = FilterDesktop(Code=_code)

    _stdPhi2KK = DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles")

    return Selection(name, 
                     Algorithm=_phi, 
                     RequiredSelections=[_stdPhi2KK]
                     )



def makeBetac2PhiP( name
                 , inputSel
                 , Daug_P_MIN
                 , Daug_PT_MIN
                 , Daug_MIPDV
                 , Proton_TRCHI2DOF_MAX
                 , Proton_PIDpi_MIN
                 , Proton_PIDK_MIN 
                 , Comb_AM_MIN
                 , Comb_AM_MAX
                 , Betac_BPVDIRA_MIN   
                 , Betac_BPVLTIME_MIN
                 , Betac_BPVLTIME_MAX
                 , Betac_VCHI2VDOF_MAX
                 , decDescriptors = [ "[Lambda_c+ -> p+ phi(1020)]cc" ]
                ): 


    phiCuts =  "(ALL)"

    pCuts =    "(P >  %(Daug_P_MIN)s) & (PT > %(Daug_PT_MIN)s) & (TRCHI2DOF < %(Proton_TRCHI2DOF_MAX)s)" \
               "& (MIPDV(PRIMARY) > %(Daug_MIPDV)s)" \
               "& ((PIDp-PIDpi) > %(Proton_PIDpi_MIN)s)" \
               "& ((PIDp-PIDK)  > %(Proton_PIDK_MIN)s)" % locals()    


    combCuts = "(AM < %(Comb_AM_MAX)s)" \
               "& (AM > %(Comb_AM_MIN)s)" % locals()

   
    
    dCuts =    "(VFASPF(VCHI2/VDOF) < %(Betac_VCHI2VDOF_MAX)s) " \
               "& (BPVDIRA > %(Betac_BPVDIRA_MIN)s) " \
               "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > %(Betac_BPVLTIME_MIN)s) " \
               "& (BPVLTIME('PropertimeFitter/properTime:PUBLIC') < %(Betac_BPVLTIME_MAX)s)" % locals()
     

    _Betacplus = CombineParticles(
          DecayDescriptors = decDescriptors
        , DaughtersCuts    = {"p+" : pCuts, "phi(1020)" : phiCuts }
        , CombinationCut   = combCuts
        , MotherCut        = dCuts
    )

    return Selection( name,
                      Algorithm          = _Betacplus,
                      RequiredSelections = inputSel
                    )



default_config = {
                    'Daug_TRCHI2DOF_MAX'        : 10.     
                  , 'Daug_P_MIN'                : 1200.0*MeV 
                  , 'Daug_PT_MIN'               : 400.0*MeV
                  , 'Daug_MIPDV'                : 0.05      # mm
		  , 'Proton_PIDpi_MIN'          : 5.0
                  , 'Proton_PIDK_MIN'           : 5.0 
                  , 'Phi_WIN'                   : 20.0 * MeV
                  , 'Phi_PT'                    : 1700.0*MeV         
                  , 'Betac_AM_MIN'              : 1950.0 * MeV 
                  , 'Betac_AM_MAX'              : 2800.0 * MeV 
                  , 'Betac_BPVDIRA_MIN'         : 0.9999    
                  , 'Betac_VCHI2VDOF_MAX'       : 25.0   
                  , 'Betac_BPVLTIME_MIN'        : 0.0 * ns
                  , 'Betac_BPVLTIME_MAX'        : 0.06 * ns
                  , 'Hlt2IncPhiDecision'        : "HLT_PASS('Hlt2IncPhiDecision')"
                  , 'PrescaleBetac2PhiP'        : 1.0
                  , 'PostscaleBetac2PhiP'       : 1.0
                 }


 
