# =============================================================================
# $Id: Hlt2ExpressLines.py,v 1.16 2010-03-20 11:35:03 albrecht Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.16 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class Hlt2ExpressLinesConf(HltLinesConfigurableUser):
   
   __slots__ = { 'Prescale'  : { 'Hlt2ExpressJPsi'        : 1.
                               , 'Hlt2ExpressJPsiTagProbe': 1.
                               , 'Hlt2ExpressLambda'      : 1.
                               , 'Hlt2ExpressKS'          : 1.
                               , 'Hlt2ExpressDs2PhiPi'    : 1.
                               , 'Hlt2ExpressBeamHalo'    : 1.
                               }
               , 'Postscale' : { 'Hlt2ExpressJPsi'        : 'RATE(5)'
                               , 'Hlt2ExpressJPsiTagProbe': 'RATE(5)'
                               , 'Hlt2ExpressLambda'      : 'RATE(1)'
                               , 'Hlt2ExpressKS'          : 'RATE(1)'
                               , 'Hlt2ExpressDs2PhiPi'    : 'RATE(1)'
                               , 'Hlt2ExpressBeamHalo'    : 'RATE(1)'
                               }
               , 'ExJPsiMassWindow'        :  120   # MeV
               , 'ExJPsiPt'                : 1000   # MeV
               , 'ExJPsiMuPt'              :  500   # MeV
               , 'ExJPsiTPMassWindow'      :  300   # MeV
               , 'ExJPsiTPVChi2'           :   20
               , 'ExJPsiTPCoP'             : 3000   # MeV
               , 'ExJPsiTPCoPt'            :  800   # MeV
               , 'ExJPsiTPTagP'            : 6000   # MeV
               , 'ExJPsiTPTagPt'           : 1500   # MeV
               , 'ExJPsiTPMaxEcalE'        : 1000   # MeV
               , 'ExJPsiTPMinEcalE'        :  -10   # MeV
               , 'ExJPsiTPMaxHcalE'        : 4000   # MeV
               , 'ExJPsiTPMinHcalE'        : 1000   # MeV
               , 'ExLambdaMassWin'         :   20   # MeV
               , 'ExLambdaMassWinWide'     :   30   # MeV
               , 'ExLambdaMinDz'           :   50   #  mm
               , 'ExLambdaMaxDz'           :  600   #  mm
               , 'ExLambdaDira'            : 0.9999995
               , 'ExLambdaPiP'             : 3000   # MeV
               , 'ExLambdaPiPt'            :  100   # MeV
               , 'ExLambdaPiIPChi2'        :    9
               , 'ExLambdaPP'              : 3000   # MeV
               , 'ExLambdaPPt'             :  100   # MeV
               , 'ExLambdaPIPChi2'         :    9
               , 'ExKSNu1'                 :    2   
               , 'ExKSMassWinWide'         :  150   # MeV
               , 'ExKSMassWin'             :  100   # MeV
               , 'ExPhiMassWinWide'        :   50 # MeV
               , 'ExPhiMassWin'            :   30 # MeV
               , 'ExPhiDOCAMax'		   :  0.135 # mm
	       , 'ExPhiMIPCHI2DV'          :  12.18 #log(2.5) = 12.18
               , 'ExPhiKPt'                :  300 # MeV
               , 'ExPhiKP'                 : 1000 # MeV
               , 'ExPhiKMIPCHI2DV'         :  1.0
	       , 'ExDsMassWinWide'         :  100 # MeV
               , 'ExDsMassWin'             :   50 # MeV
               , 'ExDsBPVDIRA'             :    0.9999
               , 'ExDsVCHI2'               :   12.18 #log(2.5) = 12.18
	       , 'ExDsMIPCHI2DV'	   :   12.18 #log(2.5) = 12.18
               , 'ExDsMIPDV'               :  0.05
	       , 'ExDsPiPt'                :  300 # MeV
               , 'ExDsPiP'                 : 1000 # MeV
               , 'ExDsPiMIPCHI2DV'         :  12.18 #log(2.5) = 12.18
               , 'ExHaloMinTot'            :  20 
               , 'ExHaloMaxTot'            : 5000 
               , 'ExHaloMinRSlice'         :  10 
               , 'ExHaloMinCell'           :  10
               , 'ExHaloMaxCell'           :  100
               , 'ExHaloMaxDiffCell'       :  -1
               , 'ExHaloMaxSensor'         :  25 
               , 'ExHaloOverlaps'          :  False
               , 'ExHaloBigCell'           :  False
               , 'ExHaloMinOverlap'        :  3
               }  
   
   def __apply_configuration__(self):
      from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
      from HltTracking.HltPVs import PV3D
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
                             , Code = "(ADMASS('J/psi(1S)')<%(ExJPsiMassWindow)d*MeV)"\
                             " & (PT>%(ExJPsiPt)d*MeV)"\
                             " & (MINTREE('mu-'==ABSID,PT)>%(ExJPsiMuPt)d*MeV) " %  self.getProps() 
                             , InputLocations  = [ DiMuon ]
                             , InputPrimaryVertices = "None"
                             , UseP2PVRelations = False
                             )
      
      line = Hlt2Line('ExpressJPsi'
                      , prescale = self.prescale 
                      , algos = [ DiMuon, filter ]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      JPsi from only one muon (tag and probe)
      from StrippingMuIDCalib.py

      '''
      from Hlt2SharedParticles.BasicParticles import Muons, NoCutsPions
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D
      from HltTracking.Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
      Hlt2UnfittedForwardTracking = Hlt2UnfittedForwardTracking()	     
 
     ############################
      # Prompt Jpsi tag-and-probe
     #############################
      
      # common cut
      cocut = "(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))"\
      " & (P>%(ExJPsiTPCoP)d*MeV) &(PT>%(ExJPsiTPCoPt)d*MeV)"%  self.getProps()
      
      #Tag and probe cuts:
      #   TAG::  P>6Gev and Pt>1.5 GeV
      #   PROBE:: Ecal (-10,1000) and Hcal (1000,4000) energy filtered
      tag1cuts = "  (CHILDCUT((P>%(ExJPsiTPTagP)d*MeV),1))"\
                 " & (CHILDCUT((PT>%(ExJPsiTPTagPt)d*MeV),1)) "%  self.getProps()
      tag2cuts = "  (CHILDCUT((P>%(ExJPsiTPTagP)d*MeV),2))"\
                 " & (CHILDCUT((PT>%(ExJPsiTPTagPt)d*MeV),2)) "%  self.getProps()
      
      probe2cuts = " ( "\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<%(ExJPsiTPMaxEcalE)d*MeV),2)) &"\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<%(ExJPsiTPMaxHcalE)d*MeV),2)) &"\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>%(ExJPsiTPMinEcalE)d*MeV),2)) &"\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>%(ExJPsiTPMinHcalE)d*MeV),2)) ) "%  self.getProps()
      probe1cuts = " ( "\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<%(ExJPsiTPMaxEcalE)d*MeV),1)) &"\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<%(ExJPsiTPMaxHcalE)d*MeV),1)) &"\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>%(ExJPsiTPMinEcalE)d*MeV),1)) &"\
      "(CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>%(ExJPsiTPMinHcalE)d*MeV),1)) ) "%  self.getProps()
      
      child1cuts = tag1cuts + " & " + probe2cuts
      
      child2cuts = tag2cuts + " & " + probe1cuts
      
     
      
      JPsiCombine = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , InputLocations = [ NoCutsPions, Muons ]
                                , DecayDescriptor = '[J/psi(1S) -> mu+ pi-]cc'
                                , DaughtersCuts = { 'mu+' : cocut ,
                                                    'pi-' : cocut }
                                , CombinationCut = "(ADAMASS('J/psi(1S)')<%(ExJPsiTPMassWindow)d*MeV)"%  self.getProps()
                                , MotherCut = "( " + child1cuts + " | " + child2cuts + " ) "\
                                " & (VFASPF(VCHI2/VDOF)<%(ExJPsiTPVChi2)d)"%  self.getProps()
                                )

      caloProtos = Hlt2UnfittedForwardTracking.hlt2ChargedHadronProtos()
      
      line = Hlt2Line('ExpressJPsiTagProbe'
                      , prescale = self.prescale
                      , HLT = "HLT_PASS_RE('Hlt1.*SingleMuon.*Decision')"
                      , algos = [ PV3D(), caloProtos, Muons, NoCutsPions, JPsiCombine ]
                      , postscale = self.postscale
                      )
      
      #--------------------------------------------
      '''
      Lambda no PID
      from StrippingLambdaNoPID.py
      '''
      from Hlt2SharedParticles.BasicParticles import NoCutsPions,NoCutsProtons
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D
     
      LambdaCombine = Hlt2Member( CombineParticles
                                  , "LambdaCombine"
                                  , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                  , InputLocations = [NoCutsPions,NoCutsProtons ]
                                  , CombinationCut = "(ADAMASS('Lambda0')<%(ExLambdaMassWinWide)d*MeV)"%  self.getProps()
                                  , MotherCut = "(ADMASS('Lambda0')<%(ExLambdaMassWin)d*MeV)"\
                                  " & (%(ExLambdaMinDz)d*mm<BPVVDZ)"\
                                  " & (%(ExLambdaMaxDz)d*mm>BPVVDZ)"\
                                  " & (BPVDIRA>%(ExLambdaDira)d) "%  self.getProps()
                                  , DaughtersCuts = { "p+"  :  "(P>%(ExLambdaPP)d*MeV)"\
                                                      " & (PT>%(ExLambdaPPt)d*MeV)"\
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExLambdaPIPChi2)d) "%  self.getProps(),
                                                      "pi-"  :  "(P>%(ExLambdaPiP)d*MeV)"\
                                                      " & (PT>%(ExLambdaPiPt)d*MeV)"\
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExLambdaPiIPChi2)d) "%  self.getProps(),
                                                      }
                                  )
      
      line = Hlt2Line('ExpressLambda'
                      , prescale = self.prescale 
                      , algos = [ PV3D(), NoCutsPions, NoCutsProtons, LambdaCombine ]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      Ks no PID
      '''
      from Hlt2SharedParticles.BasicParticles import NoCutsPions,NoCutsProtons
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D

      KsCombine = Hlt2Member( CombineParticles
                              , "KsCombine"
                              , DecayDescriptor = "[KS0 -> pi+ pi-]cc"
                              , InputLocations = [NoCutsPions]
                              , CombinationCut = "(ADAMASS('KS0') < %(ExKSMassWinWide)d*MeV)"%  self.getProps()
                              , MotherCut = "(ADMASS('KS0') < %(ExKSMassWin)d*MeV)"\
                              " & (log((CHILD(MIPDV(PRIMARY), 1)) * (CHILD(MIPDV(PRIMARY), 2) )"\
                              " / (MIPDV(PRIMARY) ) )  > %(ExKSNu1)d*mm )"%  self.getProps()
                              )
      
      line = Hlt2Line('ExpressKS'
                      , prescale = self.prescale 
                      , algos = [ PV3D(), NoCutsPions, KsCombine]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      Phi no PID

      TODO JA 18. March
      Updated selection, please check!

      '''
      from Hlt2SharedParticles.BasicParticles import NoCutsKaons
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D

      PhiCombine = Hlt2Member( CombineParticles
                               , "PhiCombine"
                               , DecayDescriptor = "phi(1020) -> K+ K-"
                               , InputLocations = [NoCutsKaons]
                               , CombinationCut = "(ADAMASS('phi(1020)')<%(ExPhiMassWinWide)d*MeV)"%  self.getProps()
                               , MotherCut = "(ADMASS('phi(1020)') < %(ExPhiMassWin)d*MeV)"\
                               " & (DOCAMAX < %(ExPhiDOCAMax)d*mm)"\
			       " & (MIPCHI2DV(PRIMARY) > %(ExPhiMIPCHI2DV)d)"%  self.getProps()
                              , DaughtersCuts = {"K+":"(PT>%(ExPhiKPt)d*MeV)"\
                                                  " & (P>%(ExPhiKP)d*MeV)"\
						  " & (MIPCHI2DV(PRIMARY) > %(ExPhiKMIPCHI2DV)d)"%  self.getProps()}
                               )#

      DsCombine = Hlt2Member( CombineParticles
                              , "DsCombine"
                              , DecayDescriptor = "[D_s+ -> pi+ phi(1020)]cc"
                              , InputLocations = [NoCutsPions, PhiCombine]
                              , CombinationCut = "(ADAMASS('D_s+')<%(ExDsMassWinWide)d*MeV)"%  self.getProps()
                              , MotherCut = "(ADMASS('D_s+')<%(ExDsMassWin)d*MeV)"\
                              " & (BPVDIRA > %(ExDsBPVDIRA)d)"\
                              " & (VFASPF(VCHI2) < %(ExDsVCHI2)d)"\
			      " & (MIPCHI2DV(PRIMARY) < %(ExDsMIPCHI2DV)d)"\
			      " & (MIPDV(PRIMARY) < %(ExDsMIPDV)d)"%  self.getProps()
                              , DaughtersCuts = {"pi+":"(PT > %(ExDsPiPt)d*MeV)"\
                                                 " & (P > %(ExDsPiP)d*MeV)"\
						 " & (MIPCHI2DV(PRIMARY) > %(ExDsPiMIPCHI2DV)d)"%  self.getProps()}
                              )
      
      line = Hlt2Line('ExpressDs2PhiPi'
                      , prescale = self.prescale 
                      , algos = [ PV3D(), NoCutsPions, NoCutsKaons, PhiCombine, DsCombine]
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
                           , PatVeloAlignTrackFilter('Hlt2ExpressBeamHaloDecision'
                                                     , MinTot = self.getProp('ExHaloMinTot')  
                                                     , MaxTot = self.getProp('ExHaloMaxTot')  
                                                     , MinRSlice = self.getProp('ExHaloMinRSlice')  
                                                     , MinCell = self.getProp('ExHaloMinCell') 
                                                     , MaxCell = self.getProp('ExHaloMaxCell') 
                                                     , MaxDiffCell = self.getProp('ExHaloMaxDiffCell') 
                                                     , MaxSensor = self.getProp('ExHaloMaxSensor')  
                                                     , Overlaps = self.getProp('ExHaloOverlaps') 
                                                     , BigCell = self.getProp('ExHaloBigCell') 
                                                     , MinOverlap = self.getProp('ExHaloMinOverlap')
                                                     ) 
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

