# =============================================================================
# $Id: Hlt2ExpressLines.py,v 1.30 2010-08-23 16:36:34 raaij Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.30 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *
from GaudiKernel.SystemOfUnits        import mm, cm, MeV, GeV

def __monitor__ ( variable ,  label , center , window, id =None, nbins = 100 ) :
      return "process ( monitor( %s,  Gaudi.Histo1DDef( '%s', %s , %s, %s), '%s' ) ) >> ~EMPTY " % (variable, label, center-window,center+window, nbins, id if id else variable )

class Hlt2ExpressLinesConf(HltLinesConfigurableUser):
   
   __slots__ = { 'Prescale'  : { 'Hlt2ExpressJPsi'        : 1.
                               , 'Hlt2ExpressJPsiTagProbe': 1.
                               , 'Hlt2ExpressLambda'      : 1.
                               , 'Hlt2ExpressKS'          : 1.
                               , 'Hlt2ExpressDs2PhiPi'    : 1.
                               , 'Hlt2ExpressBeamHalo'    : 1.
                               , 'Hlt2ExpressDStar2D0Pi'  : 1.
                               , 'Hlt2ExpressD02KPi'      : 1.
                               , 'Hlt2ExpressHLT1Physics' : 1.
                                 }
               , 'Postscale' : { 'Hlt2ExpressJPsi'        : 'RATE(5)'
                               , 'Hlt2ExpressJPsiTagProbe': 'RATE(1)'
                               , 'Hlt2ExpressLambda'      : 'RATE(1)'
                               , 'Hlt2ExpressKS'          : 'RATE(1)'
                               , 'Hlt2ExpressDs2PhiPi'    : 'RATE(1)'
                               , 'Hlt2ExpressBeamHalo'    : 'RATE(2)'
                               , 'Hlt2ExpressDStar2D0Pi'  : 'RATE(5)'
                               , 'Hlt2ExpressD02KPi'      : 'RATE(5)'
                               , 'Hlt2ExpressHLT1Physics' : 'RATE(1)'
                                 }
                 , 'TrackChi2'               :    4
                 , 'ExJPsiMassWindow'        :   80   # MeV
                 , 'ExJPsiTrkChi2'           :    5
                 , 'ExJPsiVChi2'             :   10
                 , 'ExJPsiPt'                : 1000   # MeV
                 , 'ExJPsiMuPt'              :  500   # MeV
                 , 'ExJPsiMuIPChi2'          :   25
                 , 'ExJPsiTPMassWindow'      :  200   # MeV
                 , 'ExJPsiTPVChi2'           :    5
                 , 'ExJPsiTDVChi2'           :  225
                 , 'ExJPsiTPCoP'             : 3000   # MeV
                 , 'ExJPsiTPCoPt'            :  800   # MeV
                 , 'ExJPsiTPCoTrkChi2'       :    3   
                 , 'ExJPsiTPCoTrkMIPChi2'    :   15 
                 , 'ExJPsiTPTagTrkMIPChi2'   :   25  
                 , 'ExJPsiTPTagP'            : 6000   # MeV
                 , 'ExJPsiTPTagPt'           : 1500   # MeV
                 , 'ExLambdaVChi2'           :    9
                 , 'ExLambdaVZ'              :  220*cm 
                 , 'ExLambdaLTimeChi2'       :   36
                 , 'ExLambdaCTau'            :    5*mm
                 , 'ExLambdaWSMassKS'        :   20   # MeV 
                 , 'ExLambdaMassWin'         :   25   # MeV
                 , 'ExLambdaMassWinWide'     :   30   # MeV
                 , 'ExLambdaPiP'             : 2000   # MeV
                 , 'ExLambdaPiIPChi2'        :   25
                 , 'ExLambdaPiTrackChi2'     :    5  
                 , 'ExLambdaPP'              : 2000   # MeV
                 , 'ExLambdaPIPChi2'         :   25
                 , 'ExLambdaPTrackChi2'      :    5  
                 , 'ExKSMassWinWide'         :   70   # MeV
                 , 'ExKSMassWin'             :   50   # MeV
                 , 'ExKSMaxZ'                :  220*cm   
                 , 'ExKSLTimeChi2'           :   36
                 , 'ExKSCTau'                :    1*mm
                 , 'ExKSWrongMass'           :    9   # MeV
                 , 'ExKSTrackP'              : 2000   # MeV
                 , 'ExKSTrackChi2'           :    5
                 , 'ExKSTrackMinIPChi2'      :   25
                 , 'ExPhiMassWinWide'        :   70 # MeV
                 , 'ExPhiMassWin'            :   50 # MeV
                 , 'ExPhiDOCAMax'		   :  10. # mm
                 , 'ExPhiMIPCHI2DV'          :  2.18 #log(2.5) = 12.18
                 , 'ExPhiKPt'                :  300 # MeV
                 , 'ExPhiKP'                 : 1000 # MeV
                 , 'ExPhiKMIPCHI2DV'         :  1.0
                 , 'ExDsMassWinWide'         :  100 # MeV
                 , 'ExDsMassWin'             :   50 # MeV
                 , 'ExDsBPVDIRA'             :    0.999
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
                 , 'ExTrVeloOverlapsR'       :  25
                 , 'ExTrVeloOverlapsPhi'     :  25
                 , 'ExD0MassWinWide'         :  100   # MeV
                 , 'ExD0MassWin'             :   50   # MeV
                 , 'ExD0VCHI2'               :   10 
                 , 'ExD0Pt'                  : 1000   # MeV
                 , 'ExD0BPVDIRA'             : 0.9999
                 , 'ExD0BPVVDCHI2'           :   10
                 , 'ExD0KP'                  : 3000   # MeV
                 , 'ExD0KPt'                 :  600   # MeV
                 , 'ExD0KIPChi2'             :    5
                 , 'ExD0PiP'                 : 3000   # MeV
                 , 'ExD0PiPt'                :  600   # MeV
                 , 'ExD0PiIPChi2'            :    5
                 , 'ExDStarMassWinWide'      :  100   # MeV
                 , 'ExDStarMassWin'          :   50   # MeV
                 , 'ExDStarPt'               : 2200   # MeV
                 , 'ExDStarVCHI2'            :   15
                 , 'ExDStarMassDiff'         :155.5   # MeV
                 , 'ExDStarPiPt'             : 110    # MeV
                 , 'ExDStarPiIPChi2'         :   2
                 , 'ExD0PromptTrP'           : 5000   # MeV
                 , 'ExD0PromptTrPt'          :  800   # MeV
                 , 'ExD0PromptTrIPChi2'      :  2.0
                 , 'ExD0PromptTrCHI2DOFMAX'  :  3.0
                 , 'ExD0PromptMassWinWide'   :  100   # MeV
                 , 'ExD0PromptD0MassWin'     :   50   # MeV
                 , 'ExD0PromptTrkMaxAPTMIN'  : 1500   # MeV
                 , 'ExD0PromptD0PTMIN'       : 2000   # MeV
                 , 'ExD0PromptPairAMINDOCAMAX': 0.10 * mm
                 , 'ExD0PromptD0VCHI2PDOF'   :   10
                 , 'ExD0PromptD0BPVDIRA'     : 0.99985
                 , 'ExD0PromptD0BPVVDCHI2'   :   25
                 #
                 ,'HltANNSvcID'              : {
                                              'ExpressD02KPi'           :  50098                                      
                                              }
                 }  
   
   
   def __apply_configuration__(self):
      from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
      from HltLine.Hlt2Monitoring import Hlt2Monitor,Hlt2MonitorMinMax
      from HltTracking.HltPVs import PV3D
      from Configurables import HltANNSvc
      from Hlt2SharedParticles.TrackFittedDiMuon import TrackFittedDiMuon
      from Configurables import FilterDesktop
      from HltTracking.HltPVs import PV3D
      '''
      Comment content of express stream here
      '''
      
      #--------------------------------------------
      '''
      unbiased JPsi - prescaled to ~5 Hz
      '''
      
      filter = Hlt2Member(   FilterDesktop 
                             , "Filter"
                             , Code = " (ADMASS('J/psi(1S)')<%(ExJPsiMassWindow)s*MeV)"\
                             " & (PT>%(ExJPsiPt)s*MeV)"\
                             " & (VFASPF(VCHI2PDOF)<%(ExJPsiVChi2)s)"\
                             " & (MINTREE('mu-'==ABSID,MIPCHI2DV(PRIMARY))>%(ExJPsiMuIPChi2)s )"\
                             " & (MINTREE('mu-'==ABSID,TRCHI2DOF)<%(ExJPsiTrkChi2)s)"\
                             " & (MINTREE('mu-'==ABSID,PT)>%(ExJPsiMuPt)s*MeV) " %  self.getProps() 
                             , Inputs  = [ TrackFittedDiMuon ]
                             , PreMonitor  =  Hlt2Monitor( "M","M(#mu#mu)",3097,self.getProp("ExJPsiMassWindow"),'M_in',nbins=101) 
                             , PostMonitor =  Hlt2Monitor( "M","M(#mu#mu)",3097,self.getProp("ExJPsiMassWindow"),'M_out',nbins=101)
                             )
      
      line = Hlt2Line('ExpressJPsi'
                      , prescale = self.prescale 
                      , algos = [  PV3D(), TrackFittedDiMuon, filter ]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      JPsi from only one muon (tag and probe)
      from StrippingMuIDCalib.py

      '''
      from Hlt2SharedParticles.TagAndProbeParticles import TagAndProbeMuons, TagAndProbePions
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D
 
     ############################
      # Prompt Jpsi tag-and-probe
     #############################
      
      # common cut
      cocut = "(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))"\
          " & (P>%(ExJPsiTPCoP)s*MeV) & (PT>%(ExJPsiTPCoPt)s*MeV)"\
          " & (TRCHI2DOF<%(ExJPsiTPCoTrkChi2)s)"\
          " & (MIPCHI2DV(PRIMARY)>%(ExJPsiTPCoTrkMIPChi2)s)"%  self.getProps()

      tag1cuts = "(  (CHILDCUT((P>%(ExJPsiTPTagP)s*MeV),1)) & (CHILDCUT((PT>%(ExJPsiTPTagPt)s*MeV),1))"\
          " & (CHILDCUT((MIPCHI2DV(PRIMARY)>%(ExJPsiTPTagTrkMIPChi2)s),1)) )"%  self.getProps()
      tag2cuts = "(  (CHILDCUT((P>%(ExJPsiTPTagP)s*MeV),2)) & (CHILDCUT((PT>%(ExJPsiTPTagPt)s*MeV),2))"\
          " & (CHILDCUT((MIPCHI2DV(PRIMARY)>%(ExJPsiTPTagTrkMIPChi2)s),2)) )"%  self.getProps()

      child1cuts = tag1cuts 
      child2cuts = tag2cuts 
     
      JPsiCombine = Hlt2Member( CombineParticles
                                , 'JPsiCombine'
                                , Inputs = [ TagAndProbePions, TagAndProbeMuons ]
                                , DecayDescriptor = '[J/psi(1S) -> mu+ pi-]cc'
                                , DaughtersCuts = { 'mu+' : cocut ,
                                                    'pi-' : cocut }
                                , CombinationCut = "(ADAMASS('J/psi(1S)')<%(ExJPsiTPMassWindow)s*MeV)"%  self.getProps()
                                , MotherCut = "( " + child1cuts + " | " + child2cuts + " ) "\
                                " & (VFASPF(VCHI2PDOF)<%(ExJPsiTPVChi2)s) & (BPVVDCHI2>%(ExJPsiTDVChi2)s)"%  self.getProps()
                                , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",2000,5000 )
                                )

      line = Hlt2Line('ExpressJPsiTagProbe'
                      , prescale = self.prescale
                      , HLT = "HLT_PASS_RE('Hlt1.*SingleMuon.*Decision')"
                      , algos = [ PV3D(), TagAndProbeMuons, TagAndProbePions, JPsiCombine ]
                      , postscale = self.postscale
                      )
      
      #--------------------------------------------
      '''
      Lambda no PID
      from StrippingLambdaNoPID.py
      '''
      from Hlt2SharedParticles.TrackFittedBasicParticles import ( BiKalmanFittedProtons,
                                                                  BiKalmanFittedPions )
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D
     
      LambdaCombine = Hlt2Member( CombineParticles
                                  , "LambdaCombine"
                                  , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                  , Preambulo        = [ "from GaudiKernel.PhysicalConstants import c_light "]
                                  , Inputs = [BiKalmanFittedPions,BiKalmanFittedProtons ]
                                  , CombinationCut = "(ADAMASS('Lambda0')<%(ExLambdaMassWinWide)s*MeV)"%  self.getProps()
                                  , MotherCut = "(ADMASS('Lambda0')<%(ExLambdaMassWin)s*MeV)"\
                                  " & in_range ( 0 , VFASPF ( VCHI2 ) ,  %(ExLambdaVChi2)s   )" \
                                  " & ( VFASPF ( VZ  ) < %(ExLambdaVZ)s)" \
                                  " & in_range ( 0 , BPVLTFITCHI2()   ,  %(ExLambdaLTimeChi2)s )" \
                                  " & ( BPVLTIME()*c_light > %(ExLambdaCTau)s)" \
                                  " & ( ADWM( 'KS0' , WM( 'pi+' , 'pi-') )  > %(ExLambdaWSMassKS)s)"  %  self.getProps()
                                  , DaughtersCuts = { "p+"  :  "(P>%(ExLambdaPP)s)"\
                                                      " & (ISLONG) " \
                                                      " & (TRCHI2DOF < %(ExLambdaPTrackChi2)s) " \
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExLambdaPIPChi2)s) "%  self.getProps(),
                                                      "pi-"  :  "(P>%(ExLambdaPiP)s)"\
                                                      " & (ISLONG) " \
                                                      " & (TRCHI2DOF < %(ExLambdaPiTrackChi2)s) " \
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExLambdaPiIPChi2)s) "%  self.getProps(),
                                                      }
                                  , MotherMonitor  =  Hlt2Monitor("M", "M(p#pi)",1116,self.getProp("ExLambdaMassWin"))
                                  )
      
      line = Hlt2Line('ExpressLambda'
                      , prescale = self.prescale
                      , algos = [ PV3D(), BiKalmanFittedPions, BiKalmanFittedProtons, LambdaCombine ]
                      , postscale = self.postscale
                      )
      #--------------------------------------------
      '''
      Ks no PID
      '''
      from Configurables                 import CombineParticles
      from HltTracking.HltPVs            import PV3D
      from GaudiKernel.PhysicalConstants import c_light

      KsCombine = Hlt2Member( CombineParticles
                              , "KsCombine"
                              , Preambulo        = [ "from GaudiKernel.PhysicalConstants import c_light "]
                              , DecayDescriptor  = "KS0 -> pi+ pi-"
                              , Inputs   = [BiKalmanFittedPions]
                              , DaughtersCuts    = {"pi+" : "(P > %(ExKSTrackP)s) & (ISLONG) & (TRCHI2DOF < %(ExKSTrackChi2)s) " \
                              " & (MIPCHI2DV(PRIMARY) > %(ExKSTrackMinIPChi2)s) "%  self.getProps() }
                              , CombinationCut   = "(ADAMASS('KS0') < %(ExKSMassWinWide)s )"%  self.getProps()
                              , MotherCut = "(ADMASS('KS0') < %(ExKSMassWin)s )" \
                              "& (VFASPF (VZ) < %(ExKSMaxZ)s) " \
                              "& in_range(0, BPVLTFITCHI2(), %(ExKSLTimeChi2)s )" \
                              "& ( BPVLTIME()*c_light > %(ExKSCTau)s)"\
                              "& (ADWM( 'Lambda0' , WM( 'p+' , 'pi-') ) >%(ExKSWrongMass)s)" \
                              "& (ADWM( 'Lambda0' , WM( 'pi+' , 'p~-') ) > %(ExKSWrongMass)s)" %  self.getProps()
                              , MotherMonitor  =  Hlt2Monitor("M", "M(#pi#pi)",498,self.getProp("ExKSMassWin"))
                              )
      
      line = Hlt2Line('ExpressKS'
                      , prescale = self.prescale
                      , algos = [ PV3D(), BiKalmanFittedPions, KsCombine]
                      , postscale = self.postscale
                      )

      #--------------------------------------------
      '''
      Phi no PID

      TODO JA 18. March
      Updated selection, please check!

      '''
      from Hlt2SharedParticles.TrackFittedBasicParticles import ( BiKalmanFittedKaons,
                                                                  BiKalmanFittedPions )
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D

      PhiCombine = Hlt2Member( CombineParticles
                               , "PhiCombine"
                               , DecayDescriptor = "phi(1020) -> K+ K-"
                               , Inputs = [BiKalmanFittedKaons]
                               , CombinationCut = "(ADAMASS('phi(1020)')<%(ExPhiMassWinWide)s*MeV)"%  self.getProps()
                               , MotherCut = "(ADMASS('phi(1020)') < %(ExPhiMassWin)s*MeV)"\
                               " & (DOCAMAX < %(ExPhiDOCAMax)s*mm)"\
			       " & (MIPCHI2DV(PRIMARY) > %(ExPhiMIPCHI2DV)s)"%  self.getProps()
                              , DaughtersCuts = {"K+":"(PT>%(ExPhiKPt)s*MeV)"\
                                                  " & (P>%(ExPhiKP)s*MeV)"\
						  " & (MIPCHI2DV(PRIMARY) > %(ExPhiKMIPCHI2DV)s)"\
                                                 "& (TRCHI2DOF<%(TrackChi2)s) "%  self.getProps()}
                             , MotherMonitor  =  Hlt2Monitor("M", "M(KK)",1020,self.getProp("ExPhiMassWin"))
                             )

      DsCombine = Hlt2Member( CombineParticles
                              , "DsCombine"
                              , DecayDescriptor = "[D_s+ -> pi+ phi(1020)]cc"
                              , Inputs = [BiKalmanFittedPions, PhiCombine]
                              , CombinationCut = "(ADAMASS('D_s+')<%(ExDsMassWinWide)s*MeV)"%  self.getProps()
                              , MotherCut = "(ADMASS('D_s+')<%(ExDsMassWin)s*MeV)"\
                              " & (BPVDIRA > %(ExDsBPVDIRA)s)"\
                              " & (VFASPF(VCHI2) < %(ExDsVCHI2)s)"\
			      " & (MIPCHI2DV(PRIMARY) < %(ExDsMIPCHI2DV)s)"\
			      " & (MIPDV(PRIMARY) < %(ExDsMIPDV)s)"%  self.getProps()
                              , DaughtersCuts = {"pi+":"(PT > %(ExDsPiPt)s*MeV)"\
                                                 " & (P > %(ExDsPiP)s*MeV)"\
						 " & (MIPCHI2DV(PRIMARY) > %(ExDsPiMIPCHI2DV)s)"%  self.getProps()}
                              , MotherMonitor  =  Hlt2Monitor( "M","M(#phi#pi)",1968,self.getProp("ExDsMassWin"))
                              )
      
      line = Hlt2Line('ExpressDs2PhiPi'
                      , prescale = self.prescale 
                      , algos = [ PV3D(), BiKalmanFittedPions, BiKalmanFittedKaons, PhiCombine, DsCombine]
                      , postscale = self.postscale
                      )

    
      #--------------------------------------------      
      '''
      selection of beam halo tracks
      '''
      from Configurables import Tf__PatVeloAlignTrackFilter as PatVeloAlignTrackFilter
      from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
      
      from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
      from HltLine.HltDecodeRaw import DecodeVELO
      
      Hlt2BiKalmanFittedForwardTracking = Hlt2BiKalmanFittedForwardTracking()
      hlt2VeloTracking = Hlt2BiKalmanFittedForwardTracking.hlt2VeloTracking()

      Hlt2Line('ExpressBeamHalo'
               ,prescale = self.prescale
               , algos = [ DecodeVELO
                           # , hlt2VeloTracking
                           # , Hlt2Member ( FilterDesktop
                           # , 'ExHaloOverlapFilter'
                           # , Preambulo = [   'from LoKiPhys.decorators import *'
                           #                 , 'from LoKiProtoParticles.decorators import *'
                           #                 , 'from LoKiArrayFunctors.decorators import *'
                           #                 , 'from LoKiCore.functions import *'
                           #                 , 'from LoKiCore.math import *' 
                           #                 , 'from LoKiTracks.functions import *' 
                           #                 , 'from LoKiTracks.decorators import *' 
                           #                 ]
                           #                 , Inputs    = [ hlt2VeloTracking ]
                           #                 , Code      = " (TrNVELOOVERLAPSR>%(ExTrVeloOverlapsR)s)" \
                           #                               " | (TrNVELOOVERLAPSPHI>%(ExTrVeloOverlapsPhi)s)"%  self.getProps()
                           #               )
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
      '''
      D* no PID

      From RichPIDQC/DstarToDzeroPi.py

      '''
      from Hlt2SharedParticles.TrackFittedBasicParticles import ( BiKalmanFittedKaons,
                                                                  BiKalmanFittedPions )
      from Configurables import CombineParticles
      from HltTracking.HltPVs import PV3D
      
      D02KPiCombine = Hlt2Member( CombineParticles
                                  , "D02KPiCombine"
                                  , DecayDescriptor = "[ D0 -> K- pi+ ]cc"
                                  , Inputs = [BiKalmanFittedKaons, BiKalmanFittedPions]
                                  , CombinationCut = "(ADAMASS('D0')<%(ExD0MassWinWide)s*MeV)"%  self.getProps()
                                  , MotherCut = "(ADMASS('D0') < %(ExD0MassWin)s*MeV)"\
                                  " & (VFASPF(VCHI2PDOF) < %(ExD0VCHI2)s)"\
                                  " & (PT > %(ExD0Pt)s*MeV)"\
                                  " & (BPVDIRA > %(ExD0BPVDIRA)s)"\
                                  " & (BPVVDCHI2 > %(ExD0BPVVDCHI2)s)"%  self.getProps()
                                  , DaughtersCuts = { "K-"  :  "(P>%(ExD0KP)s*MeV)"\
                                                      " & (PT>%(ExD0KPt)s*MeV)"\
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExD0KIPChi2)s) "%  self.getProps(),
                                                      "pi+"  :  "(P>%(ExD0PiP)s*MeV)"\
                                                      " & (PT>%(ExD0PiPt)s*MeV)"\
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExD0PiIPChi2)s) "%  self.getProps()
                                                      }
                                  , MotherMonitor  =  Hlt2Monitor("M", "M(K#pi)",1865,self.getProp("ExD0MassWin"))
                                  )

      DStarCombine = Hlt2Member( CombineParticles
                                 , "DStarCombine"
                                 , DecayDescriptor = "[ D*(2010)+ -> D0 pi+ ]cc"
                                 , Inputs = [BiKalmanFittedPions, D02KPiCombine]
                                 , CombinationCut = "(ADAMASS('D*(2010)+')<%(ExDStarMassWinWide)s*MeV)"%  self.getProps()
                                 , MotherCut = "(ADMASS('D*(2010)+')<%(ExDStarMassWin)s*MeV)"\
                                 " & (PT > %(ExDStarPt)s*MeV)"\
                                 " & (VFASPF(VCHI2PDOF) < %(ExDStarVCHI2)s)"\
                                 " & (M-MAXTREE('D0'==ABSID,M)< %(ExDStarMassDiff)s)"%  self.getProps()
                                 , DaughtersCuts = {"pi+":"(PT>%(ExDStarPiPt)s*MeV)"\
                                                    " & (MIPCHI2DV(PRIMARY)>%(ExDStarPiIPChi2)s) "%  self.getProps()
                                                    }
                                 , MotherMonitor  =  Hlt2Monitor("M", "M(D0#pi)",2010,self.getProp("ExDStarMassWin"))
                                 #                  "process ( monitor( M-MAXTREE('D0'=ABSID,M), Gaudi.Histo1DDef( '#Delta M',120,200),'DM') ) >> ~EMPTY "
                                 )

      line = Hlt2Line('ExpressDStar2D0Pi'
                      , prescale = self.prescale 
                      , algos = [ PV3D(), BiKalmanFittedPions, BiKalmanFittedKaons,
                                  D02KPiCombine, DStarCombine ]
                      , postscale = self.postscale
                      )

      #--------------------------------------------      
      '''
      D0->KPi

      Take D0->KPi trigger line from Charm and apply a postscale

      '''
      
      D02KPiPromptCombine = Hlt2Member( CombineParticles
                                     , "PromptD0Combine"
                                     , DecayDescriptor = "[ D0 -> K- pi+ ]cc"
                                     , Inputs = [BiKalmanFittedKaons, BiKalmanFittedPions]
                                     , CombinationCut =  "(ADAMASS('D0')<%(ExD0PromptMassWinWide)s*MeV)" \
                                       " & ((APT1 > %(ExD0PromptTrkMaxAPTMIN)s) " \
                                       "| (APT2 > %(ExD0PromptTrkMaxAPTMIN)s))" \
                                       "& (APT > %(ExD0PromptD0PTMIN)s)" \
                                       "& (AMINDOCA('LoKi::TrgDistanceCalculator') " \
                                       "< %(ExD0PromptPairAMINDOCAMAX)s )" %  self.getProps()
                                     , MotherCut = "(ADMASS('D0') < %(ExD0PromptD0MassWin)s*MeV)"\
                                     " & (VFASPF(VCHI2PDOF) < %(ExD0PromptD0VCHI2PDOF)s)"\
                                     " & (BPVDIRA > %(ExD0PromptD0BPVDIRA)s)"\
                                     " & (BPVVDCHI2 > %(ExD0PromptD0BPVVDCHI2)s)"%  self.getProps()
                                     , DaughtersCuts = { "K-"  :  "(P>%(ExD0PromptTrP)s*MeV)"\
                                                         "& (TRCHI2DOF< %(ExD0PromptTrCHI2DOFMAX)s)"\
                                                      " & (PT>%(ExD0PromptTrPt)s*MeV)"\
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExD0PromptTrIPChi2)s) "%  self.getProps(),
                                                      "pi+"  :   "(P>%(ExD0PromptTrP)s*MeV)"\
                                                         "& (TRCHI2DOF< %(ExD0PromptTrCHI2DOFMAX)s)"\
                                                      " & (PT>%(ExD0PromptTrPt)s*MeV)"\
                                                      " & (MIPCHI2DV(PRIMARY)>%(ExD0PromptTrIPChi2)s) "%  self.getProps()
                                                      }
                                    , MotherMonitor  =  Hlt2Monitor("M", "M(K#pi)",1865,self.getProp("ExD0PromptD0MassWin"))
                                    )

      line = Hlt2Line('ExpressD02KPi'
                      , prescale = self.prescale 
                      , algos = [ PV3D(), BiKalmanFittedPions, BiKalmanFittedKaons,
                                  D02KPiPromptCombine ]
                      , postscale = self.postscale
                      )
      # line = Hlt2Line('ExpressD02KPi'
      #                 , prescale = self.prescale
      #                 , HLT= "HLT_PASS_SUBSTR('Hlt2CharmHadD02HH_D02KPi')"
      #                 , priority = 255
      #                 , VoidFilter = ''
      #                 , postscale = self.postscale
      #                )
      
      #--------------------------------------------                      
      '''
      Hlt1Physics for Express stream
      '''
      Hlt1Physics = "HLT_PASS_RE('Hlt1(?!Lumi).*Decision')"
      line = Hlt2Line('ExpressHLT1Physics'
                      , prescale = self.prescale
                      , HLT = Hlt1Physics
                      , VoidFilter = '' # explicitly require NO pile up filter... 
                      , postscale = self.postscale
                      )

      #--------------------------------------------                      
      
      HltANNSvc().Hlt2SelectionID.update( { 'Hlt2ExpressJPsiDecision'         : 50090
                                          , 'Hlt2ExpressJPsiTagProbeDecision' : 50091
                                          , 'Hlt2ExpressLambdaDecision'       : 50092
                                          , 'Hlt2ExpressKSDecision'           : 50093
                                          , 'Hlt2ExpressDs2PhiPiDecision'     : 50094
                                          , 'Hlt2ExpressBeamHaloDecision'     : 50095
                                          , 'Hlt2ExpressDStar2D0PiDecision'   : 50096
                                          , 'Hlt2ExpressHLT1PhysicsDecision'  : 50097
                                          } )
      
      HltANNSvc().Hlt2SelectionID.update( { "Hlt2ExpressD02KPiDecision" :
                                              self.getProp('HltANNSvcID')['ExpressD02KPi'] } ) 

