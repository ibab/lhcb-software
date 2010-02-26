###
#
#  Hlt2 for D*(2010)+ -> pi+ (D0->xx) selection:
#       where xx = {mu mu, pi pi, pi mu, k mu, ... }
#
#  @author F. Dettori Francesco.Dettori@cern.ch
#  @date 2010-02-16
#  For reference on these trigger lines see:
#  http://indico.cern.ch/conferenceDisplay.py?confId=82193
##
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2Dst2D2XXLinesConf(HltLinesConfigurableUser) :
    __slots__ = { 'Prescale'     : { 'Hlt2Dst2PiD02PiPi'         :  1
                                     , 'Hlt2Dst2PiD02PiPiD0SB'   :  1
                                     , 'Hlt2Dst2PiD02PiPiDMSB'   :  1
                                     , 'Hlt2Dst2PiD02PiPiSignal' :  1
                                     , 'Hlt2Dst2PiD02KPi'        :  1
                                     , 'Hlt2Dst2PiD02KPiD0SB'    :  1
                                     , 'Hlt2Dst2PiD02KPiDMSB'    :  1
                                     , 'Hlt2Dst2PiD02KPiSignal'  :  1
                                     , 'Hlt2Dst2PiD02MuMu'       :  1
                                     , 'Hlt2Dst2PiD02MuMuD0SB'   :  1
                                     , 'Hlt2Dst2PiD02MuMuDMSB'   :  1
                                     , 'Hlt2Dst2PiD02MuMuSignal' :  1
                                     , 'Hlt2Dst2PiD02PiMu'       :  1
                                     , 'Hlt2Dst2PiD02KMu'        :  1
                                     }
                  ,'DMassWinTight'      : 70.        # MeV
                  ,'DMassWin'           : 100.       # MeV
                  ,'doca'               : 0.2        # mm
                  ,'XminPT'             : 800.       # MeV
                  ,'XmaxPT'             : 1200.      # MeV
                  ,'XminP'              : 3400.      # MeV
                  ,'DDira'              : 0.9987     # adimensional
                  ,'PiMinPT'            : 110.       # MeV
                  ,'PiMaxIP'            : 4.0        # mm
                  ,'D0MinPT'            : 1900.      # MeV
                  ,'D0MinD'             : 0.65       # mm
                  ,'D0MaxIP'            : 0.35       # mm
                  ,'DstMassWin'         : 110.       # MeV
                  ,'DstD0DMWin'         : 10.        # MeV
                  ,'DstD0DMWinTight'    : 5.         # MeV           
                  }
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.BasicParticles import Muons, NoCutsPions, NoCutsKaons, Electrons
        
        # Common cuts
        d0comb_combcut =       "(AMAXDOCA('')< %(doca)s *mm) & (ADAMASS('D0')< %(DMassWin)s *MeV) & (AMAXCHILD(PT)>%(XmaxPT)s *MeV)"
        d0comb_combcut_tight = "(AMAXDOCA('')< %(doca)s *mm) & (ADAMASS('D0')< %(DMassWinTight)s *MeV) & (AMAXCHILD(PT)>%(XmaxPT)s *MeV)" 

        d0comb_childcut = "(PT> %(XminPT)s *MeV) & (P>%(XminP)s *MeV)" 
        d0comb_d0cut = "(BPVDIRA> %(DDira)s)"
        
        
        dstcomb_dstcut =  "(abs(M-MAXTREE('D0'==ABSID,M)-145.42) < %(DstD0DMWin)s )"
        dstcomb_dstcut_tight =  "(abs(M-MAXTREE('D0'==ABSID,M)-145.42) < %(DstD0DMWinTight)s )"
        
        dstcomb_combcut =  "(ADAMASS('D*(2010)+')<%(DstMassWin)s * MeV)"
        dstcomb_picut = "(PT> %(PiMinPT)s * MeV) &  (MIPDV(PRIMARY) < %(PiMaxIP)s)"
        dstcomb_d0cut = "(PT> %(D0MinPT)s * MeV) & (BPVVD > %(D0MinD)s ) & (MIPDV(PRIMARY) < %(D0MaxIP)s )"
        
                

        # D0-> pi pi combination for Dst-> D0(->pipi) pi
        D2PiPiComb = Hlt2Member( CombineParticles 
                                 , "PiPi"     
                                 , DecayDescriptor =  "D0 -> pi+ pi-"
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "pi+" : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , InputLocations = [NoCutsPions ])
        
        # D0-> mu mu combination for Dst-> D0(->mumu) pi
        D2MuMuComb = Hlt2Member( CombineParticles 
                                 , "MuMu"     
                                 , DecayDescriptor =  "D0 -> mu+ mu-"
                                 #, DecayDescriptor =  "[D0 -> mu+ mu-]cc"
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "mu+" : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , InputLocations = [ Muons ])
        
        # D0-> pi mu combination for Dst-> D0(->pimu) pi 
        D2PiMuComb = Hlt2Member( CombineParticles 
                                 , "PiMu"
                                 , DecayDescriptors = ["D0 -> mu+ pi-","D0 -> mu- pi+"]
                                 , CombinationCut = d0comb_combcut % self.getProps()
                                 , DaughtersCuts = { "mu+" : d0comb_childcut % self.getProps(),
                                                     "pi+"  : d0comb_childcut % self.getProps() }
                                 , MotherCut = d0comb_d0cut  % self.getProps()
                                 , InputLocations = [NoCutsPions, Muons])

        # D0-> K pi combination for Dst-> D0(->Kpi) pi
        D2KPiComb = Hlt2Member( CombineParticles 
                                , "KPi"     
                                , DecayDescriptors =  ["D0 -> K- pi+", "D0 -> K+ pi-"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "K+" : d0comb_childcut % self.getProps(),
                                                    "pi+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [NoCutsPions, NoCutsKaons])
        # D0-> K mu combination for Dst-> D0(->K mu) pi
        D2KMuComb = Hlt2Member( CombineParticles 
                                , "KMu"     
                                , DecayDescriptors =  ["D0 -> K- mu+", "D0 -> K+ mu-"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "K+" : d0comb_childcut % self.getProps(),
                                                    "mu+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [Muons, NoCutsKaons])

        # D0-> e mu combination for Dst-> D0(->e mu) pi
        D2EMuComb = Hlt2Member( CombineParticles 
                                , "EMu"     
                                , DecayDescriptors = ["D0 -> e+ mu-","D0 -> e- mu+"]
                                , CombinationCut = d0comb_combcut % self.getProps()
                                , DaughtersCuts = { "e+" : d0comb_childcut % self.getProps(),
                                                    "mu+"  : d0comb_childcut % self.getProps() }
                                , MotherCut = d0comb_d0cut  % self.getProps()
                                , InputLocations = [Electrons, Muons])
        ########################################################################
        # Common Dst -> D0 pi combination
        Dst2D0PiComb = Hlt2Member( CombineParticles 
                                   , "DstD0Pi"     
                                   , DecayDescriptors = ["D*(2010)+ -> pi+ D0", "D*(2010)- -> pi- D0" ]
                                   , CombinationCut = dstcomb_combcut % self.getProps()
                                   , DaughtersCuts = {    "pi+" : dstcomb_picut % self.getProps() ,
                                                          "D0"    : dstcomb_d0cut % self.getProps()
                                                          }
                                   , MotherCut =  dstcomb_dstcut % self.getProps()
                                   , InputLocations = [D2PiPiComb, NoCutsPions])
        
        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Pi Pi combinations 
        DstLine = Hlt2Line('Dst2PiD02PiPi'    # Also wide mass box line for Dst-> pi ( D0 -> pipi)
                           , prescale = self.postscale
                           , postscale = self.postscale
                           , algos = [ NoCutsPions, PV3D(), D2PiPiComb , Dst2D0PiComb ]
                           )
        
        D0SBline_pp = DstLine.clone('Dst2PiD02PiPiD0SB'
                                    , postscale = self.postscale
                                    , algos = [ NoCutsPions, PV3D(), D2PiPiComb , Dst2D0PiComb ]
                                    , DstD0Pi = { "InputLocations" : [D2PiPiComb, NoCutsPions],
                                                  "MotherCut" : dstcomb_dstcut_tight % self.getProps() }
                                    )
        
        DMSBline_pp = DstLine.clone('Dst2PiD02PiPiDMSB'
                                    , algos = [ NoCutsPions, PV3D(), D2PiPiComb , Dst2D0PiComb ]
                                    , PiPi = { "CombinationCut" : d0comb_combcut_tight % self.getProps() }
                                    , DstD0Pi = { "InputLocations" : [D2PiPiComb, NoCutsPions] }
                                    )
        
        Signalline_pp = DstLine.clone('Dst2PiD02PiPiSignal'
                                      , postscale = self.postscale
                                      , algos = [ NoCutsPions, PV3D(), D2PiPiComb , Dst2D0PiComb ]
                                      , PiPi = { "CombinationCut" : d0comb_combcut_tight % self.getProps() }
                                      , DstD0Pi = { "InputLocations" : [D2PiPiComb, NoCutsPions],
                                                    "MotherCut" : dstcomb_dstcut_tight % self.getProps() }
                                      )

        
        ################################################################################
        #
        # Mu Mu combinations 
        DstLine_mm = DstLine.clone('Dst2PiD02MuMu'
                                   , algos = [ NoCutsPions, Muons, PV3D(), D2MuMuComb , Dst2D0PiComb ]
                                   , DstD0Pi = { "InputLocations" : [D2MuMuComb, NoCutsPions] }
                                   )
        
        D0SBline_mm = DstLine.clone('Dst2PiD02MuMuD0SB'
                                    , algos = [ NoCutsPions, Muons, PV3D(), D2MuMuComb , Dst2D0PiComb ]
                                    , DstD0Pi = { "InputLocations" : [D2MuMuComb, NoCutsPions] ,
                                                  "MotherCut" : dstcomb_dstcut_tight % self.getProps() }
                                    )
        
        DMSBline_mm = DstLine.clone('Dst2PiD02MuMuDMSB'
                                    , algos = [ NoCutsPions, Muons, PV3D(), D2MuMuComb , Dst2D0PiComb ]
                                    , MuMu = { "CombinationCut" : d0comb_combcut_tight % self.getProps() }
                                    , DstD0Pi = { "InputLocations" : [D2MuMuComb, NoCutsPions] }
                                    )
        
        Signalline_mm = DstLine.clone('Dst2PiD02MuMuSignal'
                                      , postscale = self.postscale
                                      , algos = [ NoCutsPions, Muons, PV3D(), D2MuMuComb , Dst2D0PiComb ]
                                      , MuMu = { "CombinationCut" : d0comb_combcut_tight % self.getProps() }
                                      , DstD0Pi = { "InputLocations" : [D2MuMuComb, NoCutsPions],
                                                    "MotherCut" : dstcomb_dstcut_tight % self.getProps() }
                                      )
        
        
        ################################################################################
        #
        # Pi Mu and K Mu combinations for Mis-ID studies
        # (No need to be divided in different mass regions)
        DstLine_pm = DstLine.clone('Dst2PiD02PiMu'
                                   , algos = [ NoCutsPions, Muons, PV3D(), D2PiMuComb , Dst2D0PiComb ]
                                   , DstD0Pi = { "InputLocations" : [D2PiMuComb, NoCutsPions] }
                                   )

        DstLine_km = DstLine.clone('Dst2PiD02KMu'
                                   , algos = [ NoCutsPions, Muons, NoCutsKaons, PV3D(), D2KMuComb , Dst2D0PiComb ]
                                   , DstD0Pi = { "InputLocations" : [D2KMuComb, NoCutsPions] }
                                   )
        ################################################################################
        #
        # K Pi combinations
        DstLine_kp = DstLine.clone('Dst2PiD02KPi'
                                   , algos = [ NoCutsPions, NoCutsKaons, PV3D(), D2KPiComb , Dst2D0PiComb ]
                                   , DstD0Pi = { "InputLocations" : [D2KPiComb, NoCutsPions] }
                                   )
        
        D0SBline_kp = DstLine.clone('Dst2PiD02KPiD0SB'
                                    , algos = [ NoCutsPions, NoCutsKaons, PV3D(), D2KPiComb , Dst2D0PiComb ]
                                    , DstD0Pi = { "InputLocations" : [D2KPiComb, NoCutsPions],
                                                  "MotherCut" : dstcomb_dstcut_tight % self.getProps() }
                                    )
        
        DMSBline_kp = DstLine.clone('Dst2PiD02KPiDMSB'
                                    , algos = [ NoCutsPions, NoCutsKaons, PV3D(), D2KPiComb , Dst2D0PiComb ]
                                    , KPi = { "CombinationCut" : d0comb_combcut_tight % self.getProps() }
                                    , DstD0Pi = { "InputLocations" : [D2KPiComb, NoCutsPions] }
                                    )
        
        Signalline_kp = DstLine.clone('Dst2PiD02KPiSignal'
                                      , postscale = self.postscale
                                      , algos = [ NoCutsPions, NoCutsKaons, PV3D(), D2KPiComb , Dst2D0PiComb ]
                                      , KPi = { "CombinationCut" : d0comb_combcut_tight % self.getProps() }
                                      , DstD0Pi = { "InputLocations" : [D2KPiComb, NoCutsPions],
                                                    "MotherCut" : dstcomb_dstcut_tight % self.getProps() }
                                      )
        
##         DstLine_emu = DstLine.clone('Dst2PiD02EMu'
##                                     , algos = [ NoCutsPions, Electrons, Muons, PV3D(), D2EMuComb , Dst2D0PiComb ]
##                                     , DstD0Pi = { "InputLocations" : [D2EMuComb, NoCutsPions] }
##                                     )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiPiDecision" : 50420 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiPiD0SBDecision" : 50421 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiPiDMSBDecision" : 50422 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiPiSignalDecision" : 50423 } )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KPiDecision" : 50424 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KPiD0SBDecision" : 50425 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KPiDMSBDecision" : 50426 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KPiSignalDecision" : 50427 } )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02MuMuDecision" : 50428 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02MuMuD0SBDecision" : 50429 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02MuMuDMSBDecision" : 50430 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02MuMuSignalDecision" : 50431 } )
        
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02PiMuDecision" : 50432 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02KMuDecision" : 50433 } )
        #HltANNSvc().Hlt2SelectionID.update( { "Hlt2Dst2PiD02EMuDecision" : 50431 } )
        
        



        
