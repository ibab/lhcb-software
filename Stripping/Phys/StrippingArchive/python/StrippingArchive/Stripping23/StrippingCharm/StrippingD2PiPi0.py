'''
StrippingD2Pi0HConf      : D+ -> h+ (pi0 -> e+ e- gamma) prompt and
                           D*+ -> (D0 -> pi0 pi0) pi+, with pi0 -> e+ e- gamma
StrippingD2EtaHConf      : D+ -> h+ (eta -> pi+ pi- gamma, pi+ pi- pi0) prompt
StrippingD2EtaPrimeHConf : D+ -> h+ (eta_prime -> pi+ pi- gamma, pi+ pi- eta) prompt
StrippingD2PhiHConf      : D+ -> h+ (phi/omega -> pi+ pi- pi0) prompt
StrippingD2EtaEEGHConf   : D+ -> h+ (eta -> e+ e- gamma) prompt
StrippingDst2PiD0EEGConf : D*+ -> (D0 -> e+ e- gamma) pi+
'''
                                                               
__author__ = ['Simone Stracka simone.stracka@unipi.it']
__date__ = '15/05/2015'
__version__ = '$Revision: 192881 $'

__all__ = (
    'default_config',
    'StrippingD2Pi0HConf',
    'StrippingD2EtaEEGHConf',
    'StrippingDst2PiD0EEGConf',
    'StrippingD2EtaHConf',
    'StrippingD2EtaPrimeHConf',
    'StrippingD2PhiHConf',
    
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mrad, picosecond

from StandardParticles import (
    StdAllNoPIDsKaons,
    StdAllNoPIDsPions,
    StdLooseMergedPi0,
    StdLooseResolvedPi0,
    StdLooseResolvedEta,
    StdLooseAllPhotons,
    StdDiElectronFromTracks
)

from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

#from Configurables import CombineParticles, FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, ConjugateNeutralPID, DaVinci__N3BodyDecays

default_config = {
    'D2Pi0H' : {
          'WGs': ['Charm'],
          'BUILDERTYPE': 'StrippingD2Pi0HConf',
          'STREAMS': ['Charm'],
          'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Bachelor_IPCHI2_MIN'      : 25.0,        
                # Minimum PT for charged D daughters 
                'Bachelor_PT_MIN'          : 350  *MeV,   
                # Minimum momentum for charged D daughters 
                'Bachelor_P_MIN'           : 1000 *MeV,
                # Bachelor particle PID requirement 
                'Bachelor_PIDK_MIN'        : -999.0, # not used if < -10
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 25.0,     
                # Minimum PT for charged D daughters 
                'Daug_PT_MIN'          : 350  *MeV,   # 600 --> 300
                # Minimum momentum for charged D daughters 
                'Daug_P_MIN'           : 1000 *MeV,
                # Maximum momentum for charged D daughters 
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters 
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters 
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 5,    
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 350  *MeV,
                # Maximum mass for dielectron                
                'DiElectron_Mass_MAX'       : 210  *MeV,
                # Minimum PT for dielectron                
                'DiElectron_PT_MIN'       : 0  *MeV, 
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 70   *MeV, 
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 210  *MeV, 
                # Minimum D PT
                'D_PT_Min'           : 2000 *MeV, 
                # Minimum D mass
                'D_Mass_MIN'          : 1600 *MeV,
                # Maximum D mass
                'D_Mass_MAX'          : 2500 *MeV,
                # Maximum chi^2 on D end vertex
                'D_VCHI2PDOF_MAX'         : 5,
                # minimum flight time                
                'D_BPVLTIME_MIN'       : 0.15*picosecond,
                # max chi^2 per DOF of DTF fit
                'DTF_CHI2NDOF_MAX'   : 5,
                # Track quality requirement for soft pion from Dstar
                'Soft_TRCHI2DOF_MAX'   : 5,
                # Maximum Dstar-D0 mass difference
                'Dstar_AMDiff_MAX': 165. * MeV,
                'Dstar_MDiff_MAX': 160. * MeV,                
                # Maximum chi^2 on Dstar vertex
                'Dstar_VCHI2VDOF_MAX': 100.,
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'      : None,
                'Hlt2Filter'      : "(HLT_PASS_RE('Hlt2CharmHadD.*EmEp.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision'))",
                # prescale and postscale
                'PrescaleD2PiPi0EEG' : 1,
                'PrescaleD2KPi0EEG'  : 1,
                'PrescaleDst2D0PiEEG' : 1,
                'PostscaleD2PiPi0EEG': 1,
                'PostscaleD2KPi0EEG' : 1,
                'PostscaleDst2D0PiEEG' : 1
                }
          },
    'D2EtaEEGH' : {
          'WGs': ['Charm'],
          'BUILDERTYPE': 'StrippingD2EtaEEGHConf',
          'STREAMS': ['Charm'],
          'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Bachelor_IPCHI2_MIN'      : 25.0,        
                # Minimum PT for charged D daughters 
                'Bachelor_PT_MIN'          : 350  *MeV,   
                # Minimum momentum for charged D daughters 
                'Bachelor_P_MIN'           : 1000 *MeV,
                # Bachelor particle PID requirement 
                'Bachelor_PIDK_MIN'        : -999.0, # not used if < -10
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 25.0,     
                # Minimum PT for charged D daughters 
                'Daug_PT_MIN'          : 350  *MeV,   # 600 --> 300
                # Minimum momentum for charged D daughters 
                'Daug_P_MIN'           : 1000 *MeV,
                # Maximum momentum for charged D daughters 
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters 
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters 
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 5,    
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 350  *MeV,
                # Maximum mass for dielectron                
                'DiElectron_Mass_MAX'       : 650  *MeV,
                # Minimum PT for dielectron                
                'DiElectron_PT_MIN'       : 0  *MeV, 
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 400  *MeV, 
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 700  *MeV, 
                # Minimum D PT
                'D_PT_Min'           : 2000 *MeV, 
                # Minimum D mass
                'D_Mass_MIN'          : 1600 *MeV,
                # Maximum D mass
                'D_Mass_MAX'          : 2500 *MeV,
                # Maximum chi^2 on D end vertex
                'D_VCHI2PDOF_MAX'         : 5,
                # minimum flight time                
                'D_BPVLTIME_MIN'       : 0.15*picosecond,
                # max chi^2 per DOF of DTF fit
                'DTF_CHI2NDOF_MAX'   : 5,
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'         : None,
                'Hlt2Filter'      : "HLT_PASS_RE('Hlt2CharmHadDp2Eta.*_Eta2EmEpG.*Decision')",
                # prescale and postscale
                'PrescaleD2PiEtaEEG' : 1,
                'PrescaleD2KEtaEEG'  : 1,
                'PostscaleD2PiEtaEEG': 1,
                'PostscaleD2KEtaEEG' : 1,
                }
          },
    'Dst2PiD0EEG' : {
          'WGs': ['Charm'],
          'BUILDERTYPE': 'StrippingDst2PiD0EEGConf',
          'STREAMS': ['Charm'],
          'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 6.0,      
                # Minimum PT for charged D daughters 
                'Daug_PT_MIN'          : 1300  *MeV, 
                # Minimum momentum for charged D daughters 
                'Daug_P_MIN'           : 2000 *MeV,  
                # Maximum momentum for charged D daughters 
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters 
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters 
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 3,    
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 600  *MeV, # 350
                # Maximum mass for dielectron                
                'DiElectron_Mass_MAX'       : 1914  *MeV,
                # Minimum PT for dielectron                
                'DiElectron_PT_MIN'       : 2600  *MeV, 
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 1814   *MeV, 
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 1914  *MeV, 
                # Track quality requirement for soft pion from Dstar
                'Soft_TRCHI2DOF_MAX'   : 3,
                # Maximum Dstar-D0 mass difference
                'Dstar_AMDiff_MAX': 165. * MeV,
                'Dstar_MDiff_MAX': 160. * MeV,                
                # Maximum chi^2 on Dstar vertex
                'Dstar_VCHI2VDOF_MAX': 9.,
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'         : None,
                'Hlt2Filter'      : "HLT_PASS_RE('Hlt2CharmHadDstp2.*EmEp.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision')",
                 # prescale and postscale
                'PrescaleDst2PiD0EEG' : 1,
                'PostscaleDst2PiD0EEG' : 1                
                }
          },
    'D2EtaH' : {
          'WGs': ['Charm'],
          'BUILDERTYPE': 'StrippingD2EtaHConf',
          'STREAMS': ['Charm'],
          'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Bachelor_IPCHI2_MIN'      : 16.0,        # 25 --> 16
                # Minimum PT for charged D daughters
                'Bachelor_PT_MIN'          : 600  *MeV,   # 600
                # Minimum momentum for charged D daughters
                'Bachelor_P_MIN'           : 1000 *MeV,
                # Bachelor particle PID requirement 
                'Bachelor_PIDK_MIN'        : -999.0, # not used
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 16.0,        # 25 --> 4
                # Minimum PT for charged D daughters
                'Daug_PT_MIN'          : 500  *MeV,   # 600 --> 300
                # Minimum momentum for charged D daughters
                'Daug_P_MIN'           : 1000 *MeV,
                # Maximum momentum for charged D daughters
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 5,
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Maximum DLLK of intermediate resonance daughter (pion)
                'Pi_PIDK_MAX'        : 0.0,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 600  *MeV,
                # Mass window for intermediate resonance neutral daughters (pi0, eta)
                'Neut_Mass_Win'      :  50  *MeV,
                # Minimum dipion mass
                'Dipion_Mass_MIN'    : 200 *MeV,
                # Maximum dipion mass
                'Dipion_Mass_MAX'    : 750 *MeV,
                # Maximum DOCA chi2 for dipion combination
                'Dipion_DOCACHI2_MAX': 15,
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 350   *MeV,  # 500 --> 400
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 750  *MeV,   # 600 --> 700
                # Minimum D PT
                'D_PT_Min'           : 2000 *MeV,   # 
                # Minimum D mass
                'D_Mass_MIN'          : 1600 *MeV,
                # Maximum D mass
                'D_Mass_MAX'          : 2200 *MeV,
                # Maximum chi^2 on D end vertex
                'D_VCHI2PDOF_MAX'         : 4,
                # minimum flight time
                'D_BPVLTIME_MIN'       : 0.25*picosecond,
                # max chi^2 per DOF of DTF fit
                'DTF_CHI2NDOF_MAX'   : 5,    # not used
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'         : None,
                #'Hlt2Filter'         : None,
                #'Hlt2Filter'      : "(HLT_PASS_RE('Hlt2CharmHadDp2EtaPip_.*Decision') | HLT_PASS_RE('Hlt2CharmHadDp2EtaKp_.*Decision'))",
                'Hlt2Filter'      : "HLT_PASS_RE('Hlt2CharmHadDp2Eta.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision')",
                # prescale and postscale
                'PrescaleD2PiEtaPPG' : 1,
                'PrescaleD2PiEta3HM' : 1,
                'PrescaleD2PiEta3HR' : 1,
                'PrescaleD2KEtaPPG'  : 1,
                'PrescaleD2KEta3HM'  : 1,
                'PrescaleD2KEta3HR'  : 1,
                'PostscaleD2PiEtaPPG': 1,
                'PostscaleD2PiEta3HM': 1,
                'PostscaleD2PiEta3HR': 1,
                'PostscaleD2KEtaPPG' : 1,
                'PostscaleD2KEta3HM' : 1,
                'PostscaleD2KEta3HR' : 1
                }
          },
    'D2EtaPrimeH' : {
          'WGs': ['Charm'],
          'BUILDERTYPE': 'StrippingD2EtaPrimeHConf',
          'STREAMS': ['Charm'],
          'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Bachelor_IPCHI2_MIN'      : 16.0,        # 25 
                # Minimum PT for charged D daughters
                'Bachelor_PT_MIN'          : 600  *MeV,   # 600
                # Minimum momentum for charged D daughters
                'Bachelor_P_MIN'           : 1000 *MeV,
                # Bachelor particle PID requirement 
                'Bachelor_PIDK_MIN'        : -999.0, # not used 
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 16.0,        # 25 --> 16
                # Minimum PT for charged D daughters
                'Daug_PT_MIN'          : 500  *MeV,   # 600 --> 500
                # Minimum momentum for charged D daughters
                'Daug_P_MIN'           : 1000 *MeV,
                # Maximum momentum for charged D daughters
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 5,
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Maximum DLLK of intermediate resonance daughter (pion)
                'Pi_PIDK_MAX'        : 0,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 1000  *MeV,
                # Mass window for intermediate resonance neutral daughters (pi0, eta)
                'Neut_Mass_Win'      :  50  *MeV,
                # Minimum dipion mass
                'Dipion_Mass_MIN'    : 200 *MeV,
                # Maximum dipion mass
                'Dipion_Mass_MAX'    : 1200 *MeV,
                # Maximum DOCA chi2 for dipion combination
                'Dipion_DOCACHI2_MAX': 15,
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 650   *MeV,  # 900 --> 800
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 1200  *MeV,   # 990 --> 1050
                # Minimum D PT
                'D_PT_Min'           : 2000 *MeV,   # 2000
                # Minimum D mass
                'D_Mass_MIN'          : 1600 *MeV,
                # Maximum D mass
                'D_Mass_MAX'          : 2200 *MeV,
                # Maximum chi^2 on D end vertex
                'D_VCHI2PDOF_MAX'         : 4,
                # minimum flight time
                'D_BPVLTIME_MIN'       : 0.25*picosecond,
                # max chi^2 per DOF of DTF fit
                'DTF_CHI2NDOF_MAX'   : 5,
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'         : None,
                #'Hlt2Filter'         : None, 
                'Hlt2Filter'      : "HLT_PASS_RE('Hlt2CharmHadDp2Etap.*Decision') | HLT_PASS_RE('Hlt2Topo.*Decision')",
                # prescale and postscale
                'PrescaleD2PiEtaPrimePPG' : 1,
                'PrescaleD2PiEtaPrime3HR' : 1,
                'PrescaleD2KEtaPrimePPG'  : 1,
                'PrescaleD2KEtaPrime3HR'  : 1,
                'PostscaleD2PiEtaPrimePPG': 1,
                'PostscaleD2PiEtaPrime3HR': 1,
                'PostscaleD2KEtaPrimePPG' : 1,
                'PostscaleD2KEtaPrime3HR' : 1
                }
          },
    'D2PhiH' : {
          'WGs': ['Charm'],
          'BUILDERTYPE': 'StrippingD2PhiHConf',
          'STREAMS': ['Charm'],
          'CONFIG': {
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Bachelor_IPCHI2_MIN'      : 16.0,        # 25 --> 16
                # Minimum PT for charged D daughters
                'Bachelor_PT_MIN'          : 600  *MeV,   # 600 
                # Minimum momentum for charged D daughters
                'Bachelor_P_MIN'           : 1000 *MeV,
                # Bachelor particle PID requirement 
                'Bachelor_PIDK_MIN'        : -999.0, # not used 
                # Minimum best primary vertex IP chi^2 for charged D daughters
                'Daug_IPCHI2_MIN'      : 16.0,        # 25 --> 4
                # Minimum PT for charged D daughters
                'Daug_PT_MIN'          : 500  *MeV,   # 600 --> 300
                # Minimum momentum for charged D daughters
                'Daug_P_MIN'           : 1000 *MeV,
                # Maximum momentum for charged D daughters
                'Daug_P_MAX'           : 100000 *MeV,
                # Minimum pseudorapidity for charged D daughters
                'Daug_ETA_MIN'         : 2.0,
                # Maximum pseudorapidity for charged D daughters
                'Daug_ETA_MAX'         : 5.0,
                # Track quality requirement for charged D daughters
                'Daug_TRCHI2DOF_MAX'   : 5,
                # Maximum ghost probability for charged D daughters
                'Daug_TRGHOSTPROB_MAX' : 0.5,
                # Maximum DLLK of intermediate resonance daughter (pion)
                'Pi_PIDK_MAX'        : 0,
                # Minimum PT for intermediate resonance neutral daughters
                'Neut_PT_MIN'        : 600  *MeV,
                # Mass window for intermediate resonance neutral daughters (pi0, eta)
                'Neut_Mass_Win'      :  50  *MeV,
                # Minimum dipion mass
                'Dipion_Mass_MIN'    : 200 *MeV,
                # Maximum dipion mass
                'Dipion_Mass_MAX'    : 1200 *MeV,
                # Maximum DOCA chi2 for dipion combination
                'Dipion_DOCACHI2_MAX': 15,
                # Minimum mass for intermediate resonance
                'Res_Mass_MIN'       : 650   *MeV,  #  includes omega
                # Maximum mass for intermediate resonance
                'Res_Mass_MAX'       : 1200  *MeV,   #
                # Minimum D PT
                'D_PT_Min'           : 2000 *MeV,   #
                # Minimum D mass
                'D_Mass_MIN'          : 1600 *MeV,
                # Maximum D mass
                'D_Mass_MAX'          : 2200 *MeV,
                # Maximum chi^2 on D end vertex
                'D_VCHI2PDOF_MAX'         : 4,
                # minimum flight time
                'D_BPVLTIME_MIN'       : 0.25*picosecond,
                # max chi^2 per DOF of DTF fit
                'DTF_CHI2NDOF_MAX'   : 5,
                # HLT filters, only process events firing triggers matching the RegEx
                'Hlt1Filter'         : None,
                'Hlt2Filter'      : None,
                # prescale and postscale
                'PrescaleD2PiPhi3HM' : 1,
                'PrescaleD2PiPhi3HR' : 1,
                'PrescaleD2KPhi3HM'  : 1,
                'PrescaleD2KPhi3HR'  : 1,
                'PostscaleD2PiPhi3HM': 1,
                'PostscaleD2PiPhi3HR': 1,
                'PostscaleD2KPhi3HM' : 1,
                'PostscaleD2KPhi3HR' : 1
                }
          },
    }


