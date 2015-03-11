###
#
#  Hlt2 for D*(2010)+ -> pi+ (D0->xx) selection:
#       where xx = {mu mu, pi pi, pi mu, k mu, ... }
#
#  @author F. Dettori Francesco.Dettori@cern.ch
#  @date 2010-02-16
#  Update W. Bonivento 23/9/2010
#  For reference on these trigger lines see:
#  http://indico.cern.ch/conferenceDisplay.py?confId=82193
#
#  Update - 2011-02-25 - Alexandr Kozlinskiy
#
##
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2Dst2D2XXLinesConf(HltLinesConfigurableUser) :
    __slots__ = { 'Prescale'     : { 'Hlt2Dst2PiD02PiPi'       :  1
                                   , 'Hlt2Dst2PiD02KPi'        :  1
                                   , 'Hlt2Dst2PiD02MuMu'       :  1
                                   , 'Hlt2Dst2PiD02PiMu'       :  1
                                   , 'Hlt2Dst2PiD02KMu'        :  1
                                   , 'Hlt2Dst2PiD02EMu'        :  1
                                   , 'Hlt2Dst2PiD02EPi'        :  1
                                   , 'Hlt2Dst2PiD02EK'         :  1
                                   }
                  ,'XTrackChi2'         : 5          # adimensional
                  ,'XminP'              : 4000       # MeV
                  ,'XminPT'             : 750        # MeV
                  ,'XmaxPT'             : 1100       # MeV
                  ,'XminIPChi2'         : 3          # adimensional
                  ,'XmaxIPChi2'         : 8          # adimensional

                  ,'DMassWin'           : 70.        # MeV
                  ,'DMassWinSig'        : 300.       # MeV
                  ,'D0MinPT'            : 1800.      # MeV
                  ,'doca'               : 1.0        # mm

                  ,'DVChi2'             : 10
                  ,'DDira'              : 0.9997     # adimensional
                  ,'DMinFlightChi2'     : 20
                  ,'DMaxIPChi2'         : 15
                  }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import FilterDesktop, CombineParticles, HltANNSvc
        from HltTracking.HltPVs import PV3D
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons, BiKalmanFittedPions, BiKalmanFittedKaons, BiKalmanFittedElectrons
        
        # Common cuts
        d0comb_childcut = \
            "(P                  > %(XminP)s  * MeV) &" + \
            "(PT                 > %(XminPT)s * MeV) &" + \
            "(TRCHI2DOF          < %(XTrackChi2)s  ) &" + \
            "(MIPCHI2DV(PRIMARY) > %(XminIPChi2)s  )  "

        d0comb_combcut = \
            "(AMAXCHILD(PT) > %(XmaxPT)s   * MeV) &" + \
            "(ADAMASS('D0') < %(DMassWin)s * MeV) &" + \
            "(APT           > %(D0MinPT)s       ) &" + \
            "(AMAXDOCA('')  < %(doca)s     * mm )  "

        d0comb_combcut_sig = \
            "(AMAXCHILD(PT) > %(XmaxPT)s      * MeV) &" + \
            "(ADAMASS('D0') < %(DMassWinSig)s * MeV) &" + \
            "(APT           > %(D0MinPT)s          ) &" + \
            "(AMAXDOCA('')  < %(doca)s        * mm )  "

        d0comb_d0cut = \
            "INGENERATION(BPVIPCHI2() > %(XmaxIPChi2)s, 1 ) &" + \
            "(VFASPF(VCHI2PDOF)       < %(DVChi2)s        ) &" + \
            "(BPVDIRA                 > %(DDira)s         ) &" + \
            "(BPVVDCHI2               > %(DMinFlightChi2)s) &" + \
            "(MIPCHI2DV(PRIMARY)      < %(DMaxIPChi2)s    )  "

        # D0-> pi pi combination for Dst-> D0(->pipi) pi
        D2PiPiComb = Hlt2Member( CombineParticles 
                                 , "PiPi"     
                                 , DecayDescriptor =  "D0 -> pi+ pi-"
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "pi+" : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , Inputs = [ BiKalmanFittedPions ])
        
        # D0-> mu mu combination for Dst-> D0(->mumu) pi
        D2MuMuComb = Hlt2Member( CombineParticles 
                                 , "MuMu"     
                                 , DecayDescriptor =  "D0 -> mu+ mu-"
                                 #, DecayDescriptor =  "[D0 -> mu+ mu-]cc"
                                 , CombinationCut = d0comb_combcut_sig % self.getProps()
                                 , DaughtersCuts = { "mu+" : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , Inputs = [ BiKalmanFittedMuons ])
        
        # D0-> pi mu combination for Dst-> D0(->pimu) pi 
        D2PiMuComb = Hlt2Member( CombineParticles 
                                 , "PiMu"
                                 , DecayDescriptors = ["D0 -> mu+ pi-", "D0 -> mu- pi+"]
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "mu+" : d0comb_childcut % self.getProps(),
                                                     "pi+" : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , Inputs = [ BiKalmanFittedPions, BiKalmanFittedMuons ])

        # D0-> K pi combination for Dst-> D0(->Kpi) pi
        D2KPiComb = Hlt2Member( CombineParticles 
                                , "KPi"     
                                , DecayDescriptors =  ["D0 -> K- pi+", "D0 -> K+ pi-"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "K+"  : d0comb_childcut % self.getProps(),
                                                    "pi+" : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , Inputs = [ BiKalmanFittedPions, BiKalmanFittedKaons ])
        # D0-> K mu combination for Dst-> D0(->K mu) pi
        D2KMuComb = Hlt2Member( CombineParticles 
                                , "KMu"     
                                , DecayDescriptors =  ["D0 -> K- mu+", "D0 -> K+ mu-"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "K+"  : d0comb_childcut % self.getProps(),
                                                    "mu+" : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , Inputs = [ BiKalmanFittedMuons, BiKalmanFittedKaons ])

        # D0-> e mu combination for Dst-> D0(->e mu) pi
        D2EMuComb = Hlt2Member( CombineParticles 
                                , "EMu"     
                                , DecayDescriptors = ["D0 -> e+ mu-", "D0 -> e- mu+"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "e+"  : d0comb_childcut % self.getProps(),
                                                    "mu+" : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , Inputs = [ BiKalmanFittedElectrons, BiKalmanFittedMuons ])
        # D0-> e pi combination for Dst-> D0(->e pi) pi
        D2EPiComb = Hlt2Member( CombineParticles 
                                , "EPi"     
                                , DecayDescriptors = ["D0 -> e+ pi-", "D0 -> e- pi+"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "e+"  : d0comb_childcut % self.getProps(),
                                                    "pi+" : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , Inputs = [ BiKalmanFittedElectrons, BiKalmanFittedPions ])
        # D0-> e K combination for Dst-> D0(->e K) pi
        D2EKComb  = Hlt2Member( CombineParticles 
                                , "EK"     
                                , DecayDescriptors = ["D0 -> e+ K-", "D0 -> e- K+"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "e+"  : d0comb_childcut % self.getProps(),
                                                    "K+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , Inputs = [ BiKalmanFittedElectrons, BiKalmanFittedKaons ])
        
        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Pi Pi combinations 
        DstLine     = Hlt2Line('Dst2PiD02PiPi'    # Also wide mass box line for Dst-> pi ( D0 -> pipi)
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [ PV3D('Hlt2'), BiKalmanFittedPions,  D2PiPiComb  ]
                               )

        ################################################################################
        #
        # Mu Mu combinations 
        DstLine_mm  = Hlt2Line('Dst2PiD02MuMu'    # Also wide mass box line for Dst-> pi ( D0 -> pipi)
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [ PV3D('Hlt2'), BiKalmanFittedMuons, D2MuMuComb  ]
                               )

        ################################################################################
        #
        # Pi Mu and K Mu combinations for Mis-ID studies
        # (No need to be divided in different mass regions)
        DstLine_pm  = Hlt2Line('Dst2PiD02PiMu'
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [ PV3D('Hlt2'), BiKalmanFittedPions, BiKalmanFittedMuons, D2PiMuComb ]
                               )

        DstLine_km  = Hlt2Line('Dst2PiD02KMu'
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [ PV3D('Hlt2'), BiKalmanFittedKaons, BiKalmanFittedMuons, D2KMuComb ]
                               )

        ################################################################################
        #
        # K Pi combinations
        DstLine_kp  = Hlt2Line('Dst2PiD02KPi'
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [  PV3D('Hlt2'), BiKalmanFittedKaons, BiKalmanFittedPions, D2KPiComb ]
                               )
        
        ###########################################################        
        DstLine_emu = Hlt2Line('Dst2PiD02EMu'
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [ PV3D('Hlt2'), BiKalmanFittedElectrons, BiKalmanFittedMuons, D2EMuComb ]
                               , L0DU = "L0_CHANNEL('Electron')"
                               , HLT1 = "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"
                               )

        ###########################################################        
        DstLine_epi = Hlt2Line('Dst2PiD02EPi'
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [ PV3D('Hlt2'), BiKalmanFittedElectrons, BiKalmanFittedPions, D2EPiComb ]
                               , L0DU = "L0_CHANNEL('Electron')"
                               , HLT1 = "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"
                               )

        ###########################################################        
        DstLine_eK  = Hlt2Line('Dst2PiD02EK'
                               , prescale = self.prescale
                               , postscale = self.postscale
                               , algos = [ PV3D('Hlt2'), BiKalmanFittedElectrons, BiKalmanFittedKaons, D2EKComb ]
                               , L0DU = "L0_CHANNEL('Electron')"
                               , HLT1 = "HLT_PASS_RE('Hlt1(Track|.*Electron).*Decision')"
                               )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiPiDecision" : 50420 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KPiDecision"  : 50424 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02MuMuDecision" : 50428 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiMuDecision" : 50432 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KMuDecision"  : 50433 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02EMuDecision"  : 50434 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02EPiDecision"  : 50435 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02EKDecision"   : 50436 } )
