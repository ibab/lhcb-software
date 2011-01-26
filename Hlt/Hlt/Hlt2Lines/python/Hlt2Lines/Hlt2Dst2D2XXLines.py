###
#
#  Hlt2 for D*(2010)+ -> pi+ (D0->xx) selection:
#       where xx = {mu mu, pi pi, pi mu, k mu, ... }
#
#  @author F. Dettori Francesco.Dettori@cern.ch
#  @date 2010-02-16
# Update W. Bonivento 23/9/2010
#  For reference on these trigger lines see:
#  http://indico.cern.ch/conferenceDisplay.py?confId=82193
##
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2Dst2D2XXLinesConf(HltLinesConfigurableUser) :
    __slots__ = { 'Prescale'     : { 'Hlt2Dst2PiD02PiPi'         :  1
                                     , 'Hlt2Dst2PiD02KPi'        :  1
                                     , 'Hlt2Dst2PiD02MuMu'       :  1
                                     , 'Hlt2Dst2PiD02PiMu'       :  1
                                     , 'Hlt2Dst2PiD02KMu'        :  1
                                     , 'Hlt2Dst2PiD02EMu'        :  1
                                      , 'Hlt2Dst2PiD02EPi'        :  1
                                     , 'Hlt2Dst2PiD02EK'        :  1
                                     }
                  ,'DMassWin'           : 70.       # MeV
                  ,'doca'               : 0.1        # mm
                  ,'XminPT'             : 750.       # MeV
                  ,'XmaxPT'             : 1100.      # MeV
                  ,'XminP'              : 4000.      # MeV
                  ,'XTrackChi2'         : 5.        # adimensional
                  ,'XminIPChi2'         : 3        # adimensional
                  ,'XmaxIPChi2'         : 8        # adimensional
                  ,'DMinFlightChi2'    :  20.
                  ,'DDira'              : 0.9997     # adimensional
                  ,'D0MinPT'            : 1800.      # MeV
                  ,'DMaxIPChi2'        :15.
                  ,'DVChi2'            :10. 
                  }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons, BiKalmanFittedPions, BiKalmanFittedKaons, BiKalmanFittedElectrons
        
        # Common cuts
        d0comb_combcut =       "(AMAXDOCA('')< %(doca)s *mm) & (ADAMASS('D0')< %(DMassWin)s *MeV) & (AMAXCHILD(PT)>%(XmaxPT)s *MeV) & (APT> %(D0MinPT)s)"

        d0comb_childcut = "(PT> %(XminPT)s *MeV) & (P>%(XminP)s *MeV) & (TRCHI2DOF<%(XTrackChi2)s) & (MIPCHI2DV(PRIMARY)> %(XminIPChi2)s) " 
        d0comb_d0cut = "(BPVDIRA> %(DDira)s) & (INGENERATION( (BPVIPCHI2()>%(XmaxIPChi2)s),1 ) ) & (BPVVDCHI2> %(DMinFlightChi2)s) & (MIPCHI2DV(PRIMARY)< %(DMaxIPChi2)s) & (VFASPF(VCHI2PDOF)< %(DVChi2)s)"
        
        

        # D0-> pi pi combination for Dst-> D0(->pipi) pi
        D2PiPiComb = Hlt2Member( CombineParticles 
                                 , "PiPi"     
                                 , DecayDescriptor =  "D0 -> pi+ pi-"
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "pi+" : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , InputLocations = [BiKalmanFittedPions ])
        
        # D0-> mu mu combination for Dst-> D0(->mumu) pi
        D2MuMuComb = Hlt2Member( CombineParticles 
                                 , "MuMu"     
                                 , DecayDescriptor =  "D0 -> mu+ mu-"
                                 #, DecayDescriptor =  "[D0 -> mu+ mu-]cc"
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "mu+" : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , InputLocations = [ BiKalmanFittedMuons ])
        
        # D0-> pi mu combination for Dst-> D0(->pimu) pi 
        D2PiMuComb = Hlt2Member( CombineParticles 
                                 , "PiMu"
                                 , DecayDescriptors = ["D0 -> mu+ pi-","D0 -> mu- pi+"]
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "mu+" : d0comb_childcut % self.getProps(),
                                                     "pi+"  : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , InputLocations = [BiKalmanFittedPions, BiKalmanFittedMuons])

        # D0-> K pi combination for Dst-> D0(->Kpi) pi
        D2KPiComb = Hlt2Member( CombineParticles 
                                , "KPi"     
                                , DecayDescriptors =  ["D0 -> K- pi+","D0 -> K+ pi-"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "K+" : d0comb_childcut % self.getProps(),
                                                    "pi+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [BiKalmanFittedPions, BiKalmanFittedKaons])
        # D0-> K mu combination for Dst-> D0(->K mu) pi
        D2KMuComb = Hlt2Member( CombineParticles 
                                , "KMu"     
                                , DecayDescriptors =  ["D0 -> K- mu+", "D0 -> K+ mu-"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "K+" : d0comb_childcut % self.getProps(),
                                                    "mu+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [BiKalmanFittedMuons, BiKalmanFittedKaons])

        # D0-> e mu combination for Dst-> D0(->e mu) pi
        D2EMuComb = Hlt2Member( CombineParticles 
                                , "EMu"     
                                , DecayDescriptors = ["D0 -> e+ mu-","D0 -> e- mu+"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "e+" : d0comb_childcut % self.getProps(),
                                                    "mu+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [BiKalmanFittedElectrons, BiKalmanFittedMuons])
        # D0-> e pi combination for Dst-> D0(->e pi) pi
        D2EPiComb = Hlt2Member( CombineParticles 
                                , "EPi"     
                                , DecayDescriptors = ["D0 -> e+ pi-","D0 -> e- pi+"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "e+" : d0comb_childcut % self.getProps(),
                                                    "pi+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [BiKalmanFittedElectrons, BiKalmanFittedPions])
        # D0-> e K combination for Dst-> D0(->e K) pi
        D2EKComb = Hlt2Member( CombineParticles 
                                , "EK"     
                                , DecayDescriptors = ["D0 -> e+ K-","D0 -> e- K+"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "e+" : d0comb_childcut % self.getProps(),
                                                    "K+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [BiKalmanFittedElectrons, BiKalmanFittedKaons])
        
        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Pi Pi combinations 
        DstLine = Hlt2Line('Dst2PiD02PiPi'    # Also wide mass box line for Dst-> pi ( D0 -> pipi)
                           , prescale = self.prescale
                           , postscale = self.postscale
                           , algos = [ PV3D(), BiKalmanFittedPions,  D2PiPiComb  ]
                           )
        
        
        ################################################################################
        #
        # Mu Mu combinations 
        DstLine_mm = DstLine.clone('Dst2PiD02MuMu'
                                   , prescale = self.prescale
                                   , postscale = self.postscale
                                   , algos = [ PV3D(), BiKalmanFittedMuons, D2MuMuComb]
                                   )
        
        
        
        ################################################################################
        #
        # Pi Mu and K Mu combinations for Mis-ID studies
        # (No need to be divided in different mass regions)
        DstLine_pm = DstLine.clone('Dst2PiD02PiMu'
                                   , prescale = self.prescale
                                   , postscale = self.postscale
                                    , algos = [ PV3D(), BiKalmanFittedMuons, BiKalmanFittedPions, D2PiMuComb ]
                                   )

        DstLine_km = DstLine.clone('Dst2PiD02KMu'
                                   , prescale = self.prescale
                                   , postscale = self.postscale
                                   , algos = [ PV3D(), BiKalmanFittedMuons, BiKalmanFittedKaons, D2KMuComb ]
                                   )
        ################################################################################
        #
        # K Pi combinations
        DstLine_kp = DstLine.clone('Dst2PiD02KPi'
                                   , prescale = self.prescale
                                   , postscale = self.postscale
                                    , algos = [  PV3D(), BiKalmanFittedKaons, BiKalmanFittedPions, D2KPiComb ]
                                   )
        
        ###########################################################        
        DstLine_emu = DstLine.clone('Dst2PiD02EMu'
                                    , prescale = self.prescale
                                   , postscale = self.postscale
                                   , algos = [ PV3D(), BiKalmanFittedMuons,BiKalmanFittedElectrons, D2EMuComb ]
                                     )
        ###########################################################        
        DstLine_epi = DstLine.clone('Dst2PiD02EPi'
                                    , prescale = self.prescale
                                   , postscale = self.postscale
                                   , algos = [ PV3D(), BiKalmanFittedElectrons, BiKalmanFittedPions, D2EPiComb ]
                                     )
        ###########################################################        
        DstLine_eK = DstLine.clone('Dst2PiD02EK'
                                    , prescale = self.prescale
                                   , postscale = self.postscale
                                   , algos = [ PV3D(), BiKalmanFittedKaons, BiKalmanFittedElectrons, D2EKComb ]
                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiPiDecision" : 50420 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KPiDecision" : 50424 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02MuMuDecision" : 50428 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiMuDecision" : 50432 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KMuDecision" : 50433 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02EMuDecision" : 50434 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02EPiDecision" : 50435 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02EKDecision" : 50436 } )
