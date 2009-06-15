# Hlt2 Inclusive Muon and Muon+track selections 06/12/09
#
# Antonio Perez-Calero (aperez@ecm.ub.es)
#
# Single muon line with IP for B Physics, plus a generic HighPt line
#
# Muon+track lines designed to trigger on a muon plus any other cherged track in the decay
# Thus, it can be a muon, as for Bs->mumu, or not, as in the case of Bd->K*mumu.
# In principle, two variants, with and without hard mass cuts. Still under study, line with mass cuts
# provides better efficiencies, so the alternative is not included
#
# Then, another lifetime unbiased selection designed to trigger on JPsi->mumu, not requiring
# the second track to have been explicitely idenfied as a muon. Should trigger signal for B->JPsi
# studies with increased robustness but also prompt JPsi with one of the muons not identified as such
# so that can be used for Muon ID calibration or efficiency purposes.

# Still missing "Hlt2B", that is, use fast track fit, cut on Chi2 and redo cuts with tighter values
# or in significances

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line
from HltLine.HltLine import Hlt2Member
from HltLine.HltLine import bindMembers


class Hlt2InclusiveMuonLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 'SingleMuonPt'         : 2800      # MeV
                  ,'SingleMuonIP'        : 0.220     # mm
                  ,'MuTrackMuPt'         : 1000      # MeV
                  ,'MuTrackTrPt'         : 600       # MeV
                  ,'MuTrackMuIP'         : 0.080     # mm
                  ,'MuTrackTrIP'         : 0.100     # mm
                  ,'MuTrackPoint'        : 0.4       # dimensionless
                  ,'MuTrack4JpsiMuPt'    : 1200      # MeV
                  ,'MuTrack4JpsiTrPt'    : 600       # MeV
                  
                  , 'IncludeLines' :['SingleMuon',
                                     #'SingleMuonMid',
                                     #'SingleMuonLow',
                                     'SingleHighPTMuon',
                                     'IncMuTrack',
                                     #'IncMuTrackMid',
                                     #'IncMuTrackLow',
                                     #'IncMuTrackLowMass',
                                     #'IncMuTrackLowMassMid',
                                     #'IncMuTrackLowMassLow',
                                     'IncMuTrack4Jpsi'
                                     ]
                  }
    
    
    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.BasicParticles import Muons, NoCutsPions

        #some string definitions... 
        decayDescB2MuTrack = ["[B0 -> mu+ pi-]cc","[B0 -> mu+ pi+]cc"]
        Doca = "(AMAXDOCA('')<0.150)"
        Dz = "(BPVVDZ>1.0)"
        #Dz = "(BPVVDZ>-99999.)"
        Mass = "(AM>2.2*GeV) & (AM<6.*GeV)"
        LowMass = "(AM>0.5*GeV) & (AM<6.*GeV)"
        
        singmuNoIP = "HLT_PASS('Hlt1SingleMuonNoIPL0Decision')"
        dimuonNoIP = "HLT_PASS('Hlt1DiMuonNoIPL0DiDecision') | HLT_PASS('Hlt1DiMuonNoIP2L0Decision') | HLT_PASS('Hlt1DiMuonNoIPL0SegDecision')"
        mutrackNoIP = "HLT_PASS('Hlt1MuTrack4JPsiDecision')"
        Hlt1UnbDiMu = singmuNoIP+" | "+dimuonNoIP+" | "+mutrackNoIP
                                
        ############################################################################
        #    Single muon selection
        ############################################################################
        
        Hlt2SelSingleMuon = Hlt2Member( FilterDesktop
                                        , "Filter"
                                        , InputLocations  = [Muons]
                                        , Code = "((PT>"+str(self.getProp('SingleMuonPt'))+"*MeV) & (MIPDV(PRIMARY)>"+str(self.getProp('SingleMuonIP'))+"))"
                                        )
        if 'SingleMuon' in self.getProp('IncludeLines'):
            Hlt2Line('SingleMuon'
                     , prescale = 1
                     , algos = [ Muons, Hlt2SelSingleMuon ]
                     , postscale = 1)
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonDecision" : 50190 } )

        #--------------------------------------------
        
        Hlt2SelSingleMuonMid = Hlt2Member( FilterDesktop
                                           , "FilterMid"
                                           , InputLocations  = [Muons]
                                           , Code = "((PT>3.6*GeV) & (MIPDV(PRIMARY)>0.220))"
                                           )
        if 'SingleMuonMid' in self.getProp('IncludeLines'):
            Hlt2Line('SingleMuonMid'
                     , prescale = 1
                     , algos = [ Muons, Hlt2SelSingleMuonMid ]
                     , postscale = 1)
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonMidDecision" : 50191 } )

        #--------------------------------------------
        
        Hlt2SelSingleMuonLow = Hlt2Member( FilterDesktop
                                           , "FilterLow"
                                           , InputLocations  = [Muons]
                                           , Code = "((PT>4.6*GeV) & (MIPDV(PRIMARY)>0.220))"
                                           )
        if 'SingleMuonLow' in self.getProp('IncludeLines'):
            Hlt2Line('SingleMuonLow'
                     , prescale = 1
                     , algos = [ Muons, Hlt2SelSingleMuonLow ]
                     , postscale = 1)
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonLowDecision" : 50192 } )
        

        ############################################################################
        #    Selection for a large PT single muon:  Sara Taynor <sara.traynor@ucd.ie>
        ############################################################################
        
        Hlt2SelSingleHighPTMuon = Hlt2Member(   FilterDesktop
                                                , "Filter"
                                                , Code = "(PT>20*GeV)"
                                                , InputLocations  = [Muons]
                                                )
        if 'SingleHighPTMuon' in self.getProp('IncludeLines'):
            line = Hlt2Line( 'SingleHighPTMuon'
                             , prescale = 1
                             , algos = [ Muons, Hlt2SelSingleHighPTMuon]
                             , postscale = 1)
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleHighPTMuonDecision" : 50440 } )


        ############################################################################
        #    Selections for inclusive decays with mu + track with mass cuts
        ############################################################################
        combine = Hlt2Member( CombineParticles
                              , "CombineB"
                              , DecayDescriptors = decayDescB2MuTrack
                              , DaughtersCuts = { "mu+" : "(PT>"+str(self.getProp('MuTrackMuPt'))+"*MeV) & (MIPDV(PRIMARY)>"+str(self.getProp('MuTrackMuIP'))+")",     # ip, pt and chi2 per dof
                                                  "pi+" : "(PT>"+str(self.getProp('MuTrackTrPt'))+"*MeV) & (MIPDV(PRIMARY)>"+str(self.getProp('MuTrackTrIP'))+")"}
                              , CombinationCut =  Doca+" & "+Mass  #doca and combined mass
                              , MotherCut = Dz+" & "+"(BPVTRGPOINTINGWPT<"+str(self.getProp('MuTrackPoint'))+")" # DZ and pointing of two particles
                              , InputLocations  = [ Muons , NoCutsPions ]
                              )
        if 'IncMuTrack' in self.getProp('IncludeLines'):
            line = Hlt2Line('IncMuTrack'
                            , prescale = 1
                            , algos = [Muons, NoCutsPions, combine]
                            , postscale = 1
                            )
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackDecision" : 50400 } )
        
        #---------------------------------------------------------------------------------------
        combine_mid = Hlt2Member( CombineParticles
                                  , "CombineBMid"
                                  , DecayDescriptors = decayDescB2MuTrack
                                  , DaughtersCuts = { "mu+" : "(PT>1000*MeV) & (MIPDV(PRIMARY)>0.100)",     # ip, pt and chi2 per dof
                                                      "pi+" : "(PT>800*MeV)  & (MIPDV(PRIMARY)>0.125)"}
                                  , CombinationCut =  Doca+" & "+Mass #doca, dz and combined mass
                                  , MotherCut = Dz+" & "+"(BPVTRGPOINTINGWPT<0.40)" # DZ and pointing of two particles
                                  , InputLocations  = [ Muons , NoCutsPions ]
                                  )
        if 'IncMuTrackMid' in self.getProp('IncludeLines'):
            line = Hlt2Line('IncMuTrackMid'
                            , prescale = 1
                            , algos = [Muons, NoCutsPions, combine_mid]
                            , postscale = 1
                            )
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackMidDecision" : 50401 } )

        #---------------------------------------------------------------------------------------
        
        combine_low = Hlt2Member( CombineParticles
                                  ,"CombineBLow"
                                  , DecayDescriptors = decayDescB2MuTrack
                                  , DaughtersCuts = { "mu+" : "(PT>1200*MeV) & (MIPDV(PRIMARY)>0.120)",     # ip, pt and chi2 per dof
                                                      "pi+" : "(PT>800*MeV)  & (MIPDV(PRIMARY)>0.150)"}
                                  , CombinationCut =  Doca+" & "+Mass #doca, dz and combined mass
                                  , MotherCut = Dz+" & "+"(BPVTRGPOINTINGWPT<0.28)" # DZ and pointing of two particles
                                  , InputLocations  = [ Muons , NoCutsPions ]
                                  )
        if 'IncMuTrackLow' in self.getProp('IncludeLines'):
            line = Hlt2Line('IncMuTrackLow'
                            , prescale = 1
                            , algos = [Muons, NoCutsPions, combine_low]
                            , postscale = 1
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
        if 'IncMuTrackLowMass' in self.getProp('IncludeLines'):
            line = Hlt2Line('IncMuTrackLowMass'
                            , prescale = 1
                            , algos = [Muons, NoCutsPions, combineLM]
                            , postscale = 1
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
        if 'IncMuTrackLowMassMid' in self.getProp('IncludeLines'):
            line = Hlt2Line('IncMuTrackLowMassMid'
                            , prescale = 1
                            , algos = [Muons, NoCutsPions, combineLM_mid]
                            , postscale = 1
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
        if 'IncMuTrackLowMassLow' in self.getProp('IncludeLines'):
            line = Hlt2Line('IncMuTrackLowMassLow'
                            , prescale = 1
                            , algos = [Muons, NoCutsPions, combineLM_low]
                            , postscale = 1
                            )
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrackLowMassLowDecision" : 50405 } )

        ############################################################################
        #    Selection for inclusive decays with JPsi ->mu+track
        ############################################################################

        combine_4jpsi = Hlt2Member( CombineParticles
                                    , "Combine"
                                    , DecayDescriptors = ["[J/psi(1S) -> mu+ pi-]cc"] 
                                    , DaughtersCuts = { "mu+" : "(PT>"+str(self.getProp('MuTrack4JpsiMuPt'))+"*MeV) & (TRCHI2DOF<10)",     # pt and chi2 per dof
                                                        "pi-" : "(PT>"+str(self.getProp('MuTrack4JpsiTrPt'))+"*MeV) & (TRCHI2DOF<10)"}
                                    , CombinationCut = "(AMAXDOCA('')<0.1) & (AM>3.000*GeV) & (AM<3.150*GeV)"  #doca and combined mass
                                    , MotherCut = "(P>20000*MeV) & (PT>1000*MeV)"  
                                    , InputLocations  = [ Muons , NoCutsPions ]
                                    )
        if 'IncMuTrack4Jpsi' in self.getProp('IncludeLines'):
            line = Hlt2Line('IncMuTrack4Jpsi'
                            , prescale = 1
                            , HLT = Hlt1UnbDiMu
                            , algos = [Muons, NoCutsPions, combine_4jpsi]
                            , postscale = 1
                            )
            HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncMuTrack4JpsiDecision" : 50406 } )
        

            ############################## Some old cuts
            #Single muon
            #  Code = "((P>10.*GeV) & (PT>2.8*GeV) & (MIPDV(PRIMARY)>0.200) & (TRCHI2DOF<10))"
            #, Code = "((P>10.*GeV) & (PT>3.2*GeV) & (MIPDV(PRIMARY)>0.200) & (TRCHI2DOF<10))"
            #, Code = "((P>10.*GeV) & (PT>3.8*GeV) & (MIPDV(PRIMARY)>0.260) & (TRCHI2DOF<10))"
            # MuTrack
            #, CombinationCut = "(AMAXDOCA('')<0.100) & (AM>2.2*GeV) & (AM<6.*GeV)"  #doca and combined mass
            #, CombinationCut = "(AMAXDOCA('')<0.1) & (WM('mu+','mu-')>3.050*GeV) & (WM('mu+','mu-')<3.150*GeV)"  #doca and combined mass
            #, MotherCut = "(BPVTRGPOINTINGWPT<0.40)" # DZ and pointing of two particles
            #, MotherCut = "(BPVTRGPOINTINGWPT<0.28)" # DZ and pointing of two particles
