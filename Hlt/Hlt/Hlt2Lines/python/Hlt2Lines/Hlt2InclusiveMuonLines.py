# Hlt2 Inclusive Muon and Muon+track selections 06/12/09 (updated 24th may 2010)
#
# Johanens Albrecht: single muon liens
# Antonio Perez-Calero (aperez@ecm.ub.es): Mu+Track lines
#
# 
#


from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2InclusiveMuonLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 'Prescale'   : { 'Hlt2MuonFromHLT1        '       : 1.0
                                   ,'Hlt2MuonFromHLT1Bs2MuMu'      : 1.0   
                                   ,'Hlt2SingleMuon'                : 1.0
                                   ,'Hlt2SingleMuonRateLimited'     : 'RATE(200)'
                                   ,'Hlt2SingleHighPTMuon'          : 1.0
                                   ,'Hlt2SingleVHighPTMuon'          : 1.0
                                   ,'Hlt2SingleMuonLowPT'          : 0.1
                                   ,'Hlt2IncMuTrack'                : 1.0
                                   ,'Hlt2IncMuTrackNoIP'            : 1.0
                                   }
                  
                  ,'SingleMuHLT1Filter'  :  "HLT_PASS_RE('Hlt1TrackMuonDecision')"
                  ,'TrChi2'              :   10      #chi2PerDof 
                  ,'TrChi2Tight'         :    5      #chi2PerDof 
                  ,'TrChi2VeryTight'     :    2      #chi2PerDof 
                  ,'SingleMuonPt'        : 1000      # MeV
                  ,'SingleMuonIP'        : 0.25     # mm
                  ,'SingleMuonIPChi2'    : 100
                  ,'SingleMuonHighPt'    : 10000     # MeV
                  ,'SingleMuonVHighPt'    : 15000     # MeV
                  ,'SingleMuonLowPt'     : 4800     # MeV
                  # Mu+track cuts 
                  ,'MuTrackMuPt'         : 800       # MeV
                  ,'MuTrackTrPt'         : 600       # MeV
                  ,'MuTrackTrChi2'       : 5.0       # dimensionless
                  ,'MuTrackChi2'         : 15.0      # dimensionless
                  ,'MuTrackMuIPChi2'     : 9.0       # dimensionless
                  ,'MuTrackTrIPChi2'     : 9.0       # dimensionless
                  ,'MuTrackDoca'         : 0.200     # mm
                  ,'MuTrackFDChi2'       : 64.0      # dimensionless
                  ,'MuTrackMass'         : 1000      # MeV
                  ,'MuTrackSumPt'        : 2200      # MeV
                  ,'MuTrackCorMass'      : 7000.0    # MeV
                  ,'MuTrackDIRA'         : 0.995       # dimensionless
                  # Mu+track no IP cuts
                  ,'MuTrackNoIPMuPt'    : 1600       # MeV
                  ,'MuTrackNoIPTrPt'    : 400        # MeV
                  ,'MuTrackNoIPDoca'    : 0.100      # mm
                  ,'MuTrackNoIPMass'    : 2900       # MeV
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import ( BiKalmanFittedMuons,
                                                                    BiKalmanFittedPions )

        #some string definitions... 
        decayDescB2MuTrack = ["[B0 -> mu+ pi-]cc","[B0 -> mu+ pi+]cc"]
        decayDescB2MuTrackNoIP = ["[B0 -> mu+ pi-]cc"]
        
        MuTrackMuCut = "(TRCHI2DOF<%(MuTrackTrChi2)s) " \
                     "& (PT>%(MuTrackMuPt)s*MeV) " \
                     "& (MIPCHI2DV(PRIMARY)>%(MuTrackMuIPChi2)s)" % self.getProps()
        
        MuTrackTrCut = "(TRCHI2DOF<%(MuTrackTrChi2)s) " \
                     "& (PT>%(MuTrackTrPt)s*MeV) " \
                     "& (MIPCHI2DV(PRIMARY)>%(MuTrackTrIPChi2)s)" % self.getProps()
        
        MuTrackComCut = "(AM>%(MuTrackMass)s*MeV) " \
                      "& (ASUM(PT)>%(MuTrackSumPt)s) " \
                      "& (AMAXDOCA('')<%(MuTrackDoca)s*mm)" % self.getProps()
        
        MuTrackCut = "(VFASPF(VCHI2)<%(MuTrackChi2)s) " \
                   "& (BPVVDCHI2>%(MuTrackFDChi2)s) " \
                   "& (BPVDIRA>%(MuTrackDIRA)s) " \
                   "& (MCOR<%(MuTrackCorMass)s*MeV)" % self.getProps()
        
        DocaNoIP = "(AMAXDOCA('')<%(MuTrackNoIPDoca)s*mm)" % self.getProps()
        MassNoIP = "(AM>%(MuTrackNoIPMass)s*MeV)" % self.getProps()
        
        Hlt1UnbMuon = "HLT_PASS_RE('Hlt1(SingleMuonNoIPL0|MuTrack4JPsi)Decision')"
        Hlt1AllMuons = "HLT_PASS_RE('Hlt1.*Mu.*Decision')"
        
        ############################################################################
        #    Single muon selection lines
        ############################################################################
        
        line = Hlt2Line('MuonFromHLT1'
                        , prescale = self.prescale
                        , HLT =Hlt1AllMuons
                        , VoidFilter = '' # explicitly require NO pile up filter... 
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuonFromHLT1Decision" : 50190 } )        
        
        #--------------------------------------------

        line = Hlt2Line('Hlt2MuonFromHLT1Bs2MuMu'
                        , prescale = self.prescale
                        , HLT = "HLT_PASS_RE('Hlt1.*4BsMuMu.*Decision')"
                        , VoidFilter = '' # explicitly require NO pile up filter... 
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuonFromHLT1Bs2MuMuDecision" : 50193 } )        
        
        #--------------------------------------------
        Hlt2SelSingleMuonFilter = Hlt2Member( FilterDesktop
                                        , "Filter"
                                        , Inputs  = [BiKalmanFittedMuons]
                                        , Code = "( (PT>%(SingleMuonPt)s*MeV) "\
                                            "& (MIPDV(PRIMARY)>%(SingleMuonIP)s)"\
                                            "& (TRCHI2DOF<%(TrChi2VeryTight)s)"\
                                            "& (MIPCHI2DV(PRIMARY)>%(SingleMuonIPChi2)s) "\
                                            ")" % self.getProps()
                                        )

        from Configurables import TisTosParticleTagger
        from HltLine.HltDecodeRaw import DecodeHlt1SelRep
        TOSInputMuonsFilter = Hlt2Member(TisTosParticleTagger
                                         ,"TOSInputMuonsFilter"
                                         , TisTosSpecs = { "Hlt1TrackMuonDecision%TOS":0 }
                                         ,Inputs = [ Hlt2SelSingleMuonFilter ]
                                         ,NoRegex=True
                                         ,ProjectTracksToCalo = False
                                         ,CaloClustForCharged = False
                                         ,CaloClustForNeutral = False
                                         ,TOSFrac = { 4:0.0, 5:0.0 }
                                         )

        from HltLine.Hlt2Monitoring import Hlt2Monitor,Hlt2MonitorMinMax

        Hlt2SelSingleMuonMoni = Hlt2Member( FilterDesktop
                                            , "Monitor"
                                            , Inputs  = [TOSInputMuonsFilter]
                                            , Code = "ALL"
                                            , PostMonitor =  Hlt2MonitorMinMax( "PT","PT(#mu)",0,10000,'muPT_out',nbins=100)
                                            +"&"+Hlt2MonitorMinMax( "MIPDV(PRIMARY)","MIPDV(PRIMARY)(#mu)",0,2,'muIP_out',nbins=200)
                                            +"&"+Hlt2MonitorMinMax( "TRCHI2DOF","TRCHI2DOF(#mu)",0,10,'muTRChi2_out',nbins=100)
                                            +"&"+Hlt2MonitorMinMax( "MIPCHI2DV(PRIMARY)","MIPCHI2DV(PRIMARY)(#mu)",0,1000,'muIPChi2_out',nbins=200)
                                            )
               
        line = Hlt2Line('SingleMuon'
                        , prescale = self.prescale 
                        , HLT =self.getProp("SingleMuHLT1Filter")
                        , algos = [ PV3D('Hlt2'), BiKalmanFittedMuons, 
                                    Hlt2SelSingleMuonFilter,DecodeHlt1SelRep,TOSInputMuonsFilter, 
                                    Hlt2SelSingleMuonMoni ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonDecision" : 50191 } )      

        '''
        unbiased dimuon low mass - prescaled
        '''
        line.clone( 'SingleMuonRateLimited'
                    , prescale = self.prescale 
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonRateLimitedDecision":  50195 } )
        #--------------------------------------------
        
        Hlt2SelSingleHighPTMuon = Hlt2Member(   FilterDesktop
                                                , "Filter"
                                                , Code = "(PT>%(SingleMuonHighPt)s*MeV)" % self.getProps()
                                                , Inputs  = [BiKalmanFittedMuons]
                                                , InputPrimaryVertices = "None"
                                                , UseP2PVRelations = False
                                                )
        line = Hlt2Line( 'SingleMuonHighPT'
                         , prescale = self.prescale 
                         , algos = [ BiKalmanFittedMuons, Hlt2SelSingleHighPTMuon]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonHighPTDecision" : 50192 } )
        #--------------------------------------------
        
        Hlt2SelSingleVHighPTMuon = Hlt2Member(   FilterDesktop
                                                , "Filter"
                                                , Code = "(PT>%(SingleMuonVHighPt)s*MeV)" % self.getProps()
                                                , Inputs  = [BiKalmanFittedMuons]
                                                , InputPrimaryVertices = "None"
                                                , UseP2PVRelations = False
                                                )
        line = Hlt2Line( 'SingleMuonVHighPT'
                         , prescale = self.prescale 
                         , algos = [ BiKalmanFittedMuons, Hlt2SelSingleVHighPTMuon]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonVHighPTDecision" : 50196} )
        #--------------------------------------------
                
        Hlt2SelSingleLowPTMuon = Hlt2Member(   FilterDesktop
                                                , "Filter"
                                                , Code = "(PT>%(SingleMuonLowPt)s*MeV) & (TRCHI2DOF<%(TrChi2)s) " % self.getProps()
                                                , Inputs  = [BiKalmanFittedMuons]
                                                , InputPrimaryVertices = "None"
                                                , UseP2PVRelations = False
                                                )
        line = Hlt2Line( 'SingleMuonLowPT'
                         , prescale = self.prescale 
                         , algos = [ BiKalmanFittedMuons, Hlt2SelSingleLowPTMuon]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonLowPTDecision" : 50194 } )

        #####################################################
        #    Selections for inclusive decays with mu + track 
        #####################################################

        _preambulo = [ "PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                      "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS" ]

        
        combine = Hlt2Member( CombineParticles
                              , "CombineB"
                              , DecayDescriptors = decayDescB2MuTrack
                              , DaughtersCuts = { "mu+" : MuTrackMuCut , 
                                                  "pi+" : MuTrackTrCut }
                              , CombinationCut = MuTrackComCut
                              , MotherCut = MuTrackCut  
                              , Inputs  = [ BiKalmanFittedMuons , BiKalmanFittedPions ]
                              , Preambulo = _preambulo
                              )
        line = Hlt2Line('MuTrack'
                        , prescale = self.prescale 
                        , algos = [ PV3D('Hlt2'), BiKalmanFittedMuons, BiKalmanFittedPions, combine]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuTrackDecision" : 50198 } )
        
        #---------------------------------------------------
        # mu+track NoIP
        
        combine_noip = Hlt2Member( CombineParticles
                                   , "Combine"
                                   , DecayDescriptors = decayDescB2MuTrackNoIP
                                   , DaughtersCuts = { "mu+" : "(PT>%(MuTrackNoIPMuPt)s*MeV)"%self.getProps(),
                                                       "pi-" : "(PT>%(MuTrackNoIPTrPt)s*MeV)"%self.getProps() }
                                   , CombinationCut = DocaNoIP+" & "+MassNoIP 
                                   , MotherCut = "ALL"
                                   , Inputs  = [ BiKalmanFittedMuons , BiKalmanFittedPions ]
                                   , InputPrimaryVertices = "None"
                                   , UseP2PVRelations = False
                                   )
        line = Hlt2Line('MuTrackNoIP'
                        , prescale = self.prescale 
                        , HLT = Hlt1UnbMuon
                        , algos = [BiKalmanFittedMuons, BiKalmanFittedPions, combine_noip]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuTrackNoIPDecision" : 50197 } )
