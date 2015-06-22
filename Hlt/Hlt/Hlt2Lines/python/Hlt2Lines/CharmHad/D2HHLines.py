# -*- coding: utf-8 -*-
## @file
#
#  Set of Hlt2-lines for:
#   - D(s) -> h h0 with h0= pi0, eta, eta' 
#     where pi0/eta -> e+ e- gamma and eta/eta'-> pi+ pi- gamma
#   - D(s) -> eta(') h,
#     with eta(') -> pi+ pi- X (X = pi0, eta)
#
#  @author Tom HADAVIZADEH tom.hadavizadeh@physics.ox.ac.uk
#  @author Simone STRACKA simone.stracka@cern.ch
#                                                                       
#

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm 
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHLines() :
    def localcuts(self) : 
        return { 'D2HPi0' : { 'D_MassWinLow'              : 1600 * MeV, # don't tighten 
                              'D_MassWinHigh'             : 2200 * MeV, # don't tighten 
                              'D_PT'                      : 3000 * MeV, # don't tighten 
                              'Vertex_Chi2'               : 25,                              
                              'Neutral_MassWinLow'        : 70   * MeV, # don't tighten  #Neutral = Pi0
                              'Neutral_MassWinHigh'       : 210  * MeV, # don't tighten 
                              'Neutral_PT'                : 200  * MeV,                                                            
                              'Daug_PT'                   : 350  * MeV, 
                              'Daug_P'                    : 1000  * MeV,
                              'Track_Chi2'                : 5,
                              'Trk_MIPCHI2DV_MIN'         : 6.0,                               
                              'Gamma_PT'                  : 300  * MeV,
                              'Gamma_P'                   : 1000 * MeV,
                              'TisTosSpec'                : "Hlt1.*Track.*Decision%TOS"
                              },
                              
                 'D2HEta' : { 'D_MassWinLow'              : 1600 * MeV, # don't tighten 
                              'D_MassWinHigh'             : 2200 * MeV, # don't tighten 
                              'D_PT'                      : 3000 * MeV,
                              'Vertex_Chi2'               : 9,                              
                              'Neutral_MassWinLow'        : 450  * MeV, # don't tighten #Neutral = Eta
                              'Neutral_MassWinHigh'       : 650  * MeV, # don't tighten 
                              'Neutral_PT'                : 500  * MeV,                                                            
                              'Daug_PT'                   : 1000 * MeV,
                              'Daug_P'                    : 1000 * MeV,                                                            
                              'Track_Chi2'                : 3,
                              'AMAXDOCA'                  : 0.05  * mm,
                              'Trk_MIPCHI2DV_MIN'         : 6.0,      
                              'Gamma_PT'                  : 300  * MeV,  # 600 if high
                              'Gamma_P'                   : 1500 * MeV,
                              'TisTosSpec'                : "Hlt1.*Track.*Decision%TOS"
                              },                              
                                                                                                        
                 'Dstar2PiD0' :{'DeltaM_AM_MIN'           : 0    * MeV,  
                              'DeltaM_AM_MAX'             : 220  * MeV,
                              'TagVCHI2PDOF_MAX'          : 25.0,
                              'DeltaM_MIN'                : 0    * MeV,
                              'DeltaM_MAX'                : 200  * MeV, },
                                                             
                 'D0_ee' :  { 'Elec_PT'                   : 350  * MeV,
                              'Trk_MIPCHI2DV_MIN'         : 9.0,   
                              'Track_Chi2'                : 5,
                              'D0_AM_MassWinLow'          : 1780 * MeV,
                              'D0_AM_MassWinHigh'         : 1950 * MeV,
                              'D0_MassWinLow'             : 1814 * MeV,
                              'D0_MassWinHigh'            : 1914 * MeV,
                              'D0_PT'                     : 2500 * MeV,
                              'Vertex_Chi2'               : 25,
                              'TisTosSpec'                : "Hlt1.*Track.*Decision%TOS"
                              },
                                                                          
                 'D0_gg'   : {'Gamma_PT'                  : 1500  * MeV,
                              'Gamma_P'                   : 3000 * MeV,
                              'D0_AM_MassWinLow'          : 1780 * MeV,
                              'D0_AM_MassWinHigh'         : 1950 * MeV,
                              'D0_MassWinLow'             : 1814 * MeV,
                              'D0_MassWinHigh'            : 1914 * MeV,
                              'D0_PT'                     : 3000 * MeV},                              
                              
                 'Conv_Photon' : {'Elec_PT'               : 350  * MeV,
                                  'Trk_MIPCHI2DV_MIN'     : 9.0,   
                                  'Track_Chi2'            : 5,
                                  ## Never apply a TISTOS requirement to pairs
                                  ##   of conversion electrons.
                                  'TisTosSpec'            : [ ]
                                  }, 
                                  
                 'D2EtaPi_hhgamma' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  16,         # 25 if high - don't tighten beyond 25
                    'AM_MIN'            : 1450 * MeV, 
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,  # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV,  # don't tighten 
                    'APT_MIN'           : 3000.0 * MeV , # max 4000
                    'BPVLTIME_MIN'      : 0.25 * picosecond,  # don't tighten 
                    'DOCACHI2_MAX'      : 15, 
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 750 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2350 * MeV, 
                    'AMEta_MIN'         : 350 * MeV, # don't tighten 
                    'AMEta_MAX'         : 750 * MeV, # don't tighten
                    'MEta_MIN'          : 450 * MeV, # don't tighten 
                    'MEta_MAX'          : 650 * MeV, # don't tighten 
                    'Neut_ALL_PT_MIN'   : 600 * MeV, 
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                                                    },
                 'D2EtaK_hhgamma' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  16,        # 25 if high - don't tighten beyond 25
                    'AM_MIN'            : 1450 * MeV, 
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV, # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV, # don't tighten 
                    'APT_MIN'           : 3000.0 * MeV, # max 4000
                    'BPVLTIME_MIN'      : 0.25 * picosecond,  # don't tighten 
                    'DOCACHI2_MAX'      : 15,  
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 750 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2250 * MeV, #
                    'AMEta_MIN'         : 350 * MeV,  # don't tighten 
                    'AMEta_MAX'         : 750 * MeV,  # don't tighten 
                    'MEta_MIN'         : 450 * MeV,  # don't tighten 
                    'MEta_MAX'         : 650 * MeV,  # don't tighten 
                    'Neut_ALL_PT_MIN'   : 600 * MeV,
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                                                    },
                 'D2EtaPrimePi_hhgamma' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  25,
                    'AM_MIN'            : 1450 * MeV, 
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,   # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV,   # don't tighten 
                    'APT_MIN'           : 3000.0 * MeV,  
                    'BPVLTIME_MIN'      : 0.25 * picosecond, 
                    'DOCACHI2_MAX'      : 15,  
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1160 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2250 * MeV, 
                    'AMEta_MIN'         : 750 * MeV,   # don't tighten 
                    'AMEta_MAX'         : 1160 * MeV,  # don't tighten 
                    'MEta_MIN'         : 850 * MeV,   # don't tighten 
                    'MEta_MAX'         : 1060 * MeV,  # don't tighten 
                    'Neut_ALL_PT_MIN'   : 1000 * MeV, 
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                                                    },
                 'D2EtaPrimeK_hhgamma' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  25,
                    'AM_MIN'            : 1450 * MeV, 
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,  # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV,  # don't tighten 
                    'APT_MIN'           : 3000.0 * MeV,  
                    'BPVLTIME_MIN'      : 0.25 * picosecond, 
                    'DOCACHI2_MAX'      : 15,  
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1160 * MeV,
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2250 * MeV, 
                    'AMEta_MIN'         : 750 * MeV,  # don't tighten 
                    'AMEta_MAX'         : 1160 * MeV, # don't tighten 
                    'MEta_MIN'          : 850 * MeV,  # don't tighten 
                    'MEta_MAX'          : 1060 * MeV, # don't tighten 
                    'Neut_ALL_PT_MIN'   : 1000 * MeV, 
                    'Neut_ALL_P_MIN'    : 1000 * MeV,
                                                    },
                 ######                 
                 'D2EtaPi_3hR' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1300 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  25,
                    'AM_MIN'            : 1450 * MeV,
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,    # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV,    # don't tighten 
                    'APT_MIN'           : 5000.0 * MeV,
                    'BPVLTIME_MIN'      : 0.25 * picosecond, 
                    'DOCACHI2_MAX'      : 15,  
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1100 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2250 * MeV,
                    'AMEta_MIN'         : 350 * MeV,  # don't tighten beyond 680 
                    'AMEta_MAX'         : 980 * MeV,  # don't tighten 
                    'MEta_MIN'          : 450 * MeV,  # don't tighten beyond 680 
                    'MEta_MAX'          : 880 * MeV,  # don't tighten 
                    'Neut_ALL_PT_MIN'   : 900 * MeV,
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                    },
                 'D2EtaK_3hR' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1300 *MeV,  
                    'Trk_MIPCHI2DV_MIN' :  25,
                    'AM_MIN'            : 1450 * MeV,
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,  # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV,  # don't tighten 
                    'APT_MIN'           : 5000.0 * MeV,
                    'BPVLTIME_MIN'      : 0.25 * picosecond, 
                    'DOCACHI2_MAX'      : 15,  
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1100 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2350 * MeV,
                    'AMEta_MIN'         : 350 * MeV,  # don't tighten beyond 680 
                    'AMEta_MAX'         : 980 * MeV,  # don't tighten 
                    'MEta_MIN'         : 450 * MeV,  # don't tighten beyond 680 
                    'MEta_MAX'         : 880 * MeV,  # don't tighten 
                    'Neut_ALL_PT_MIN'  :  900 * MeV,
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                    },
                 'D2EtaPi_3hM' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  16,                # 25 if high - don't tighten beyond 25
                    'AM_MIN'            : 1450 * MeV,
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,         # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV,         # don't tighten 
                    'APT_MIN'           : 3000.0 * MeV,       # max 4000
                    'BPVLTIME_MIN'      : 0.25 * picosecond,  # don't tighten 
                    'DOCACHI2_MAX'      : 15,  
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1300 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2350 * MeV,
                    'AMEta_MIN'         : 300 * MeV,   # don't tighten beyond 450
                    'AMEta_MAX'         : 1300 * MeV,  # don't tighten beyond 880
                    'MEta_MIN'         : 400 * MeV,   # don't tighten beyond 450
                    'MEta_MAX'         : 1200 * MeV,  # don't tighten beyond 880
                    'Neut_ALL_PT_MIN'   : 600 * MeV,   # 800 if high
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                    },
                 'D2EtaK_3hM' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  16,               # 25 if high - don't tighten beyond 25 
                    'AM_MIN'            : 1450 * MeV,    
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,        # don't tighten 
                    'DMASS_MAX'         : 2200 * MeV,        # don't tighten 
                    'APT_MIN'           : 3000.0 * MeV,      # max 4000
                    'BPVLTIME_MIN'      : 0.25 * picosecond, # don't tighten 
                    'DOCACHI2_MAX'      : 15,  
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 1300 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2350 * MeV,
                    'AMEta_MIN'         : 300 * MeV,      # don't tighten beyond 450
                    'AMEta_MAX'         : 1300 * MeV,     # don't tighten beyond 880
                    'MEta_MIN'         : 400 * MeV,      # don't tighten beyond 450
                    'MEta_MAX'         : 1200 * MeV,     # don't tighten beyond 880
                    'Neut_ALL_PT_MIN'   : 600 * MeV,      # 800 if high
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                    },
                 'D2EtaPrimePi_3h' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  25,
                    'AM_MIN'            : 1450 * MeV,
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2200 * MeV,
                    'APT_MIN'           : 3000.0 * MeV,
                    'BPVLTIME_MIN'      : 0.25 * picosecond, 
                    'DOCACHI2_MAX'      : 15,
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 750 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 1950 * MeV,
                    'AMEta_MIN'         : 700 * MeV,
                    'AMEta_MAX'         : 1250 * MeV,
                    'MEta_MIN'         : 800 * MeV,
                    'MEta_MAX'         : 1150 * MeV,
                    'Neut_ALL_PT_MIN'   : 350 * MeV,
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                    },
                 'D2EtaPrimeK_3h' : {
                    'TisTosSpec'        : "Hlt1.*Track.*Decision%TOS",            
                    'Trk_PT_MIN'        :  1000 *MeV,
                    'Trk_MIPCHI2DV_MIN' :  25,
                    'AM_MIN'            : 1450 * MeV,
                    'AM_MAX'            : 2350 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2200 * MeV,
                    'APT_MIN'           : 3000.0 * MeV,
                    'BPVLTIME_MIN'      : 0.25 * picosecond, 
                    'DOCACHI2_MAX'      : 15,
                    'VCHI2PDOF_MAX'     : 4,
                    'MRho_MIN'          : 200 * MeV,
                    'MRho_MAX'          : 750 * MeV, 
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 1950 * MeV,
                    'AMEta_MIN'         : 700 * MeV,
                    'AMEta_MAX'         : 1250 * MeV,
                    'MEta_MIN'         : 800 * MeV,
                    'MEta_MAX'         : 1150 * MeV,
                    'Neut_ALL_PT_MIN'   : 350 * MeV,
                    'Neut_ALL_P_MIN'   : 1000 * MeV,
                    },
                 'DiPion_forD2HH' : {
                    'TisTosSpec'               : "Hlt1.*Track.*Decision%TOS",
                    'Trk_PIDK_MAX'             :   0,   
                    'Trk_PT_MIN'               : 500 *  MeV ,   
                    'Trk_MIPCHI2DV_MIN'        :  16 ,  # 25 if high        
                    'AM_MAX'                   : 1300 * MeV ,     
                    'ASUMPT_MIN'               :   0 * MeV ,
                    'ADOCA_MAX'                :  0.1 * mm , 
                    'VCHI2_MAX'                :  20.0 , 
                    'BPVVD_MIN'                :  0.0 , 
                    'BPVCORRM_MAX'             :  3.5 * GeV , 
                    'BPVVDCHI2_MIN'            :  0.0 ,
                    },                                                                 
                 }
    
    def locallines(self):
        from Stages import MassFilter
        from Stages import D2HH0_3Body_Combiner
        from Stages import TagDecay, TagDecayWithNeutral 
        from Stages import D0_gg_NeutralCombiner, D02_ee_Combiner
        from Stages import D2RhoHG_3Body_Combiner
        from Stages import D2HH_RhoToPipPim
        from Stages import SharedNoPIDDetachedChild_pi
        from Stages import SharedNoPIDDetachedChild_K
        from Stages import SharedNeutralLowPtChild_gamma
        from Stages import SharedNeutralLowPtChild_pi0R
        from Stages import SharedNeutralLowPtChild_pi0M
        from Stages import SharedNeutralLowPtChild_eta
        from Stages import SharedSoftTagChild_pi
        from Stages import Conv_Photon_2EmEp
        from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsPhotons, Hlt2NoPIDsKaons


        ## D+ -> pi+ pi0, pi0 -> (e- e+) gamma.  (e- e+) labeled KS0.
        D2PiPi0_eegamma = D2HH0_3Body_Combiner( 'Comb'
                , decay = "[D+ -> KS0 gamma pi+]cc"
                , inputs = [ Conv_Photon_2EmEp, Hlt2NoPIDsPhotons
                             , Hlt2NoPIDsPions ]
                , nickname = 'D2HPi0', shared = False )

        ## D+ -> K+ pi0, pi0 -> (e- e+) gamma.  (e- e+) labeled KS0.
        D2KPi0_eegamma = D2HH0_3Body_Combiner( 'Comb'
                , decay = "[D+ -> KS0 gamma K+]cc"
                , inputs = [ Conv_Photon_2EmEp, Hlt2NoPIDsPhotons
                             , Hlt2NoPIDsKaons ]
                , nickname = 'D2HPi0', shared = False )

        ## D+ -> pi+ pi0, eta -> (e- e+) gamma.  (e- e+) labeled KS0.
        D2PiEta_eegamma = D2PiPi0_eegamma.clone( 'Comb', nickname = 'D2HEta' )

        ## D+ -> K+ pi0, eta -> (e- e+) gamma.  (e- e+) labeled KS0.
        D2KEta_eegamma  = D2KPi0_eegamma.clone( 'Comb', nickname = 'D2HEta' )
       
        ## D*+ -> D0 pi+, D0 -> e+ e-.  Why is this in CharmHad?
        Dstar2PiD0_ee = TagDecay( 'Dstar2PiD0'
                , decay  = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                , inputs = [ D02_ee_Combiner('D0_ee'), SharedSoftTagChild_pi ])

        ## D*+ -> D0 pi+, D0 -> gamma gamma, one gamma -> e- e+.
        Dstar2PiD0_eegamma = TagDecayWithNeutral('Dstar2PiD0'
                , decay  = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [ D0_gg_NeutralCombiner('D0_gg'), SharedSoftTagChild_pi ] )

        ## D+ -> eta pi+, eta -> (pi+ pi-) gamma, (pi+ pi-) labeled rho(770)0.
        D2EtaPi_hhgamma = D2RhoHG_3Body_Combiner('D2EtaPi_hhgamma'
                , decay = [ "D+ -> rho(770)0 pi+ gamma", "D- -> rho(770)0 pi- gamma" ]
                , inputs = [ D2HH_RhoToPipPim, SharedNoPIDDetachedChild_pi, SharedNeutralLowPtChild_gamma ])

        ## D+ -> eta K+, eta -> (pi+ pi-) gamma, (pi+ pi-) labeled rho(770)0.
        D2EtaK_hhgamma = D2RhoHG_3Body_Combiner('D2EtaK_hhgamma'
                , decay = [ "D+ -> rho(770)0 K+ gamma", "D- -> rho(770)0 K- gamma" ]
                , inputs = [D2HH_RhoToPipPim, SharedNoPIDDetachedChild_K, SharedNeutralLowPtChild_gamma ])

        ## D+ -> eta' pi+, eta' -> (pi+ pi-) gamma, (pi+ pi-) labeled rho(770)0.
        D2EtaPrimePi_hhgamma = D2EtaPi_hhgamma.clone('D2EtaPrimePi_hhgamma')

        ## D+ -> eta' K+, eta' -> (pi+ pi-) gamma, (pi+ pi-) labeled rho(770)0.
        D2EtaPrimeK_hhgamma = D2EtaK_hhgamma.clone('D2EtaPrimeK_hhgamma')

        ## D+ -> eta pi+, eta -> (pi+ pi-) pi0, (pi+ pi-) labeled rho(770)0.
        D2EtaPi_3hR = D2RhoHG_3Body_Combiner('D2EtaPi_3hR'
                , decay = [ "D+ -> rho(770)0 pi+ pi0", "D- -> rho(770)0 pi- pi0" ]
                , inputs = [ D2HH_RhoToPipPim, SharedNoPIDDetachedChild_pi, SharedNeutralLowPtChild_pi0R ])
        D2EtaPi_3hM = D2EtaPi_3hR.clone('D2EtaPi_3hM'
                , inputs = [ D2HH_RhoToPipPim, SharedNoPIDDetachedChild_pi, SharedNeutralLowPtChild_pi0M ])


        ## D+ -> eta K+, eta -> (pi+ pi-) pi0, (pi+ pi-) labeled rho(770)0.
        D2EtaK_3hR = D2RhoHG_3Body_Combiner('D2EtaK_3hR'
                , decay = [ "D+ -> rho(770)0 K+ pi0", "D- -> rho(770)0 K- pi0" ]
                , inputs = [ D2HH_RhoToPipPim, SharedNoPIDDetachedChild_K, SharedNeutralLowPtChild_pi0R ])
        D2EtaK_3hM = D2EtaK_3hR.clone('D2EtaK_3hM'
                , inputs = [ D2HH_RhoToPipPim, SharedNoPIDDetachedChild_K, SharedNeutralLowPtChild_pi0M ])


        ## D+ -> eta' pi+, eta' -> (pi+ pi-) eta, eta -> gamma gamma
        ## (pi+ pi-) labeled rho(770)0.
        D2EtaPrimePi_3h = D2RhoHG_3Body_Combiner('D2EtaPrimePi_3h'
                , decay = [ "D+ -> rho(770)0 pi+ eta", "D- -> rho(770)0 pi- eta" ]
                , inputs = [ D2HH_RhoToPipPim, SharedNoPIDDetachedChild_pi, SharedNeutralLowPtChild_eta ])

        ## D+ -> eta' K+, eta' -> (pi+ pi-) eta, eta -> gamma gamma
        ## (pi+ pi-) labeled rho(770)0.
        D2EtaPrimeK_3h = D2RhoHG_3Body_Combiner('D2EtaPrimeK_3h'
                , decay = [ "D+ -> rho(770)0 K+ eta", "D- -> rho(770)0 K- eta"]
                , inputs = [ D2HH_RhoToPipPim, SharedNoPIDDetachedChild_K, SharedNeutralLowPtChild_eta ])


        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.
        stages = {'Dp2PipPi0_Pi02EmEpG'            : [ D2PiPi0_eegamma ],
                  'Dp2KpPi0_Pi02EmEpG'             : [ D2KPi0_eegamma ],
                  'Dp2EtaPip_Eta2EmEpG'            : [ D2PiEta_eegamma ],
                  'Dp2EtaKp_Eta2EmEpG'             : [ D2KEta_eegamma ],
                  'Dstp2D0Pip_D02EmEp'             : [ Dstar2PiD0_ee ],
                  'Dstp2D0Pip_D02GG_G2EmEp'        : [ Dstar2PiD0_eegamma ],
                  'Dp2EtaPip_Eta2PimPipG'          : [ D2EtaPi_hhgamma ],
                  'Dp2EtaKp_Eta2PimPipG'           : [ D2EtaK_hhgamma ],
                  'Dp2EtapPip_Etap2PimPipG'        : [ D2EtaPrimePi_hhgamma ],
                  'Dp2EtapKp_Etap2PimPipG'         : [ D2EtaPrimeK_hhgamma ],
                  'Dp2EtaPip_Eta2PimPipPi0_Pi0R'   : [ D2EtaPi_3hR ],
                  'Dp2EtaKp_Eta2PimPipPi0_Pi0R'    : [ D2EtaK_3hR ],
                  'Dp2EtaPip_Eta2PimPipPi0_Pi0M'   : [ D2EtaPi_3hM ],
                  'Dp2EtaKp_Eta2PimPipPi0_Pi0M'    : [ D2EtaK_3hM ],
                  'Dp2EtapPip_Etap2EtaPimPip_EtaR' : [D2EtaPrimePi_3h],
                  'Dp2EtapKp_Etap2EtaPimPip_EtaR'  : [D2EtaPrimeK_3h]
                  }
        return stages


