# Hlt2 Inclusive Phi selections 06/19/09
#
# Magnus Lieng (magnus.lieng@cern.ch)
# Kim Vervink (Kim.Vervink@cern.ch)
#
# Inclusive Phi line usign three steps:
#  - Robust cuts. (IPS, PT, P, DOCA, MM)
#  - Track fitting: error based cuts. (SIPS, PT, P, VCHI2, MM)
#  - RICH PID calculated. (RICHPID(K-Pi))
#
# At each step kaons candidated are created from tracks. These kaons are
# combined to create Phi candidates. 

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusivePhiLinesConf(HltLinesConfigurableUser) :

    __slots__ = {  'KaonPT'             : 800      # MeV
                  ,'KaonIP'             : 0.05     # mm
                  ,'PhiMassWin'         : 12       # MeV
                  ,'PhiMassWinSB'       : 30       # MeV
                  ,'PhiPT'              : 1800     # MeV
                  ,'PhiDOCA'            : 0.2      # mm
                  ,'TFKaonPT'           : 0        # MeV
                  ,'TFKaonIPS'          : 6        # dimensionless
                  ,'TFPhiMassWin'       : 12       # MeV
                  ,'TFPhiMassWinSB'     : 30       # MeV
                  ,'TFPhiPT'            : 1800     # MeV
                  ,'TFPhiVCHI2'         : 20       # dimensionless
                  ,'TFKaonRichPID'      : 0        # dimensionless
                  ,'Prescale'           : {'Hlt2IncPhiSidebands'  : 0.1
                                          ,'Hlt2IncPhi'           : 1.0
                                          ,'Hlt2IncPhiRobust'     : 0.001
                                          ,'Hlt2IncPhiTrackFit'         : 0.001
                                          }
                  ,'Postscale'          : {'Hlt2IncPhiSidebands'  : 1.0
                                          ,'Hlt2IncPhi'           : 1.0
                                          ,'Hlt2IncPhiRobust'     : 1.0
                                          ,'Hlt2IncPhiTrackFit'         : 1.0
                                          }
                  ,'HltANNSvcID'        : {'IncPhi'           : 50000
                                          ,'IncPhiSidebands'  : 50003
                                          ,'IncPhiRobust'     : 50001
                                          ,'IncPhiTrackFit'   : 50002
                                          }
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.GoodParticles import GoodKaons
	from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedRichKaons
        
        # Some string definitions... 
        decayDesc = ["phi(1020) -> K+ K-"]

        ############################################################################
        #    Inclusive Phi selection, robust cuts
        ############################################################################
        
        kaonPtCut = "(PT>%(KaonPT)s*MeV)" % self.getProps()
        kaonIpCut = "(MIPDV(PRIMARY)>%(KaonIP)s)"%self.getProps()
        phiMassCut = "(ADAMASS('phi(1020)')<%(PhiMassWin)s*MeV)"%self.getProps()+""
        phiMassCutSB = "(ADAMASS('phi(1020)')<%(PhiMassWinSB)s*MeV)"%self.getProps()
        phiDocaCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<%(PhiDOCA)s)"%self.getProps()
        phiPtCut = "(PT>%(PhiPT)s*MeV)"%self.getProps()
        Hlt2InclusivePhi = Hlt2Member( CombineParticles
                                       , "Combine"
                                       , DecayDescriptors = decayDesc
                                       , DaughtersCuts = { "K+" : kaonPtCut+" & "+kaonIpCut }
                                       , CombinationCut =  phiMassCut+" & "+phiDocaCut
                                       , MotherCut = phiPtCut
                                       , InputLocations  = [ GoodKaons ]
                                       )
        Hlt2InclusivePhiSB = Hlt2Member( CombineParticles
                                         , "CombineSB"
                                         , DecayDescriptors = decayDesc
                                         , DaughtersCuts = { "K+" : kaonPtCut+" & "+kaonIpCut }
                                         , CombinationCut =  phiMassCutSB+" & "+phiDocaCut
                                         , MotherCut = phiPtCut
                                         , InputLocations  = [ GoodKaons ]
                                         )
        
        
        ############################################################################
        #    Inclusive Phi selection, Track fitted
        ############################################################################
        
        TfKaonPtCut = "(PT>%(TFKaonPT)s*MeV)"%self.getProps()
        TfKaonIpsCut = "(MIPCHI2DV(PRIMARY)>%(TFKaonIPS)s)"%self.getProps()
        TfPhiMassCut = "(ADAMASS('phi(1020)')<%(TFPhiMassWin)s*MeV)"%self.getProps()
        TfPhiMassCutSB = "(ADAMASS('phi(1020)')<%(TFPhiMassWinSB)s*MeV)"%self.getProps()
        TfPhiVchi2Cut = "(VFASPF(VCHI2PDOF)<%(TFPhiVCHI2)s)"%self.getProps()
        TfPhiPtCut = "(PT>%(TFPhiPT)s*MeV)"%self.getProps()
        Hlt2InclusivePhiTF = Hlt2Member( CombineParticles
                                         , "TFCombine"
                                         , DecayDescriptors = decayDesc
                                         , DaughtersCuts = { "K+" : TfKaonPtCut+" & "+TfKaonIpsCut }
                                         , CombinationCut = TfPhiMassCut 
                                         , MotherCut = TfPhiVchi2Cut+" & "+TfPhiPtCut
                                         , InputLocations  = [ BiKalmanFittedKaons ]
                                         )
        Hlt2InclusivePhiTFSB = Hlt2Member( CombineParticles
                                           , "TFCombineSB"
                                           , DecayDescriptors = decayDesc
                                           , DaughtersCuts = { "K+" : TfKaonPtCut+" & "+TfKaonIpsCut }
                                           , CombinationCut = TfPhiMassCutSB 
                                           , MotherCut = TfPhiVchi2Cut+" & "+TfPhiPtCut
                                           , InputLocations  = [ BiKalmanFittedKaons ]
                                           )
        
        ############################################################################
        #    Inclusive Phi selection, RICH PID
        ############################################################################

        from HltLine.Hlt2Monitoring import Hlt2Monitor
    
        # Filter on RICH info
        TfKaonRichPidTf = "(2 == NINGENERATION(('K+'==ABSID) & (PIDK > %(TFKaonRichPID)s), 1))" % self.getProps()
        Hlt2InclusivePhiRich 	= Hlt2Member( CombineParticles
                                         , "RichCombine"
                                         , DecayDescriptors = decayDesc
                                         , DaughtersCuts = { "K+" : TfKaonPtCut+" & "+TfKaonIpsCut }
                                         , CombinationCut = TfPhiMassCut
                                         , MotherCut = TfPhiVchi2Cut+" & "+TfPhiPtCut + " & " + TfKaonRichPidTf
                                         , InputLocations  = [ BiKalmanFittedRichKaons ]
                                         , MotherMonitor  =  Hlt2Monitor("M", "M(KK)",1020,20)
                                         )
        Hlt2InclusivePhiRichSB 	= Hlt2Member( CombineParticles
                                           , "RichCombineSB"
                                           , DecayDescriptors = decayDesc
                                           , DaughtersCuts = { "K+" : TfKaonPtCut+" & "+TfKaonIpsCut }
                                           , CombinationCut = TfPhiMassCutSB
                                           , MotherCut = TfPhiVchi2Cut+" & "+TfPhiPtCut+" & "+TfKaonRichPidTf
                                           , InputLocations  = [ BiKalmanFittedRichKaons ]
                                           )
        ############################################################################
        #    Inclusive Phi complete line
        ############################################################################

        line = Hlt2Line('IncPhi'
                        , prescale = self.prescale
                        , algos = [ GoodKaons, 
                                    Hlt2InclusivePhi, 
                                    BiKalmanFittedKaons,
				    Hlt2InclusivePhiTF,
                                    BiKalmanFittedRichKaons, 
                                    Hlt2InclusivePhiRich]
                        , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncPhiDecision" : self.getProp('HltANNSvcID')['IncPhi'] } )
 
        ############################################################################
        #    Inclusive Phi robust only line
        ############################################################################
        
        line = Hlt2Line('IncPhiRobust'
                        , prescale = self.prescale
                        , algos = [ GoodKaons, 
                                    Hlt2InclusivePhi]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncPhiRobustDecision" : self.getProp('HltANNSvcID')['IncPhiRobust'] } )
        

        ############################################################################
        #    Inclusive Phi robust and TF line
        ############################################################################

        line = Hlt2Line('IncPhiTrackFit'
                        , prescale = self.prescale
                        , algos = [ GoodKaons, 
                                    Hlt2InclusivePhi, 
                                    BiKalmanFittedKaons,
				    Hlt2InclusivePhiTF]
                        , postscale = self.postscale
                          )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncPhiTrackFitDecision" : self.getProp('HltANNSvcID')['IncPhiTrackFit'] } )
        

        ############################################################################
        #    Inclusive Phi complete line for mass sidebands
        ############################################################################

        line = Hlt2Line('IncPhiSidebands'
                        , prescale = self.prescale
                        , algos = [ GoodKaons, 
                                    Hlt2InclusivePhiSB, 
                                    BiKalmanFittedKaons,
				    Hlt2InclusivePhiTFSB,
                                    BiKalmanFittedRichKaons,
				    Hlt2InclusivePhiRichSB]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2IncPhiSidebandsDecision" : self.getProp('HltANNSvcID')['IncPhiSidebands'] } )
          
        
