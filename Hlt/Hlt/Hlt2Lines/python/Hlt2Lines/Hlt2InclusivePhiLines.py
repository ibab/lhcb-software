#!/usr/bin/env python
# =============================================================================
# @file AnalyzeHlt2Efficiency.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# based on Magnus Lieng (magnus.lieng@cern.ch)
#          Kim Vervink (Kim.Vervink@cern.ch)
# @date 2011-01-04
# =============================================================================
"""Inclusive Phi line usign two steps:
  - Track cuts (IPS, PT, P, DOCA, MM, SIPS, PT, P, VCHI2, MM)
  - RICH PID calculated. (RICHPID(K-Pi))

Heavily based on the 3-step Inclusive Phi Line implementation by Magnus Lieng
and Kim Vervink.

"""

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2InclusivePhiLinesConf(HltLinesConfigurableUser) :
    __slots__ = {  'KaonPT'             : 800      # MeV
                  ,'KaonIP'             : 0.05     # mm
                  ,'KaonIPS'            : 6        # dimensionless
                  ,'PhiMassWin'         : 12       # MeV
                  ,'PhiMassWinSB'       : 30       # MeV
                  ,'PhiPT'              : 1800     # MeV
                  ,'PhiDOCA'            : 0.2      # mm
                  ,'PhiVCHI2'           : 20       # dimensionless
                  ,'KaonRichPID'        : 0        # dimensionless
                  ,'Prescale'           : {'Hlt2IncPhiSidebands'  : 0.1
                                          ,'Hlt2IncPhiTrackFit'   : 0.001
                                          ,'Hlt2IncPhi'           : 1.0
                                          }
                  ,'Postscale'          : {'Hlt2IncPhiSidebands'  : 1.0
                                          ,'Hlt2IncPhiTrackFit'   : 1.0
                                          ,'Hlt2IncPhi'           : 1.0
                                          }
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from Configurables import CombineParticles
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedRichKaons
        
        # Our decay descriptor 
        decayDesc = ["phi(1020) -> K+ K-"]

        ############################################################################
        #    Inclusive Phi selection, track cuts
        ############################################################################
        
        KaonPtCut = "(PT>%(KaonPT)s*MeV)" % self.getProps()
        KaonIpCut = "(MIPDV(PRIMARY)>%(KaonIP)s)" % self.getProps()
        KaonIpsCut = "(MIPCHI2DV(PRIMARY)>%(KaonIPS)s)" % self.getProps()
        PhiMassCut = "(ADAMASS('phi(1020)')<%(PhiMassWin)s*MeV)" % self.getProps()
        PhiMassCutSB = "(ADAMASS('phi(1020)')<%(PhiMassWinSB)s*MeV)" % self.getProps()
        PhiPtCut = "(PT>%(PhiPT)s*MeV)" % self.getProps()
        PhiDocaCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<%(PhiDOCA)s)" % self.getProps()
        PhiVchi2Cut = "(VFASPF(VCHI2PDOF)<%(PhiVCHI2)s)" % self.getProps()

        Hlt2InclusivePhi = Hlt2Member( CombineParticles
                                       , "TrackCombine"
                                       , DecayDescriptors = decayDesc
                                       , DaughtersCuts = { "K+" : "%s & %s & %s" % (KaonPtCut, KaonIpCut, KaonIpsCut) }
                                       , CombinationCut =  "%s & %s" % (PhiMassCut, PhiDocaCut)
                                       , MotherCut = "%s & %s" % (PhiPtCut, PhiVchi2Cut)
                                       , InputLocations  = [ BiKalmanFittedKaons ]
                                       )
        Hlt2InclusivePhiSB = Hlt2Member( CombineParticles
                                       , "TrackCombine"
                                       , DecayDescriptors = decayDesc
                                       , DaughtersCuts = { "K+" : "%s & %s & %s" % (KaonPtCut, KaonIpCut, KaonIpsCut) }
                                       , CombinationCut =  "%s & %s" % (PhiMassCut, PhiDocaCut)
                                       , MotherCut = "%s & %s" % (PhiPtCut, PhiVchi2Cut)
                                       , InputLocations  = [ BiKalmanFittedKaons ]
                                       )
                                               
        ############################################################################
        #    Inclusive Phi selection, RICH PID
        ############################################################################

        # from HltLine.Hlt2Monitoring import Hlt2Monitor
    
        # Filter on RICH info
        KaonRichPid = "(2 == NINGENERATION(('K+'==ABSID) & (PIDK > %(KaonRichPID)s), 1))" % self.getProps()
        Hlt2InclusivePhiRich  = Hlt2Member( CombineParticles
                                         , "RichCombine"
                                         , DecayDescriptors = decayDesc
                                         , DaughtersCuts = { "K+" : "%s & %s" % (KaonPtCut, KaonIpsCut) }
                                         , CombinationCut = PhiMassCut
                                         , MotherCut = "%s & %s & %s" % (PhiPtCut, PhiVchi2Cut, KaonRichPid)
                                         , InputLocations  = [ BiKalmanFittedRichKaons ]
                                         # , MotherMonitor  =  Hlt2Monitor("M", "M(KK)",1020,20)
                                         )
        Hlt2InclusivePhiRichSB  = Hlt2Member( CombineParticles
                                           , "RichCombineSB"
                                           , DecayDescriptors = decayDesc
                                           , DaughtersCuts = { "K+" : "%s & %s" % (KaonPtCut, KaonIpsCut) }
                                           , CombinationCut = PhiMassCutSB
                                           , MotherCut = "%s & %s & %s" % (PhiPtCut, PhiVchi2Cut, KaonRichPid)
                                           , InputLocations  = [ BiKalmanFittedRichKaons ]
                                           # , MotherMonitor  =  Hlt2Monitor("M", "M(KK)",1020,20)
                                           )
        ############################################################################
        #    Inclusive Phi complete line
        ############################################################################

        line = Hlt2Line('IncPhi'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedKaons,
                                    Hlt2InclusivePhi,
                                    BiKalmanFittedRichKaons, 
                                    Hlt2InclusivePhiRich]
                        , postscale = self.postscale
                         )
 
        ############################################################################
        #    Inclusive Phi robust and Track line
        ############################################################################

        line = Hlt2Line('IncPhiTrackFit'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedKaons,
                                    Hlt2InclusivePhi]
                        , postscale = self.postscale
                          )

        ############################################################################
        #    Inclusive Phi complete line for mass sidebands
        ############################################################################

        line = Hlt2Line('IncPhiSidebands'
                        , prescale = self.prescale
                        , algos = [ BiKalmanFittedKaons,
                                    Hlt2InclusivePhiSB,
                                    BiKalmanFittedRichKaons,
                                    Hlt2InclusivePhiRichSB]
                        , postscale = self.postscale
                        )

# EOF

