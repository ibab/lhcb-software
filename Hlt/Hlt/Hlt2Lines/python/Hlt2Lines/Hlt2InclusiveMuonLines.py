# Hlt2 Inclusive Muon and Muon+track selections 06/12/09
#
# Antonio Perez-Calero (aperez@ecm.ub.es)
#
# Single muon line with IP for generic B Physics data mining, plus a HighPt line
#
# Muon+track lines designed to trigger on a muon plus any other cherged track in the decay
# Thus, it can be a muon, as for Bs->mumu, or not, as in the case of Bd->K*mumu.
# In principle, two variants, with and without hard mass cuts. Line with mass cuts
# provides better efficiencies, so the alternative is not enabled.
#
# Then, another lifetime unbiased selection designed to trigger on JPsi->mumu, not requiring
# the second track to have been explicitely identified as a muon. Should trigger signal for B->JPsi
# studies with increased robustness but also prompt JPsi with one of the muons not identified as such
# so that can be used for Muon ID calibration or efficiency purposes.

# Still "Hlt2B", that is, use fast track fit, cut on Chi2 and redo cuts with tighter values
# or in significances, not implemented.

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2InclusiveMuonLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 'Prescale'             : { 'Hlt2SingleMuonLow'    : 0.4
                                             ,'Hlt2IncMuTrack4Jpsi' : 0.1
                                             }
                  
                  ,'SingleMuonPt'        : 3600      # MeV
                  ,'SingleMuonIP'        : 0.220     # mm
                  
                  ,'MuTrackMuPt'         : 1000      # MeV
                  ,'MuTrackTrPt'         : 800       # MeV
                  ,'MuTrackMuIP'         : 0.100     # mm
                  ,'MuTrackTrIP'         : 0.125     # mm
                  ,'MuTrackPoint'        : 0.3       # dimensionless
                  ,'MuTrack4JpsiMuPt'    : 1200      # MeV
                  ,'MuTrack4JpsiTrPt'    : 600       # MeV                  
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.BasicParticles import Muons, NoCutsPions

        #some string definitions... 
        decayDescB2MuTrack = ["[B0 -> mu+ pi-]cc","[B0 -> mu+ pi+]cc"]
        Doca = "(AMAXDOCA('')<0.150)"
        Dz = "(BPVVDZ>1.0)"
        Mass = "(AM>2.2*GeV) & (AM<6.*GeV)"
        LowMass = "(AM>0.5*GeV) & (AM<6.*GeV)"
        
        singmuNoIP  = "HLT_PASS_SUBSTR('Hlt1SingleMuonNoIP')"
        dimuonNoIP  = "HLT_PASS_SUBSTR('Hlt1DiMuonNoIP')"
        mutrackNoIP = "HLT_PASS('Hlt1MuTrack4JPsiDecision')"
        Hlt1UnbDiMu = "HLT_PASS_RE('Hlt1(Single|Di)MuonNoIP')" +"|" +mutrackNoIP
                                
        ############################################################################
        #    Single muon selection
        ############################################################################

        Hlt2Line('MuonFromHLT1'
                 , prescale = self.prescale
                 , HLT ="HLT_PASS_RE('Hlt1.*Muon.*Decision')"
                 , postscale = self.postscale
                 )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuonFromHLT1Decision" : 50189 } )        

        
        Hlt2SelSingleMuon = Hlt2Member( FilterDesktop
                                        , "Filter"
                                        , InputLocations  = [Muons]
                                        , Code = "((PT>2.8*GeV) & (MIPDV(PRIMARY)>0.220))"
                                        )
        Hlt2Line('SingleMuon'
                 , prescale = self.prescale 
                 , algos = [ Muons, PV3D(), Hlt2SelSingleMuon ]
                 , postscale = self.postscale
                 )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonDecision" : 50190 } )
            
        #--------------------------------------------
        
        Hlt2SelSingleMuonMid = Hlt2Member( FilterDesktop
                                           , "FilterMid"
                                           , InputLocations  = [Muons]
                                           , Code = "((PT>"+str(self.getProp('SingleMuonPt'))+"*MeV) & (MIPDV(PRIMARY)>"+str(self.getProp('SingleMuonIP'))+"))"
                                           )
        Hlt2Line('SingleMuonMid'
                 , prescale = self.prescale 
                 , algos = [ Muons, PV3D(), Hlt2SelSingleMuonMid ]
                 , postscale = self.postscale
                 )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonMidDecision" : 50191 } )
        
        #--------------------------------------------
        
        Hlt2SelSingleMuonLow = Hlt2Member( FilterDesktop
                                           , "FilterLow"
                                           , InputLocations  = [Muons]
                                           #, Code = "((PT>4.6*GeV) & (MIPDV(PRIMARY)>0.220))"
                                           , Code = "((PT>3.0*GeV) & (MIPDV(PRIMARY)>0.300))"
                                           )
        Hlt2Line('SingleMuonLow'
                 , prescale = self.prescale
                 , algos = [ Muons, PV3D(), Hlt2SelSingleMuonLow ]
                 , postscale = self.postscale
                 )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonLowDecision" : 50192 } )
    

        ############################################################################
        #    Selection for a large PT single muon:  Sara Taynor <sara.traynor@ucd.ie>
        ############################################################################
        
        Hlt2SelSingleHighPTMuon = Hlt2Member(   FilterDesktop
                                                , "Filter"
                                                , Code = "(PT>20*GeV)"
                                                , InputLocations  = [Muons]
                                                )
        line = Hlt2Line( 'SingleHighPTMuon'
                         , prescale = self.prescale 
                         , algos = [ Muons, Hlt2SelSingleHighPTMuon]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleHighPTMuonDecision" : 50440 } )
        

        ############################################################################
        #    Selections for inclusive decays with mu + track with mass cuts
        ############################################################################
        combine = Hlt2Member( CombineParticles
                              , "CombineB"
                              , DecayDescriptors = decayDescB2MuTrack
                              , DaughtersCuts = { "mu+" : "(PT>1000*MeV) & (MIPDV(PRIMARY)>0.080)",     
                                                  "pi+" : "(PT>600*MeV)  & (MIPDV(PRIMARY)>0.100)"}
                              , CombinationCut =  Doca+" & "+Mass  
                              , MotherCut = Dz+" & "+"(BPVTRGPOINTINGWPT<0.40)" 
                              , InputLocations  = [ Muons , NoCutsPions ]
                              )
        line = Hlt2Line('IncMuTrack'
                        , prescale = self.prescale 
                        , algos = [Muons,NoCutsPions, PV3D(), combine]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackDecision" : 50400 } )
        
        #---------------------------------------------------------------------------------------
        combine_mid = Hlt2Member( CombineParticles
                                  , "CombineBMid"
                                  , DecayDescriptors = decayDescB2MuTrack
                                  , DaughtersCuts = { "mu+" : "(PT>"+str(self.getProp('MuTrackMuPt'))+"*MeV) & (MIPDV(PRIMARY)>"+str(self.getProp('MuTrackMuIP'))+")",
                                                      "pi+" : "(PT>"+str(self.getProp('MuTrackTrPt'))+"*MeV) & (MIPDV(PRIMARY)>"+str(self.getProp('MuTrackTrIP'))+")"}
                                  , CombinationCut =  Doca+" & "+Mass 
                                  , MotherCut = Dz+" & "+"(BPVTRGPOINTINGWPT<"+str(self.getProp('MuTrackPoint'))+")" 
                                  , InputLocations  = [ Muons , NoCutsPions ]
                                  )
        line = Hlt2Line('IncMuTrackMid'
                        , prescale = self.prescale 
                        , algos = [Muons, NoCutsPions, PV3D(), combine_mid]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackMidDecision" : 50401 } )
        
        #---------------------------------------------------------------------------------------
        
        combine_low = Hlt2Member( CombineParticles
                                  ,"CombineBLow"
                                  , DecayDescriptors = decayDescB2MuTrack
                                  , DaughtersCuts = { "mu+" : "(PT>1200*MeV) & (MIPDV(PRIMARY)>0.120)",     
                                                      "pi+" : "(PT>800*MeV)  & (MIPDV(PRIMARY)>0.150)"}
                                  , CombinationCut =  Doca+" & "+Mass 
                                  , MotherCut = Dz+" & "+"(BPVTRGPOINTINGWPT<0.30)" 
                                  , InputLocations  = [ Muons , NoCutsPions ]
                                  )
        line = Hlt2Line('IncMuTrackLow'
                        , prescale = self.prescale 
                        , algos = [Muons, NoCutsPions, PV3D(),combine_low]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackLowDecision" : 50402 } )
        
        ############################################################################
        #    Selections for inclusive decays with mu + track no hard mass cuts
        ############################################################################
        combineLM = Hlt2Member( CombineParticles
                                , "CombineBLM"
                                , DecayDescriptors = decayDescB2MuTrack
                                , DaughtersCuts = { "mu+" : "(PT>1500*MeV) & (MIPDV(PRIMARY)>0.200) & (TRCHI2DOF<30.)",
                                                    "pi+" : "(PT>600*MeV) & (MIPDV(PRIMARY)>0.100) & (TRCHI2DOF<30.)" }
                                , CombinationCut = "(AMAXDOCA('')<0.10) & (AM>0.5*GeV) & (AM<6.*GeV)"
                                , MotherCut = "(BPVVDZ>2.) & (BPVTRGPOINTINGWPT<0.4)  "
                                , InputLocations  = [ Muons , NoCutsPions ]
                                )
        line = Hlt2Line('IncMuTrackLowMass'
                        , prescale = self.prescale 
                        , algos = [Muons, NoCutsPions, PV3D(), combineLM]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackLowMassDecision" : 50403 } )
        
        #---------------------------------------------------------------------------------------
        
        combineLM_mid = Hlt2Member( CombineParticles
                                    ,"CombineBLMMid"
                                    , DecayDescriptors = decayDescB2MuTrack
                                    , DaughtersCuts = { "mu+" : "(PT>2000*MeV) & (MIPDV(PRIMARY)>0.200) & (TRCHI2DOF<30.)",
                                                        "pi+" : "(PT>600*MeV) & (MIPDV(PRIMARY)>0.150) & (TRCHI2DOF<30.)" }
                                    , CombinationCut = "(AMAXDOCA('')<0.10) & (AM>0.5*GeV) & (AM<6.*GeV)"
                                    , MotherCut = "(BPVVDZ>2) & (BPVTRGPOINTINGWPT<0.35)  "
                                    , InputLocations  = [ Muons , NoCutsPions ]
                                    )
        line = Hlt2Line('IncMuTrackLowMassMid'
                        , prescale = self.prescale 
                        , algos = [Muons, NoCutsPions, PV3D(), combineLM_mid]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackLowMassMidDecision" : 50404 } )
        
        #---------------------------------------------------------------------------------------
        
        combineLM_low = Hlt2Member( CombineParticles
                                    ,"CombineBLMLow"
                                    , DecayDescriptors = decayDescB2MuTrack
                                    , DaughtersCuts = { "mu+" : "(PT>2000*MeV) & (MIPDV(PRIMARY)>0.200) & (TRCHI2DOF<30.)",
                                                        "pi+" : "(PT>800*MeV) & (MIPDV(PRIMARY)>0.150) & (TRCHI2DOF<30.)" }
                                    , CombinationCut = "(AMAXDOCA('')<0.10) & (AM>0.5*GeV) & (AM<6.*GeV)"
                                    , MotherCut = "(BPVVDZ>2.) & (BPVTRGPOINTINGWPT<0.30)  "
                                    , InputLocations  = [ Muons , NoCutsPions ]
                                    )
        line = Hlt2Line('IncMuTrackLowMassLow'
                        , prescale = self.prescale 
                        , algos = [Muons, NoCutsPions, PV3D(), combineLM_low]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackLowMassLowDecision" : 50405 } )
        
        ############################################################################
        #    Selection for inclusive decays with JPsi ->mu+track
        ############################################################################

        combine_4jpsi = Hlt2Member( CombineParticles
                                    , "Combine"
                                    , DecayDescriptors = ["[J/psi(1S) -> mu+ pi-]cc"] 
                                    , DaughtersCuts = { "mu+" : "(PT>"+str(self.getProp('MuTrack4JpsiMuPt'))+"*MeV) & (TRCHI2DOF<10)",     
                                                        "pi-" : "(PT>"+str(self.getProp('MuTrack4JpsiTrPt'))+"*MeV) & (TRCHI2DOF<10)"}
                                    , CombinationCut = "(AMAXDOCA('')<0.1) & (AM>3.000*GeV) & (AM<3.150*GeV)"  
                                    , MotherCut = "(P>20000*MeV) & (PT>1000*MeV)"  
                                    , InputLocations  = [ Muons , NoCutsPions ]
                                    , InputPrimaryVertices = "None"
                                    , UseP2PVRelations = False
                                    )
        line = Hlt2Line('IncMuTrack4Jpsi'
                        , prescale = self.prescale 
                        , HLT = Hlt1UnbDiMu
                        , algos = [Muons, NoCutsPions, combine_4jpsi]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrack4JpsiDecision" : 50406 } )
