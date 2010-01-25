# =============================================================================
# $Id: Hlt2ExpressLines.py,v 1.2 2010-01-25 12:12:28 albrecht Exp $
# =============================================================================
## @file
#  Configuration of Hlt2 Lines for the express stream
#  @author Johannes Albrecht albrecht@cern.ch
#  @date 2009-12-02
# =============================================================================
"""
 script to configure Hlt2 Express stream
"""
# =============================================================================
__author__  = "Johannes Albrecht albrecht@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt2ExpressLinesConf(HltLinesConfigurableUser):
   
   __slots__ = { 'Prescale' : { 'Hlt2ExpressJPsi'          : 1.
                                , 'Hlt2ExpressJPsiTagProbe': 1.
                                , 'Hlt2ExpressLambda' : 1.
                                , 'Hlt2ExpressKS' : 1.
                                , 'Hlt2ExpressDs2PhiPi' : 1.
                                , 'Hlt2ExpressBeamHalo'           : 1.
                                }
                 ,'ExpressJPsiMassWindow'        :  120   # MeV
                 ,'ExpressJPsiPt'                : 1000   # MeV
                 ,'ExpressJPsiMuPt'              :  500   # MeV
                 ,'ExJPsiTPMassWindow'           :  300   # MeV
                 
                 }
   def __apply_configuration__(self):
      from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
      from HltLine.HltReco import PV3D
      from Configurables import HltANNSvc
      from Hlt2SharedParticles.DiMuon import DiMuon
      from Configurables import FilterDesktop
      '''
      Comment content of express stream here
      '''
      
      #--------------------------------------------
      '''
      unbiased JPsi - prescaled to ~5 Hz
      '''
      
      filter = Hlt2Member(   FilterDesktop 
                             , "Filter"
                             , Code = "(ADMASS('J/psi(1S)')<%(ExpressJPsiMassWindow)d*MeV)"\
                             " & (PT>%(ExpressJPsiPt)d*MeV)"\
                             " & (MINTREE('mu-'==ABSID,PT)>%(ExpressJPsiMuPt)d*MeV) " %  self.getProps() 
                             , InputLocations  = [ DiMuon ]
                             )
      
      line = Hlt2Line('ExpressJPsi'
                      , prescale = self.prescale 
                      , algos = [ PV3D,DiMuon, filter ]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      JPsi from only one muon (tag and probe)
      from StrippingMuIDCalib.py
      '''
      from Hlt2SharedParticles.BasicParticles import Muons, NoCutsPions
      from Configurables import CombineParticles
      from HltLine.HltReco import PV3D

      JPsiCombine = Hlt2Member( CombineParticles
                                , "JPsiCombine"
                                , DecayDescriptor = "[J/psi(1S) -> mu+ pi-]cc"
                                , InputLocations = [NoCutsPions,Muons]
                                , MotherCut = "(ADMASS('J/psi(1S)')<%(ExJPsiTPMassWindow)d*MeV)"\
                                " & (PT>%(ExpressJPsiPt)d*MeV) & (VFASPF(VCHI2/VDOF)<20)" %  self.getProps() 
                                , DaughtersCuts = { "mu+" : "(P>6000*MeV) & (PT>1500*MeV)"\
                                                    " & (MIPDV(PRIMARY)>0.08)",     
                                                    "pi+" : "(P >3000*MeV) &(PT>800*MeV)"\
                                                    " & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV)"\
                                                    " & (PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV)"\
                                                    " & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV)"\
                                                    " & (PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV)"} 
                                )
      
      line = Hlt2Line('ExpressJPsiTagProbe'
                      , prescale = self.prescale 
                      , algos = [ PV3D, Muons, NoCutsPions, JPsiCombine ]
                      , postscale = self.postscale
                      )
      
      #--------------------------------------------
      '''
      Lambda no PID
      from StrippingLambdaNoPID.py
      '''
      from Hlt2SharedParticles.BasicParticles import NoCutsPions,NoCutsProtons
      from Configurables import CombineParticles
      from HltLine.HltReco import PV3D
      
      LambdaCombine = Hlt2Member( CombineParticles
                                  , "LambdaCombine"
                                  , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                  , InputLocations = [NoCutsPions,NoCutsProtons ]
                                  , CombinationCut = "(ADAMASS('Lambda0')<20*MeV)"
                                  , MotherCut = "(50*mm<BPVVDZ) & (600*mm>BPVVDZ)"\
                                  " & (BPVDIRA>0.9999995) "
                                  , DaughtersCuts = { "p+"  :  "(P>3000*MeV) & (PT>100*MeV)"\
                                                      " & (MIPCHI2DV(PRIMARY)>9) " }
                                  )
      
      line = Hlt2Line('ExpressLambda'
                      , prescale = self.prescale 
                      , algos = [ PV3D, NoCutsPions, NoCutsProtons, LambdaCombine ]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      Ks no PID
      '''
      from Hlt2SharedParticles.BasicParticles import NoCutsPions,NoCutsProtons
      from Configurables import CombineParticles
      from HltLine.HltReco import PV3D

      KsCombine = Hlt2Member( CombineParticles
                              , "KsCombine"
                              , DecayDescriptor = "[KS0 -> pi+ pi-]cc"
                              , InputLocations = [NoCutsPions]
                              , CombinationCut = "(ADAMASS('KS0') < 150*MeV) & (AMAXDOCA('') < 0.6*mm)"
                              , MotherCut = "(ADMASS('KS0') < 100*MeV)"\
                              " & (VFASPF(VCHI2/VDOF) < 10)"\
                              " & (MIPDV(PRIMARY) < 0.75)"\
                              " & (BPVVDCHI2 > 150)"\
                              " & (MIPCHI2DV(PRIMARY) < 100) "
                              , DaughtersCuts = { "pi+"  :
                                                  "(P>2000*MeV) & (MIPCHI2DV(PRIMARY)>30) " }
                             )
      
      line = Hlt2Line('ExpressKS'
                      , prescale = self.prescale 
                      , algos = [ PV3D, NoCutsPions, KsCombine]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      Phi no PID
      '''
      from Hlt2SharedParticles.BasicParticles import NoCutsKaons
      from Configurables import CombineParticles
      from HltLine.HltReco import PV3D
      
      PhiCombine = Hlt2Member( CombineParticles
                               , "PhiCombine"
                               , DecayDescriptor = "phi(1020) -> K+ K-"
                               , InputLocations = [NoCutsKaons]
                               , CombinationCut = "(ADAMASS('phi(1020)')<75*MeV)"
                               , MotherCut = "(ADMASS('phi(1020)')<50*MeV)"\
                               " & (BPVVDCHI2>60) & (MIPDV(PRIMARY)<0.5) & (VFASPF(VCHI2) < 20)"
                               , DaughtersCuts = {"K+":"(PT>300*MeV) & (P>2*GeV)"\
                                                  " & (MIPDV(PRIMARY) < 0.5) &  (BPVIPCHI2() > 20)"}
                               )

      DsCombine = Hlt2Member( CombineParticles
                              , "DsCombine"
                              , DecayDescriptor = "[D_s+ -> pi+ phi(1020)]cc"
                              , InputLocations = [NoCutsPions, PhiCombine]
                              , CombinationCut = "(ADAMASS('D_s+')<75*MeV)"
                              , MotherCut = "(ADMASS('D_s+')<50*MeV) & (BPVDIRA>0.9999)"\
                              " & (BPVVDCHI2>85) & (MIPDV(PRIMARY)<0.1) &  (VFASPF(VCHI2) < 10)"
                              , DaughtersCuts = {"pi+":"(PT>300*MeV) & (P>2*GeV)"\
                                              " & (MIPDV(PRIMARY) >0.1) & (BPVIPCHI2() > 20)"}
                              )
      
      line = Hlt2Line('ExpressDs2PhiPi'
                      , prescale = self.prescale 
                      , algos = [ PV3D, NoCutsPions, NoCutsKaons, PhiCombine, DsCombine]
                      , postscale = self.postscale
                      )
      
      #--------------------------------------------      
      '''
      selection of beam halo tracks
      '''
      from Configurables import Tf__PatVeloAlignTrackFilter as PatVeloAlignTrackFilter
      from HltLine.HltDecodeRaw import DecodeVELO

      Hlt2Line('ExpressBeamHalo'
               ,prescale = self.prescale
               , algos = [ DecodeVELO
                           , PatVeloAlignTrackFilter('Hlt2ExpressBeamHaloDecision') 
                           ]
               , postscale = self.postscale
               )
      
      #--------------------------------------------      
      
      
      HltANNSvc().Hlt2SelectionID.update( { "Hlt2ExpressJPsiDecision"   : 50090
                                            ,'Hlt2ExpressJPsiTagProbeDecision' : 50091
                                            ,'Hlt2ExpressLambdaDecision' : 50092
                                            ,'Hlt2ExpressKSDecision' : 50093
                                            , 'Hlt2ExpressDs2PhiPiDecision' : 50094
                                            ,"Hlt2ExpressBeamHaloDecision"     : 50095
                                            } )