class StrippingD2Pi0HConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    
    # Allowed configuration keys
    __configuration_keys__ = default_config['D2Pi0H']['CONFIG'].keys()
    
    # Decay descriptors
    Dst2D0Pi = ['D*(2010)+ -> D0 pi+','D*(2010)- -> D0 pi-']
    D02Pi0Pi0 = ['D0 -> pi0 pi0']
    D2PiPi0 = ['[D+ -> pi0 pi+]cc']
    D2KPi0 = ['[D+ -> pi0 K+]cc']
    
    Pi0EEG = ['pi0 -> J/psi(1S) gamma']
    
    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)
        
        pi0eeg_name = '{0}Pi0EEG'.format(name)
        
        d2pipi0eeg_name = '{0}D2PiPi0EEG'.format(name)        
        d2kpi0eeg_name = '{0}D2KPi0EEG'.format(name)

        d02pi0pi0eeg_name = '{0}D02Pi0Pi0EEG'.format(name)        
        dst2d0pieeg_name = '{0}Dst2D0PiEEG'.format(name)

        self.selPi0EEG = makeEEGResonance(
            pi0eeg_name, 
            config,
            inputSel=[StdLooseAllPhotons,StdDiElectronFromTracks],
            decDescriptors=self.Pi0EEG
            )
        
        self.selD2PiPi0EEG = makeD(
            d2pipi0eeg_name,
            config,
            inputSel=[self.selPi0EEG, StdAllNoPIDsPions],
            decDescriptors=self.D2PiPi0,
            useBachelorPID=False
            )

        usePIDforKaon = False
        if (config['Bachelor_PIDK_MIN'] > -10):
            usePIDforKaon = True
        else :
            usePIDforKaon = False
            
        self.selD2KPi0EEG = makeD(
            d2kpi0eeg_name, 
            config,
            inputSel=[self.selPi0EEG, StdAllNoPIDsKaons],
            decDescriptors=self.D2KPi0,
            useBachelorPID=usePIDforKaon
            )

        self.selD02Pi0Pi0EEG = makeD(
            d02pi0pi0eeg_name, 
            config,
            inputSel=[self.selPi0EEG],
            decDescriptors=self.D02Pi0Pi0,
            useBachelorPID=False,
            )

        self.selDst2D0PiEEG = makeDstar(
            dst2d0pieeg_name,
            config,
            inputSel=[self.selD02Pi0Pi0EEG, StdAllNoPIDsPions],
            decDescriptors=self.Dst2D0Pi,
            )

        self.line_D2PiPi0EEG = make_line(
            self,
            '{0}Line'.format(d2pipi0eeg_name),
            prescale=config['PrescaleD2PiPi0EEG'],
            postscale=config['PostscaleD2PiPi0EEG'],
            selection=self.selD2PiPi0EEG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

        self.line_D2KPi0EEG = make_line(
            self,
            '{0}Line'.format(d2kpi0eeg_name),
            prescale=config['PrescaleD2KPi0EEG'],
            postscale=config['PostscaleD2KPi0EEG'],
            selection=self.selD2KPi0EEG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_Dst2D0PiEEG = make_line(
            self,
            '{0}Line'.format(dst2d0pieeg_name),
            prescale=config['PrescaleDst2D0PiEEG'],
            postscale=config['PostscaleDst2D0PiEEG'],
            selection=self.selDst2D0PiEEG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

class StrippingDst2PiD0EEGConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    
    # Allowed configuration keys
    __configuration_keys__ = default_config['Dst2PiD0EEG']['CONFIG'].keys()
    
    # Decay descriptors
    Dst2D0Pi = ['D*(2010)+ -> D0 pi+','D*(2010)- -> D0 pi-']
    D0EEG = ['D0 -> J/psi(1S) gamma']
    
    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)
        
        d0eeg_name = '{0}D0EEG'.format(name)
        dst2pid0eeg_name = '{0}Dst2PiD0EEG'.format(name)

        self.selD0EEG = makeEEGResonance(
            d0eeg_name, 
            config,
            inputSel=[StdLooseAllPhotons,StdDiElectronFromTracks],
            decDescriptors=self.D0EEG
            )
        
        self.selDst2PiD0EEG = makeDstar(
            dst2pid0eeg_name,
            config,
            inputSel=[self.selD0EEG, StdAllNoPIDsPions],
            decDescriptors=self.Dst2D0Pi,
            )

        self.line_Dst2PiD0EEG = make_line(
            self,
            '{0}Line'.format(dst2pid0eeg_name),
            prescale=config['PrescaleDst2PiD0EEG'],
            postscale=config['PostscaleDst2PiD0EEG'],
            selection=self.selDst2PiD0EEG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )


class StrippingD2EtaEEGHConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    
    # Allowed configuration keys
    __configuration_keys__ = default_config['D2EtaEEGH']['CONFIG'].keys()
    
    # Decay descriptors
    D2PiEta = ['[D+ -> eta pi+]cc']
    D2KEta = ['[D+ -> eta K+]cc']
    
    EtaEEG = ['eta -> J/psi(1S) gamma']
    
    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)
        
        etaeeg_name = '{0}EtaEEG'.format(name)
        
        d2pietaeeg_name = '{0}D2PiEtaEEG'.format(name)        
        d2ketaeeg_name = '{0}D2KEtaEEG'.format(name)

        self.selEtaEEG = makeEEGResonance(
            etaeeg_name, 
            config,
            inputSel=[StdLooseAllPhotons,StdDiElectronFromTracks],
            decDescriptors=self.EtaEEG
            )
        
        self.selD2PiEtaEEG = makeD(
            d2pietaeeg_name,
            config,
            inputSel=[self.selEtaEEG, StdAllNoPIDsPions],
            decDescriptors=self.D2PiEta,
            useBachelorPID=False
            )

        usePIDforKaon = False
        if (config['Bachelor_PIDK_MIN'] > -10):
            usePIDforKaon = True
        else :
            usePIDforKaon = False
            
        self.selD2KEtaEEG = makeD(
            d2ketaeeg_name, 
            config,
            inputSel=[self.selEtaEEG, StdAllNoPIDsKaons],
            decDescriptors=self.D2KEta,
            useBachelorPID=usePIDforKaon
            )

        self.line_D2PiEtaEEG = make_line(
            self,
            '{0}Line'.format(d2pietaeeg_name),
            prescale=config['PrescaleD2PiEtaEEG'],
            postscale=config['PostscaleD2PiEtaEEG'],
            selection=self.selD2PiEtaEEG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

        self.line_D2KEtaEEG = make_line(
            self,
            '{0}Line'.format(d2ketaeeg_name),
            prescale=config['PrescaleD2KEtaEEG'],
            postscale=config['PostscaleD2KEtaEEG'],
            selection=self.selD2KEtaEEG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        
class StrippingD2EtaHConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    
    # Allowed configuration keys
    __configuration_keys__ = default_config['D2EtaH']['CONFIG'].keys()
    
    # Decay descriptors
    D2PiEta = ['[D+ -> eta pi+]cc']
    D2KEta = ['[D+ -> eta K+]cc']
    
    EtaPPG = ['eta -> pi+ pi- gamma']
    Eta3H = ['eta -> pi+ pi- pi0']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)
        
        etappg_name = '{0}EtaPPG'.format(name)
        eta3hr_name = '{0}Eta3HR'.format(name)
        eta3hm_name = '{0}Eta3HM'.format(name)
        
        d2pietappg_name = '{0}D2PiEtaPPG'.format(name)
        d2pieta3hr_name = '{0}D2PiEta3HR'.format(name)
        d2pieta3hm_name = '{0}D2PiEta3HM'.format(name)
        
        d2ketappg_name = '{0}D2KEtaPPG'.format(name)
        d2keta3hr_name = '{0}D2KEta3HR'.format(name)
        d2keta3hm_name = '{0}D2KEta3HM'.format(name)

        self.selEtaPPG = makeResonance(
            etappg_name,
            config,
            inputSel=[StdAllNoPIDsPions,StdLooseAllPhotons],
            decDescriptors=self.EtaPPG
            )

        self.selEta3HR = makeResonance(
            eta3hr_name,
            config,
            inputSel=[StdAllNoPIDsPions,StdLooseResolvedPi0],
            decDescriptors=self.Eta3H
            )

        self.selEta3HM = makeResonance(
            eta3hm_name,
            config,
            inputSel=[StdAllNoPIDsPions,StdLooseMergedPi0],
            decDescriptors=self.Eta3H
            )

        self.selD2PiEtaPPG = makeD(
            d2pietappg_name,
            config,
            inputSel=[self.selEtaPPG, StdAllNoPIDsPions],
            decDescriptors=self.D2PiEta,
            useBachelorPID=False,
            )

        self.selD2KEtaPPG = makeD(
            d2ketappg_name,
            config,
            inputSel=[self.selEtaPPG, StdAllNoPIDsKaons],
            decDescriptors=self.D2KEta,
            useBachelorPID=False,
            )
        
        self.selD2PiEta3HR = makeD(
            d2pieta3hr_name,
            config,
            inputSel=[self.selEta3HR, StdAllNoPIDsPions],
            decDescriptors=self.D2PiEta,
            useBachelorPID=False,
            )

        self.selD2KEta3HR = makeD(
            d2keta3hr_name,
            config,
            inputSel=[self.selEta3HR, StdAllNoPIDsKaons],
            decDescriptors=self.D2KEta,
            useBachelorPID=False,
            )
                                        
        self.selD2PiEta3HM = makeD(
            d2pieta3hm_name,
            config,
            inputSel=[self.selEta3HM, StdAllNoPIDsPions],
            decDescriptors=self.D2PiEta,
            useBachelorPID=False,
            )
        

        self.selD2KEta3HM = makeD(
            d2keta3hm_name,
            config,
            inputSel=[self.selEta3HM, StdAllNoPIDsKaons],
            decDescriptors=self.D2KEta,
            useBachelorPID=False,
            )
        

        self.line_D2PiEtaPPG = make_line(
            self,
            name='{0}Line'.format(d2pietappg_name),
            prescale=config['PrescaleD2PiEtaPPG'],
            postscale=config['PostscaleD2PiEtaPPG'],
            selection=self.selD2PiEtaPPG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

        self.line_D2PiEta3HM = make_line(
            self,
            name='{0}Line'.format(d2pieta3hm_name),
            prescale=config['PrescaleD2PiEta3HM'],
            postscale=config['PostscaleD2PiEta3HM'],
            selection=self.selD2PiEta3HM,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

        self.line_D2PiEta3HR = make_line(
            self,
            name='{0}Line'.format(d2pieta3hr_name),
            prescale=config['PrescaleD2PiEta3HR'],
            postscale=config['PostscaleD2PiEta3HR'],
            selection=self.selD2PiEta3HR,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

        self.line_D2KEtaPPG = make_line(
            self,
            name='{0}Line'.format(d2ketappg_name),
            prescale=config['PrescaleD2KEtaPPG'],
            postscale=config['PostscaleD2KEtaPPG'],
            selection=self.selD2KEtaPPG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_D2KEta3HM = make_line(
            self,
            name='{0}Line'.format(d2keta3hm_name),
            prescale=config['PrescaleD2KEta3HM'],
            postscale=config['PostscaleD2KEta3HM'],
            selection=self.selD2KEta3HM,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_D2KEta3HR = make_line(
            self,
            name='{0}Line'.format(d2keta3hr_name),
            prescale=config['PrescaleD2KEta3HR'],
            postscale=config['PostscaleD2KEta3HR'],
            selection=self.selD2KEta3HR,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

class StrippingD2EtaPrimeHConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['D2EtaPrimeH']['CONFIG'].keys()
    
    # Decay descriptors
    D2PiEtaPrime = ["[D+ -> eta_prime pi+]cc"]
    D2KEtaPrime = ["[D+ -> eta_prime K+]cc"]
    
    EtaPrimePPG = ["eta_prime -> pi+ pi- gamma"]
    EtaPrime3H = ["eta_prime -> pi+ pi- eta"]
    
    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)
        
        etaprimeppg_name = '{0}EtaPrimePPG'.format(name)
        etaprime3hr_name = '{0}EtaPrime3HR'.format(name)
        
        d2pietaprimeppg_name = '{0}D2PiEtaPrimePPG'.format(name)
        d2pietaprime3hr_name = '{0}D2PiEtaPrime3HR'.format(name)
        
        d2ketaprimeppg_name = '{0}D2KEtaPrimePPG'.format(name)
        d2ketaprime3hr_name = '{0}D2KEtaPrime3HR'.format(name)
        
        self.selEtaPrimePPG = makeResonance(
            etaprimeppg_name,
            config,
            inputSel=[StdAllNoPIDsPions,StdLooseAllPhotons],
            decDescriptors=self.EtaPrimePPG
            )
        
        self.selEtaPrime3HR = makeResonance(
            etaprime3hr_name,
            config,
            inputSel=[StdAllNoPIDsPions,StdLooseResolvedEta],
            decDescriptors=self.EtaPrime3H
            )
        
        self.selD2PiEtaPrimePPG = makeD(
            d2pietaprimeppg_name,
            config,
            inputSel=[self.selEtaPrimePPG, StdAllNoPIDsPions],
            decDescriptors=self.D2PiEtaPrime,
            useBachelorPID=False,
            )
        
        self.selD2KEtaPrimePPG = makeD(
            d2ketaprimeppg_name,
            config,
            inputSel=[self.selEtaPrimePPG, StdAllNoPIDsKaons],
            decDescriptors=self.D2KEtaPrime,
            useBachelorPID=False,
            )
        
        self.selD2PiEtaPrime3HR = makeD(
            d2pietaprime3hr_name,
            config,
            inputSel=[self.selEtaPrime3HR, StdAllNoPIDsPions],
            decDescriptors=self.D2PiEtaPrime,
            useBachelorPID=False,            
            )
        
        self.selD2KEtaPrime3HR = makeD(
            d2ketaprime3hr_name,
            config,
            inputSel=[self.selEtaPrime3HR, StdAllNoPIDsKaons],
            decDescriptors=self.D2KEtaPrime,
            useBachelorPID=False,
            )
        
        self.line_D2PiEtaPrimePPG = make_line(
            self,
            name='{0}Line'.format(d2pietaprimeppg_name),
            prescale=config['PrescaleD2PiEtaPrimePPG'],
            postscale=config['PostscaleD2PiEtaPrimePPG'],
            selection=self.selD2PiEtaPrimePPG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_D2PiEtaPrime3HR = make_line(
            self,
            name='{0}Line'.format(d2pietaprime3hr_name),
            prescale=config['PrescaleD2PiEtaPrime3HR'],
            postscale=config['PostscaleD2PiEtaPrime3HR'],
            selection=self.selD2PiEtaPrime3HR,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_D2KEtaPrimePPG = make_line(
            self,
            name='{0}Line'.format(d2ketaprimeppg_name),
            prescale=config['PrescaleD2KEtaPrimePPG'],
            postscale=config['PostscaleD2KEtaPrimePPG'],
            selection=self.selD2KEtaPrimePPG,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_D2KEtaPrime3HR = make_line(
            self,
            name='{0}Line'.format(d2ketaprime3hr_name),
            prescale=config['PrescaleD2KEtaPrime3HR'],
            postscale=config['PostscaleD2KEtaPrime3HR'],
            selection=self.selD2KEtaPrime3HR,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )

class StrippingD2PhiHConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['D2PhiH']['CONFIG'].keys()
    
    # Decay descriptors
    D2PiPhi = ["[D+ -> phi(1020) pi+]cc"]
    D2KPhi = ["[D+ -> phi(1020) K+]cc"]
    
    Phi3H = ["phi(1020) -> pi+ pi- pi0"]
    
    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)
        
        phi3hm_name = '{0}Phi3HM'.format(name)
        phi3hr_name = '{0}Phi3HR'.format(name)
        
        d2piphi3hm_name = '{0}D2PiPhi3HM'.format(name)
        d2piphi3hr_name = '{0}D2PiPhi3HR'.format(name)
        
        d2kphi3hm_name = '{0}D2KPhi3HM'.format(name)
        d2kphi3hr_name = '{0}D2KPhi3HR'.format(name)
        

        self.selPhi3HM = makeResonance(
            phi3hm_name,
            config,
            inputSel=[StdAllNoPIDsPions,StdLooseMergedPi0],
            decDescriptors=self.Phi3H
            )

        self.selPhi3HR = makeResonance(
            phi3hr_name,
            config,
            inputSel=[StdAllNoPIDsPions,StdLooseResolvedPi0],
            decDescriptors=self.Phi3H
            )
        
        self.selD2PiPhi3HM = makeD(
            d2piphi3hm_name,
            config,
            inputSel=[self.selPhi3HM, StdAllNoPIDsPions],
            decDescriptors=self.D2PiPhi,
            useBachelorPID=False,
            )
        
        self.selD2KPhi3HM = makeD(
            d2kphi3hm_name,
            config,
            inputSel=[self.selPhi3HM, StdAllNoPIDsKaons],
            decDescriptors=self.D2KPhi,
            useBachelorPID=False,
            )
        
        self.selD2PiPhi3HR = makeD(
            d2piphi3hr_name,
            config,
            inputSel=[self.selPhi3HR, StdAllNoPIDsPions],
            decDescriptors=self.D2PiPhi,
            useBachelorPID=False,
            )
        
        self.selD2KPhi3HR = makeD(
            d2kphi3hr_name,
            config,
            inputSel=[self.selPhi3HR, StdAllNoPIDsKaons],
            decDescriptors=self.D2KPhi,
            useBachelorPID=False,
            )
        
        self.line_D2PiPhi3HM = make_line(
            self,
            name='{0}Line'.format(d2piphi3hm_name),
            prescale=config['PrescaleD2PiPhi3HM'],
            postscale=config['PostscaleD2PiPhi3HM'],
            selection=self.selD2PiPhi3HM,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_D2PiPhi3HR = make_line(
            self,
            name='{0}Line'.format(d2piphi3hr_name),
            prescale=config['PrescaleD2PiPhi3HR'],
            postscale=config['PostscaleD2PiPhi3HR'],
            selection=self.selD2PiPhi3HR,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        self.line_D2KPhi3HM = make_line(
            self,
            name='{0}Line'.format(d2kphi3hm_name),
            prescale=config['PrescaleD2KPhi3HM'],
            postscale=config['PostscaleD2KPhi3HM'],
            selection=self.selD2KPhi3HM,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        self.line_D2KPhi3HR = make_line(
            self,
            name='{0}Line'.format(d2kphi3hr_name),
            prescale=config['PrescaleD2KPhi3HR'],
            postscale=config['PostscaleD2KPhi3HR'],
            selection=self.selD2KPhi3HR,
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
            )
        
        
def make_line(self, name, selection, prescale, postscale, **kwargs):
    """Create the stripping line defined by the selection.
    
    Keyword arguments:
    name -- Base name for the Line
    selection -- Selection instance
    prescale -- Fraction of candidates to randomly drop before stripping
    postscale -- Fraction of candidates to randomly drop after stripping
    **kwargs -- Keyword arguments passed to StrippingLine constructor
    """
    # Only create the line with positive pre- and postscales
    # You can disable each line by setting either to a negative value
    if prescale > 0 and postscale > 0:
        line = StrippingLine(
            name,
            selection=selection,
            prescale=prescale,
            postscale=postscale,
            **kwargs
            )
        self.registerLine(line)
        return line
    else:
        return False

    
    
def makeEEGResonance(name, config, inputSel, decDescriptors):  
    
    daugCuts = ( 
        '(PT > {0[Daug_PT_MIN]})'
        '& (P > {0[Daug_P_MIN]}) '
        '& (MIPCHI2DV(PRIMARY) > {0[Daug_IPCHI2_MIN]})'
        '& (TRCHI2DOF < {0[Daug_TRCHI2DOF_MAX]}) '
        '& (TRGHOSTPROB < {0[Daug_TRGHOSTPROB_MAX]}) '
        ).format(config)
    
    pidFiducialCuts = ( 
        '(in_range({0[Daug_P_MIN]}, P, {0[Daug_P_MAX]}))'
        '& (in_range({0[Daug_ETA_MIN]}, ETA, {0[Daug_ETA_MAX]}))'
        ).format(config)
    
    combCuts = "in_range( {0[Res_Mass_MIN]},AM,{0[Res_Mass_MAX]} )".format(config)
    
    dielectronCuts = (
        "(MM < {0[DiElectron_Mass_MAX]})"
        " & ( PT > {0[DiElectron_PT_MIN]})"
        ).format(config)
    
    gammaCuts = (
        "(PT > {0[Neut_PT_MIN]})"
        ).format(config)
    
    resCuts = "ALL"
    
    _combiner = CombineParticles(
        DecayDescriptors=decDescriptors,
        DaughtersCuts={
        'J/psi(1S)'  : '{0}'.format(dielectronCuts), 
        'gamma': '{0}'.format(gammaCuts)
        },
        CombinationCut=combCuts,
        MotherCut=resCuts
        )
    
    return Selection(name, Algorithm=_combiner, RequiredSelections=inputSel)


def makeResonance( name, config, inputSel, decDescriptors):  

    daugCuts = (
        '(PT > {0[Daug_PT_MIN]})'
        '& (P > {0[Daug_P_MIN]}) '
        '& (MIPCHI2DV(PRIMARY) > {0[Daug_IPCHI2_MIN]})'
        '& (TRCHI2DOF < {0[Daug_TRCHI2DOF_MAX]}) '
        '& (TRGHOSTPROB < {0[Daug_TRGHOSTPROB_MAX]}) '
        ).format(config)
    
    pidFiducialCuts = (
        '(in_range({0[Daug_P_MIN]}, P, {0[Daug_P_MAX]}))'
        '& (in_range({0[Daug_ETA_MIN]}, ETA, {0[Daug_ETA_MAX]}))'
        ).format(config)
    
    pionPIDCuts = (
        pidFiducialCuts +
        '& (PIDK-PIDpi < {0[Pi_PIDK_MAX]})'
        ).format(config)
    
    combCuts = "in_range( {0[Res_Mass_MIN]},AM,{0[Res_Mass_MAX]} )".format(config)
    
    pizeroCuts = (
        "(PT > {0[Neut_PT_MIN]})"
        "& (ADMASS('pi0') > {0[Neut_Mass_Win]})"
        ).format(config)
    
    gammaCuts = (
        "(PT > {0[Neut_PT_MIN]})"
        ).format(config)
    
    resCuts = "ALL"
    
    comb12Cuts = (
        "( in_range( {0[Dipion_Mass_MIN]},AM,{0[Dipion_Mass_MAX]}) )"
        "& ( ACUTDOCACHI2( {0[Dipion_DOCACHI2_MAX]} , '') ) "
        ).format(config)
    
    piCuts = ''
    if (config['Pi_PIDK_MAX'] < 10):
        piCuts = ('{0} & {1}'.format(daugCuts, pionPIDCuts))
    else:
        piCuts = ('{0}'.format(daugCuts))
        
    _combiner = DaVinci__N3BodyDecays(
        Combination12Cut = comb12Cuts,
        DecayDescriptors=decDescriptors,
        DaughtersCuts={
        'pi+'  : '{0}'.format(piCuts),
        'gamma': '{0}'.format(gammaCuts),
        'pi0'  : '{0}'.format(pizeroCuts),
        'eta'  : '{0}'.format(pizeroCuts),
        },
        CombinationCut=combCuts,
        MotherCut=resCuts
        )
    
    return Selection(name, Algorithm=_combiner, RequiredSelections=inputSel)


def makeD(name, config, inputSel, decDescriptors, useBachelorPID):        
    
    
    pidFiducialCuts = ( 
        '(in_range({0[Daug_P_MIN]}, P, {0[Daug_P_MAX]}))'
        '& (in_range({0[Daug_ETA_MIN]}, ETA, {0[Daug_ETA_MAX]}))'
        ).format(config)
    
    bachelorPIDCuts = (  
        pidFiducialCuts +
        '& (PIDK-PIDpi > {0[Bachelor_PIDK_MIN]})'
        ).format(config)
    
    daugCuts = (
        '(PT > {0[Bachelor_PT_MIN]})'
        '& (P > {0[Bachelor_P_MIN]}) '
        '& (MIPCHI2DV(PRIMARY) > {0[Bachelor_IPCHI2_MIN]})'
        '& (TRCHI2DOF < {0[Daug_TRCHI2DOF_MAX]}) ' # common TRCHI2DOF and TRGHOSTPROB for all pions
        '& (TRGHOSTPROB < {0[Daug_TRGHOSTPROB_MAX]}) '
        ).format(config)
    
    bachelorCuts = ''
    if (useBachelorPID):
        bachelorCuts = ('{0} & {1}'.format(daugCuts, bachelorPIDCuts))
    else:
        bachelorCuts = ('{0}'.format(daugCuts))
        
    combCuts = (
        "(APT > {0[D_PT_Min]})"
        "& ( in_range( {0[D_Mass_MIN]},AM,{0[D_Mass_MAX]}) )"
        ).format(config)
    
    dCuts = (
        '(VFASPF(VCHI2PDOF) < {0[D_VCHI2PDOF_MAX]})'
        '& (BPVLTIME() > {0[D_BPVLTIME_MIN]})'
        #'& (DTF_CHI2NDOF(True) < {0[DTF_CHI2NDOF_MAX]})' 
        ).format(config)
    
    _D = CombineParticles(
        #name='Combine{0}'.format(name),
        DecayDescriptors=decDescriptors,
        DaughtersCuts={
        'pi+': '{0}'.format(bachelorCuts),
        'K+': '{0}'.format(bachelorCuts)
        },
        CombinationCut=combCuts,
        MotherCut=dCuts
        )
    
    return Selection(name, Algorithm=_D, RequiredSelections=inputSel)



def makeDstar(name, config , inputSel , decDescriptors):

    daugCuts = (
        '(TRCHI2DOF < {0[Soft_TRCHI2DOF_MAX]}) ' 
        ).format(config)
    
    combCuts = (
        '( (AM - AM1) < {0[Dstar_AMDiff_MAX]})'
        ).format(config)
    
    dstarCuts = (
        '( VFASPF(VCHI2/VDOF) < {0[Dstar_VCHI2VDOF_MAX]} )'
        '& ( (M - M1) < {0[Dstar_MDiff_MAX]}  )'
        ).format(config)
    
    _Dstar = CombineParticles( DecayDescriptors = decDescriptors
                               , DaughtersCuts = { "pi+" : daugCuts }
                               , CombinationCut = combCuts
                               , MotherCut = dstarCuts
                               )
    
    return Selection( name , Algorithm = _Dstar, RequiredSelections = inputSel )

    
