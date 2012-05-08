#!/usr/bi/env python
# =============================================================================
# $Id:$
# =============================================================================
## @file
#
#  Hlt2 dimuon selections
#
#   a) unbiased dimuon selections, i.e. dimuon selections without
#           cuts correlated to the Bs lifetime (J. Albrecht)
#
#   b) biased dimuon and dimuon without PV (Leandro de Paula)
#
#   c) ``multi-muon''-lines:
#        - at least three muons iwth high pt ad high chi2(ip)
#        - at least two dimuons 
#        - dimuon and muon with high-pt high chi2(ip) or
#                               muon and detached dimuon
#        - tau -> 3mu
#
#   c') ``di-muon+(non-muon)''-lines
#        - dimuon + high-pt gamma
#        - dimuon + prompt charm 
#
#  @author J. Albrecht, Leandro de Paula, Antonio Perez-Calero
#  @date 2009-02-13
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""

  Hlt2 dimuon selections

   a) unbiased dimuon selections, i.e. dimuon selections without
           cuts correlated to the Bs lifetime (J. Albrecht)

   b) biased dimuon and dimuon without PV (Leandro de Paula)

   c) ``multi-muon''-lines:
   
        - at least three muons iwth high pt ad high chi2(ip)
        - at leats two dimuons 
        - dimuon and muon iwth high-pt high chi2(ip) or
                              muon and detached dimuon
        - tau -> 3mu
        
   c') ``dimuon + X ''-lines :
   
        - dimuon + high-pt photon 
        - dimuon + prompt charm 
   
"""
# =============================================================================
__version__ = " $Revision$ "
# =============================================================================
from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import GeV,MeV 


class Hlt2InclusiveDiMuonLinesConf(HltLinesConfigurableUser) :
    '''
          Put only the most discriminating variables as slots:
           - prescale factors, mass windows and dimuon pt
           - muon pt _not_ as slot as this strongly modifies the angular acceptances
    '''
    
    
    __slots__ = {  'Prescale'                  : {
        'Hlt2DiMuonDY1'             :  0.1
        }

                   ,'TrChi2'                  :   10      #chi2PerDof 
                   ,'TrChi2Tight'             :    5      #chi2PerDof 
                   ,'UnbiasedDiMuonMinMass'   : 2700      # MeV
                   ,'UnbiasedDiMuonPt'        : 1000      # MeV
                   ,'UnbiasedDiMuonMuPt'      :  500      # MeV
                   ,'UnbiasedDiMuonVertexChi2':   25


                   ,'UnbiasedDiMuonLowMinMass':  500      # MeV
                   ,'UnbiasedDiMuonLowPt'     : 1000      # MeV
                   ,'UnbiasedDiMuonLowMuPt'   :  500      # MeV
                   ,'UnbiasedDiMuonLowChi2'   :   25
                    
                   ,'UnbiasedJPsiMassWindow'  :   70      # MeV
                   ,'UnbiasedJPsiPt'          : 1000      # MeV
                   ,'UnbiasedJPsiMuPt'        :  500      # MeV
                   ,'UnbiasedJPsiVertexChi2'  :   25
                   ,'UnbiasedJPsiHighPt'      : 4000      #MeV
                   ,'UnbiasedJPsiHighPTMassWindow' : 150

                   ,'UnbiasedPsi2SMassWindow' :   70      # MeV
                   ,'UnbiasedPsi2SPt'         : 1000      # MeV
                   ,'UnbiasedPsi2SMuPt'       : 1500      # MeV
                   ,'UnbiasedPsi2SVertexChi2' :   25
                   ,'UnbiasedPsi2SPtHigh'     : 3500      #MeV
                   
                   ,'UnbiasedBmmMinMass'      : 5200      # MeV
                   ,'UnbiasedBmmVertexChi2'   :   25

                   ,'UnbiasedZmmMinMass'      :40000      # MeV
                   ,'UnbiasedZmmPt'           :    0      # MeV

                   ,'DYPt'                    :    0      # MeV
                   ,'DY1MinMass'              : 2500      # MeV
                   ,'DY1MinExcMass'           : 3000      # MeV
                   ,'DY1MaxExcMass'           : 3200      # MeV
                   ,'DY2MinMass'              : 5000      # MeV
                   ,'DY3MinMass'              :10000      # MeV 
                   ,'DY4MinMass'              :20000      # MeV
                   ,'DY1MuPt'                 :  800      # MeV
                   ,'DY2MuPt'                 : 1000      # MeV
                   
                   ,'BiasedSingleMuonPt'      :  700      # MeV
                   ,'BiasedMass'              :  500      # MeV
                   ,'BiasedLMass'             : 1200      # MeV
                   ,'BiasedTMass'             : 2900      # MeV
                   ,'BiasedSingleMuonIP'      :     .02   # mm 
                   ,'BiasedSingleMuonTIP'     :     .05   # mm 
                   ,'BiasedLTime'             :     .1    # ps 
                   ,'BiasedLTimeT'            :     .10   # ps 
                   ,'BiasedSingleIPChi2'      :   10     
                   ,'BiasedSingleIPTChi2'     :   50     
                   ,'BiasedVertexChi2'        :    5
                   ,'BiasedPVDistanceChi2'    :   10
                   ,'BiasedPVDistanceTChi2'   :  150
                   
                   ,'DetachedDiMuonMinMass'   : 1000 #MeV
                   ,'DetachedDiMuonPt'        :    0  #MeV
                   ,'DetachedDiMuonMuPt'      :  500#Mev
                   ,'DetachedDiMuonVertexChi2':  25
                   ,'DetachedDiMuonDLS'       :   5

                   ,'DetachedHeavyDiMuonMinMass': 2950 # MeV
                   ,'DetachedHeavyDiMuonDLS'    : 3
                   ,'DetachedHavyDiMuonPt'      : 0
                   ,'DetachedDiMuIPChi2'      :   9
                   
                   ,'DetachedJPsiDLS'           : 3
                   ,'DetachedJPsiMassWindow'    : 120 #MeV

                   ,'NoPVPt'                    : 1500 # MeV
                   ,'NoPVMass'                  : 1000 # MeV
                   ,
                   # ==========================================================
                   ## Configuration parameters for ``multi-muon''-lines  
                   #                                       ``reasonable range''
                   #
                   # In case of automatic-tuning/bandwidth division,
                   # the following parameters are the most important:
                   # For "ThreeDetachedMuons"-line
                   #       - 'MultiMu_TightMuon_Chi2_IP'
                   #       - 'MultiMu_TightMuon_PT' 
                   # For "Tau2MuMuMu"-line
                   #       - 'MultiMu_Tau3Mu_MassWindow' 
                   #       - 'MultiMu_Tau3Mu_max_PT'
                   # For "DiDiMuon"-line
                   #       - 'MultiMu_DiMuon_Chi2_VX'
                   #       - 'MultiMu_Psi1S_MassWindow'
                   #       - 'MultiMu_Psi2S_MassWindow'
                   # For "MuAndDiMu"-line
                   #       - 'MultiMu_DiMuon_DLS'
                   #       - 'MultiMu_TightMuon_Chi2_IP'
                   #       - 'MultiMu_TightMuon_PT'
                   # For "DiMu&Gamma"-line
                   #       - 'DiMuonGamma_Gamma_PT'
                   # For "DiMu&Charm"-lines:
                   #       - 'DiMuonCharm_Pion_Chi2_IP'   
                   #       - 'DiMuonCharm_Kaon_Chi2_IP'  
                   #       - 'DiMuonCharm_Proton_Chi2_IP' 
                   #       - 'DiMuonCharm_Charm_PT'       
                   #       - 'DiMuonCharm_D0_MassWindow'  
                   #       - 'DiMuonCharm_D_MassWindow'   
                   #       - 'DiMuonCharm_Ds_MassWindow'  
                   #       - 'DiMuonCharm_Lc_MassWindow'  
                   # ==========================================================
                   ## chi2(IP) for ``good-muons''      ## [  4 ->  9 ]
                   'MultiMu_GoodMuon_Chi2_IP'   :  16          ,
                   ## PT for ``tight'' muons           ## [ 1.4 GeV -> 1.5 GeV ]
                   'MultiMu_TightMuon_PT'       :   1.4 *  GeV ,
                   ## chi2(IP) for ``tight-muons''     ## [ 25 -> 36 ]           
                   'MultiMu_TightMuon_Chi2_IP'  :  36          ,
                   ## chi2(VX) for ``dimuons''         ## [ 16 ->  9 ]
                   'MultiMu_DiMuon_Chi2_VX'     :  12          ,
                   ## PT for ``dimuons''         ## [ 0 GeV  ->  2 * GeV  ]
                   'MultiMu_DiMuon_PT'          :   1.2 * GeV  ,
                   ## Decay flight significance for detached dimuon ## [ 5 -> 9 ]
                   'MultiMu_DiMuon_DLS'         :   6         ,
                   ## (half)mass-window for J/psi      ## [ 120 MeV -> 100 MeV ]
                   'MultiMu_Psi1S_MassWindow'   : 110   * MeV ,
                   ## (half)mass-window for psi(2S)    ## [ 125 MeV -> 110 MeV ]
                   'MultiMu_Psi2S_MassWindow'   : 110   * MeV ,
                   ## (half)mass-window for tau->3mu   ## [ 300 MeV -> 200 MeV ]
                   'MultiMu_Tau3Mu_MassWindow'  : 300   * MeV ,
                   ## max(PT) for 1-muon from tau      ## [ 1 GeV   -> 1.4 GeV ]
                   'MultiMu_Tau3Mu_max_PT'      :   1   * GeV ,
                   'MultiMu_Tau3Mu_ctau'        : 45  ,  #mum
                   ## pt(Gamma) for dimuon+gamma line  ## [ 4 GeV   -> 8   GeV ]
                   'DiMuonGamma_Gamma_PT'       :   4   * GeV , 
                   ## chi2(IP) for good pions, kaons and protons ## [ 9 -> 16 ]
                   'DiMuonCharm_Pion_Chi2_IP'   :   9 ,
                   'DiMuonCharm_Kaon_Chi2_IP'   :   9 ,
                   'DiMuonCharm_Proton_Chi2_IP' :   9 ,
                   ## PT of charm particle                [ 1.5 GeV -> 3.0 GeV ] 
                   'DiMuonCharm_Charm_PT'       : 2.5 * GeV ,
                   ## (half) mass-windows for D0, D+, Ds+ , Lambda_c+ ## [ 75 MeV -> 55 MeV ]
                   'DiMuonCharm_D0_MassWindow'  :  65 * MeV ,
                   'DiMuonCharm_D_MassWindow'   :  65 * MeV ,
                   'DiMuonCharm_Ds_MassWindow'  :  65 * MeV ,
                   'DiMuonCharm_Lc_MassWindow'  :  65 * MeV ,
                   }

    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member, bindMembers
        from HltLine.Hlt2Monitoring import Hlt2Monitor, Hlt2MonitorMinMax
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.TrackFittedDiMuon import TrackFittedDiMuon
        from Configurables import FilterDesktop
        '''
          There are in total four unbiased selections:
          
          1) Heavy dimuon for sidebands etc, prescaled by 0.05
          2) mass window around J/psi
          3) mass window around Psi(2S)
          4) mass above 5 GeV
        '''

        from Configurables import CombineParticles
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
        #--------------------------------------------        
        '''
        unbiased same sign dimuon
        '''
        SameSignDiMu = Hlt2Member( CombineParticles
                                   , "SameSignDiMu"
                                   , DecayDescriptor = "[B0 -> mu+ mu+]cc"
                                   , DaughtersCuts = { "mu+" : "(PT>%(UnbiasedDiMuonMuPt)s*MeV)"%  self.getProps() }  
                                   
                                   , CombinationCut = "(AM>%(UnbiasedDiMuonMinMass)s*MeV)"%  self.getProps()  
                                   , MotherCut = "(PT>%(UnbiasedDiMuonPt)s*MeV)"\
                                   "& (VFASPF(VCHI2PDOF)<%(UnbiasedDiMuonVertexChi2)s )" %  self.getProps()
                                   , InputPrimaryVertices = "None"
                                   , UseP2PVRelations = False
                                   , Inputs  = [ BiKalmanFittedMuons ]
                                   )
        line = Hlt2Line('DiMuonSameSign'
                        , prescale = self.prescale 
                        , algos = [BiKalmanFittedMuons,SameSignDiMu]
                        , postscale = self.postscale
                        )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonSameSignDecision":   50211 } )
         #--------------------------------------------

        filter = Hlt2Member(   FilterDesktop 
                               , "Filter"
                               , Code = "(MM>%(UnbiasedDiMuonMinMass)s*MeV)"\
                               " & (PT>%(UnbiasedDiMuonPt)s*MeV)"\
                               " & (MINTREE('mu-'==ABSID,PT)>%(UnbiasedDiMuonMuPt)s*MeV) "\
                               "& (VFASPF(VCHI2PDOF)<%(UnbiasedDiMuonVertexChi2)s )" %  self.getProps() 
                               , InputPrimaryVertices = "None"
                               , UseP2PVRelations = False
                               , Inputs  = [ TrackFittedDiMuon ]
#                               , PreMonitor  =  Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_in',nbins=25) 
 #                              , PostMonitor =  Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_out',nbins=25)
                               )
        
        #--------------------------------------------
        '''
        unbiased heavy dimuon - prescaled
        '''
        line = Hlt2Line('DiMuon'
                        , prescale = self.prescale 
                        , algos = [ TrackFittedDiMuon, filter ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDecision" : 50200 } )


        #--------------------------------------------
        '''
        unbiased dimuon low mass - prescaled
        '''
        line.clone( 'DiMuonLowMass'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>%(UnbiasedDiMuonLowMinMass)s*MeV)"\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                 "& (PT>%(UnbiasedDiMuonLowPt)s*MeV) "\
                                 "& (MINTREE('mu-'==ABSID,PT)>%(UnbiasedDiMuonLowMuPt)s*MeV) "\
                                 "& (VFASPF(VCHI2PDOF)<%(UnbiasedDiMuonLowChi2)s )"%  self.getProps()
                                 }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonLowMassDecision":  50210 } )
        #--------------------------------------------
        '''
        unbiased J/psi
        '''
        line.clone( 'DiMuonJPsi'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS('J/psi(1S)')<%(UnbiasedJPsiMassWindow)s*MeV) "\
                                 "& (PT>%(UnbiasedJPsiPt)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                 "& (MINTREE('mu-'==ABSID,PT)>%(UnbiasedJPsiMuPt)s*MeV) "\
                                 "& (VFASPF(VCHI2PDOF)<%(UnbiasedJPsiVertexChi2)s )"%  self.getProps()
                                 , 'PreMonitor' : Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_in',nbins=25) 
                                 , 'PostMonitor' :  
                                 Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_out',nbins=25)
                                 +" & "+Hlt2MonitorMinMax( "PT","PT(#mu#mu)",0,10000,'JPsiPT_out',nbins=100)
                                 +" & "+Hlt2MonitorMinMax( "MINTREE('mu-'==ABSID,PT)","MINTREE(mu-==ABSID,PT)",0,10000,'MuPT_out',nbins=100)
                                 +" & "+Hlt2MonitorMinMax( "VFASPF(VCHI2PDOF)","VFASPF(VCHI2PDOF)",0,25,'JPsiVeterxChi2_out',nbins=100)                           
            
                                 }
                    , postscale = self.postscale
                  
            
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonJPsiDecision":  50201 } )
        


        #--------------------------------------------
        '''
        J/psi high PT
        '''
        line.clone( 'DiMuonJPsiHighPT'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS('J/psi(1S)')<%(UnbiasedJPsiHighPTMassWindow)s*MeV) "\
                                     "& (PT>%(UnbiasedJPsiHighPt)s*MeV) "\
                                     "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                     "& (MINTREE('mu-'==ABSID,PT)>%(UnbiasedJPsiMuPt)s*MeV) "\
                                     "& (VFASPF(VCHI2PDOF)<%(UnbiasedJPsiVertexChi2)s )"%  self.getProps()
                                 , 'PreMonitor' : Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_in',nbins=25) 
                                 , 'PostMonitor' :  
                                 Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_out',nbins=25)
                                 +" & "+Hlt2MonitorMinMax( "PT","PT(#mu#mu)",0,10000,'JPsiPT_out',nbins=100)
                                 +" & "+Hlt2MonitorMinMax( "MINTREE('mu-'==ABSID,PT)","MINTREE(mu-==ABSID,PT)",0,10000,'MuPT_out',nbins=100)
                                 +" & "+Hlt2MonitorMinMax( "VFASPF(VCHI2PDOF)","VFASPF(VCHI2PDOF)",0,25,'JPsiVeterxChi2_out',nbins=100)                           
                                 
                                 }
                    , postscale = self.postscale
                    
                    
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonJPsiHighPTDecision":  50213 } )
        #--------------------------------------------
        '''
        unbiased Psi(2S)
        '''
        line.clone( 'DiMuonPsi2S'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS(3686.09*MeV)<%(UnbiasedPsi2SMassWindow)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                 "& (PT>%(UnbiasedPsi2SPt)s*MeV) "\
                                 "& (MINTREE('mu-'==ABSID,PT)>%(UnbiasedPsi2SMuPt)s*MeV) "\
                                 "& (VFASPF(VCHI2PDOF)<%(UnbiasedPsi2SVertexChi2)s )" %  self.getProps() 
                                 , 'PostMonitor' :
                                    Hlt2Monitor( "M","M(#mu#mu)",3686,200,'M_out',nbins=25)
                                 }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonPsi2SDecision": 50202 } )
        
        #--------------------------------------------
        ''' 
        unbiased Psi(2S) high PT
        '''
        line.clone( 'DiMuonPsi2SHighPT'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS(3686.09*MeV)<%(UnbiasedPsi2SMassWindow)s*MeV) "\
                                     "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                     "& (PT>%(UnbiasedPsi2SPtHigh)s*MeV) "\
                                     "& (MINTREE('mu-'==ABSID,PT)>%(UnbiasedPsi2SMuPt)s*MeV) "\
                                     "& (VFASPF(VCHI2PDOF)<%(UnbiasedPsi2SVertexChi2)s )" %  self.getProps() 
                                 , 'PostMonitor' :
                                     Hlt2Monitor( "M","M(#mu#mu)",3686,200,'M_out',nbins=25)
                                 }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonPsi2SHighPTDecision": 50223 } )
        
        #--------------------------------------------
        '''
        unbiased Bmm
        '''
        line.clone( 'DiMuonB'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>%(UnbiasedBmmMinMass)s*MeV) "\
                                     "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                     "& (VFASPF(VCHI2PDOF)<%(UnbiasedBmmVertexChi2)s )"  %  self.getProps() 
                                 , 'PostMonitor' :
                                   Hlt2Monitor( "M","M(#mu#mu)",5300,700,'M_out',nbins=25)
                                 }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonBDecision":   50203 } )
        
        #--------------------------------------------
      ##   '''
##         prescaled unbiased J/psi for low rate scenario
##         '''
##         line.clone( 'DiMuonJPsiLow'
##                     , prescale = self.prescale
##                     , Filter = { 'Code': "(ADMASS('J/psi(1S)')<%(UnbiasedJPsiMassWindow)s*MeV) "\
##                                  "& (PT>%(UnbiasedJPsiPt)s*MeV) "\
##                                  "& (MINTREE('mu-'==ABSID,PT)>%(UnbiasedJPsiMuPt)s*MeV) "\
##                                  "& (VFASPF(VCHI2PDOF)<%(UnbiasedJPsiVertexChi2)s )"% self.getProps() }
##                     , postscale = self.postscale
##                     )
##         HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonUnbiasedJPsiLowDecision":  50204 } )
        
        #--------------------------------------------
        def addPT( code, props, ptkey  ) :
            if props[ptkey] > 0 :   code = code + "& (PT>%s*MeV)"% props[ptkey]
            return code % props

        '''
        unbiased Zmm
        '''
        line.clone( 'DiMuonZ'
                    , prescale = self.prescale 
                    , Filter = { 'Code':  addPT("(MM>%(UnbiasedZmmMinMass)s*MeV)",self.getProps(),"UnbiasedZmmPt")
                               , 'PostMonitor' : Hlt2Monitor( "M","M(#mu#mu)",90000,10000,'M_out',nbins=25)
                               }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonZDecision":   50205 } )
        #--------------------------------------------
        '''
        unbiased Drell-Yan 1
        '''
        line.clone( 'DiMuonDY1'
                    , prescale = self.prescale 
                    , Filter = { 'Code': addPT( " ((MM>%(DY1MinMass)s*MeV) & ~in_range(%(DY1MinExcMass)s*MeV, MM,%(DY1MaxExcMass)s*MeV)) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                                 "& (MINTREE('mu-'==ABSID,PT)>%(DY1MuPt)s*MeV)" , self.getProps(), "DYPt" ) }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY1Decision":   50206 } )

        #--------------------------------------------
        '''
        unbiased Drell-Yan 2
        '''
        line.clone( 'DiMuonDY2'
                    , prescale = self.prescale 
                    , Filter = { 'Code': addPT( "  (MM>%(DY2MinMass)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                                 "& (MINTREE('mu-'==ABSID,PT)>%(DY2MuPt)s*MeV)",  self.getProps(), "DYPt" ) }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY2Decision":   50207 } )

        #----------------------------------------------------------------------------------------

        '''
        unbiased Drell-Yan 3
        '''
        line.clone( 'DiMuonDY3'
                    , prescale = self.prescale 
                    , Filter = { 'Code': addPT("  (MM>%(DY3MinMass)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )", self.getProps(), "DYPt") }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY3Decision":   50208 } )

        #--------------------------------------------
        '''
        unbiased Drell-Yan 4
        '''
        line.clone( 'DiMuonDY4'
                    , prescale = self.prescale 
                    , Filter = { 'Code': addPT("  (MM>%(DY4MinMass)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )", self.getProps(), "DYPt" ) }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY4Decision":   50209 } )

        #----------------------------------------------------------------------------------------
        
        '''
        Biased DiMuon Lines
               Leandro de Paula - leandro.de.paula@cern.ch
 
        There are in total 4 biased selections:

        Two selections try to reduce the rate without significan drop in signal efficiency.
        At nominal conditions they should be prescaled:

        5) Simple Biased DiMuon: first step of DiMuon selection
        6) Refined Biased DiMuon: use all variables

        and

        7) High Mass Biased DiMuon selection 
        8) Biased DiMuon selection with high IP cuts
        '''
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonSimpleDecision"  : 50040 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonRefinedDecision" : 50041 } )

        '''
           cut definitions for biased DiMuon Lines
        '''
        MuPtCut = "( MAXTREE(ABSID=='mu+',PT)>%(BiasedSingleMuonPt)s*MeV)" % self.getProps()
        MassCut  = "(MM>%(BiasedMass)s*MeV)" % self.getProps()
        MassLCut = "(MM>%(BiasedLMass)s*MeV)" % self.getProps()
        MassTCut = "(MM>%(BiasedTMass)s*MeV)" % self.getProps()
        MuIPCut  = "(2==NINTREE((ABSID=='mu+') & (MIPDV(PRIMARY)>%(BiasedSingleMuonIP)s*mm)))" % self.getProps()
        MuTIPCut = "(2==NINTREE((ABSID=='mu+') & (MIPDV(PRIMARY)>%(BiasedSingleMuonTIP)s*mm)))" % self.getProps()
        LTimeCut  = "(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>%(BiasedLTime)s*ps)" % self.getProps()
        LTimeTCut = "(BPVLTIME('PropertimeFitter/properTime:PUBLIC')>%(BiasedLTimeT)s*ps)" % self.getProps()
        IPChi2Cut  = "( MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))>%(BiasedSingleIPChi2)s)" % self.getProps()
        TIPChi2Cut = "( MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))>%(BiasedSingleIPTChi2)s)" % self.getProps()
        VertexChi2Cut = "(VFASPF(VCHI2PDOF)<%(BiasedVertexChi2)s)" % self.getProps()
        PVDistChi2Cut  = "(BPVVDCHI2>%(BiasedPVDistanceChi2)s)" % self.getProps()
        PVDistTChi2Cut = "(BPVVDCHI2>%(BiasedPVDistanceTChi2)s)" % self.getProps()
        '''
           sequence definitions for biased DiMuon Lines
        '''
        SimpleDiMuon = Hlt2Member( FilterDesktop
                                   , "SimpleDiMuon"          
                                   , Inputs = [ TrackFittedDiMuon ]
                                   , Code = MuPtCut +"&"+ MassCut +"&"+ MuIPCut +"&"+ LTimeCut
                                   , PreMonitor = 
                                   Hlt2MonitorMinMax( "M","M(#mu#mu)",0,6000,'M_in',nbins=25) 
                                   +" & "+Hlt2MonitorMinMax( "MINTREE('mu-'==ABSID,PT)","Pt(#mu)Min",0,10000,'MuPT_in',nbins=100)
                                   +" & "+Hlt2MonitorMinMax( "PT","PT(#mu#mu)",0,10000,'JPsiPT_in',nbins=100)
                                   +" & "+Hlt2MonitorMinMax( "MIPDV(PRIMARY)","IP(#mu)",0,.5,'MuIP_in',nbins=100)
                                   +" & "+Hlt2MonitorMinMax( "BPVLTIME('PropertimeFitter/properTime:PUBLIC')","Lifetime",0,.002,'LifeTime_in',nbins=40)
                                   , PostMonitor =  
                                   Hlt2MonitorMinMax( "M","M(#mu#mu)",0,6000,'M_out',nbins=25) 
                                   +" & "+Hlt2MonitorMinMax( "MINTREE('mu-'==ABSID,PT)","Pt(#mu)Min",0,10000,'MuPT_out',nbins=100)
                                   +" & "+Hlt2MonitorMinMax( "PT","PT(#mu#mu)",0,10000,'JPsiPT_out',nbins=100)
                                   +" & "+Hlt2MonitorMinMax( "MIPDV(PRIMARY)","IP(#mu)",0,.5,'MuIP_out',nbins=100)
                                   +" & "+Hlt2MonitorMinMax( "BPVLTIME('PropertimeFitter/properTime:PUBLIC')","Lifetime",0,.002,'LifeTime_out',nbins=40)
                                   )
        RefinedDiMuon = Hlt2Member( FilterDesktop
                                    , "RefinedDiMuon"          
                                    , Inputs = [ SimpleDiMuon ]
                                    , Code = IPChi2Cut +"&"+ VertexChi2Cut +"&"+ PVDistChi2Cut
                                    , PreMonitor = 
                                    Hlt2MonitorMinMax( "MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))","#chi^{2}(IP#mu)",0,200,'IP_Chi2_in',nbins=100)
                                    +" & "+Hlt2MonitorMinMax( "VFASPF(VCHI2PDOF)","#chi^{2}/DoF(Vertex)",0,25,'Veter_Chi2_in',nbins=100)
                                    +" & "+Hlt2MonitorMinMax( "BPVVDCHI2","#chi^{2}(Dist)",0,500,'Flight_DistanceChi2_in',nbins=100)
                                    , PostMonitor =  
                                    Hlt2MonitorMinMax( "M","M(#mu#mu)",0,6000,'M_out',nbins=25) 
                                    +" & "+Hlt2MonitorMinMax( "MAXTREE(ABSID=='mu+',MIPCHI2DV(PRIMARY))","#chi^{2}(#muIP)",0,200,'IP_Chi2_out',nbins=100)
                                    +" & "+Hlt2MonitorMinMax( "VFASPF(VCHI2PDOF)","#chi^{2}/DoF(Vertex)",0,25,'Veter_Chi2_out',nbins=100)
                                    +" & "+Hlt2MonitorMinMax( "BPVVDCHI2","#chi^{2}(Dist)",0,500,'Flight_DistanceChi2_out',nbins=100)

                                  )
        #--------------------------------------------
        '''
            simple biased DiMuon 
        '''
        line = Hlt2Line('BiasedDiMuonSimple'
                        , prescale = self.prescale 
                        , algos = [ PV3D(), TrackFittedDiMuon, SimpleDiMuon ]
                        , postscale = self.postscale
                        )

        #--------------------------------------------
        '''
            refined biased DiMuon
        '''
        line = Hlt2Line('BiasedDiMuonRefined'
                        , prescale = self.prescale 
                        , algos = [ PV3D(), TrackFittedDiMuon, SimpleDiMuon, RefinedDiMuon ]
                        , postscale = self.postscale
                        )

        #--------------------------------------------
        '''
            high mass biased DiMuon
        '''
        line.clone( 'BiasedDiMuonMass'
                    , prescale = self.prescale 
                    , SimpleDiMuon = {"Code" : MuPtCut +"&"+ MassTCut +"&"+ MuIPCut +"&"+ LTimeTCut
                                      }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonMassDecision"    : 50042 } )
        
        
        #--------------------------------------------
        '''
            biased DiMuon with tigh IP cuts
        '''
        line.clone( 'BiasedDiMuonIP'
                    , prescale = self.prescale
                    , SimpleDiMuon = {"Code" : MuPtCut +"&"+ MassLCut +"&"+ MuTIPCut +"&"+ LTimeCut }
                    , RefinedDiMuon = {"Code" : TIPChi2Cut +"&"+ VertexChi2Cut +"&"+ PVDistTChi2Cut }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BiasedDiMuonIPDecision"      : 50043 } )
        
        #----------------------------------------------------------------------------------------
        '''
            detached dimuon lines

        '''

        filter = Hlt2Member(   FilterDesktop 
                               , "Filter"
                               , Code = "(MM>%(DetachedDiMuonMinMass)s*MeV)"\
                                   "& (MINTREE('mu-'==ABSID,MIPCHI2DV(PRIMARY)) > %(DetachedDiMuIPChi2)s )"\
                                   "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                   " & (PT>%(DetachedDiMuonPt)s*MeV)"\
                                   " & (MINTREE('mu-'==ABSID,PT)>%(DetachedDiMuonMuPt)s*MeV) "\
                                   "& (VFASPF(VCHI2PDOF)<%(DetachedDiMuonVertexChi2)s )"\
                                   "& (BPVDLS>%(DetachedDiMuonDLS)s )"%  self.getProps() 
                               , Inputs  = [ TrackFittedDiMuon ]
                               )
        
        #--------------------------------------------
        '''
        detached low mass detached dimuon 
        '''
        line = Hlt2Line('DiMuonDetached'
                        , prescale = self.prescale 
                        , algos = [ PV3D(), TrackFittedDiMuon, filter ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDetachedDecision" : 50045 } )


      
        #--------------------------------------------
        '''
        detached heavy dimuon
        '''
        DiMuonBiasedMass = line.clone( 'DiMuonDetachedHeavy'
                                       , prescale = self.prescale
                                       , Filter = { 'Code' : "(MM>%(DetachedHeavyDiMuonMinMass)s*MeV)"\
                                                    "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                                    " & (PT>%(DetachedHavyDiMuonPt)s*MeV)"\
                                                    " & (MINTREE('mu-'==ABSID,PT)>%(DetachedDiMuonMuPt)s*MeV) "\
                                                    "& (VFASPF(VCHI2PDOF)<%(DetachedDiMuonVertexChi2)s )"\
                                                    "& (BPVDLS>%(DetachedHeavyDiMuonDLS)s )"%  self.getProps() 
                                                    }
                    , postscale = self.postscale
                    )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDetachedHeavyDecision" : 50046 } )

        '''
        Inclusive Biased Jpsi
        Roel Aaij, roel.aaij@cern.ch
        '''
        from Hlt2SharedParticles.TrackFittedDiMuon import DetachedTrackFittedJpsi2MuMu
        Hlt2SelDetachedJPsi = Hlt2Member( FilterDesktop
                                          , "Filter"
                                          , Inputs  = [DetachedTrackFittedJpsi2MuMu]
                                          , Code =  "(MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                              "& (ADMASS('J/psi(1S)')<%(DetachedJPsiMassWindow)s*MeV) "\
                                              "& (BPVDLS>%(DetachedJPsiDLS)s )"%  self.getProps() 
                                          )
        
        DiMuonBiasedJpsi = Hlt2Line ( 'DiMuonDetachedJPsi'
                                      , prescale = self.prescale 
                                      , algos = [ PV3D(), DetachedTrackFittedJpsi2MuMu, Hlt2SelDetachedJPsi ]
                                      , postscale = self.postscale
                                      )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDetachedJPsiDecision" : 50044 } )

        """
        Inclusive Detached Psi(2S)
        With the same cuts as Detached J/psi

        """
        Hlt2SelDetachedPsi2S = Hlt2Member( FilterDesktop
                                           , "FilterDetachedPsi2S"
                                           , Inputs  = [ TrackFittedDiMuon ]
                                           , Code =  "(MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                           " & (ADMASS('psi(2S)')<%(DetachedJPsiMassWindow)s*MeV) "\
                                           " & (BPVDLS>%(DetachedJPsiDLS)s )"%  self.getProps() 
                                           )
        
        DiMuonDetachedPsi2S = Hlt2Line ( 'DiMuonDetachedPsi2S'
                                         , prescale = self.prescale 
                                         , algos = [ PV3D(), TrackFittedDiMuon, Hlt2SelDetachedPsi2S ]
                                         , postscale = self.postscale
                                         )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDetachedPsi2SDecision" : 50144 } )        
        

        #--------------------------------------------
        '''                           
        line for dimuon without PV reconstruction  

           Leandro de Paula - leandro.de.paula@cern.ch

        '''
        NoPVMassCut = "(AM>%(NoPVMass)s*MeV)" % self.getProps()
        NoPVPTCut   = "(APT<%(NoPVPt)s*MeV)" % self.getProps()

        Hlt1Muons = "HLT_PASS_RE('Hlt1.*Muon.*Decision')"
        Hlt1NoPVMuons = "HLT_PASS_RE('Hlt1NoPVPassThroughDecision')"
        DiMuNoPV = Hlt2Member(CombineParticles
                               , "DiMuonNoPV"
                               , DecayDescriptors = ["J/psi(1S) -> mu+ mu-"]
                               , DaughtersCuts = { "mu+" : "(PT>400*MeV) ",
                                                   "mu-" : "(PT>400*MeV) "}
                               , CombinationCut = NoPVMassCut+"&"+NoPVPTCut
                               , MotherCut = "ALL"
                               , MotherMonitor = Hlt2MonitorMinMax ("M","M(#mu#mu)",0,7000)
                               +" & "+Hlt2MonitorMinMax( "PT","PT(#mu#mu)",0,2000)
                               , InputPrimaryVertices = "None"
                               , UseP2PVRelations = False
                               , Inputs  = [ BiKalmanFittedMuons ]
                              )

        line = Hlt2Line('DiMuonNoPV'
                        , prescale = self.prescale
                        , HLT = Hlt1NoPVMuons
                        , algos = [BiKalmanFittedMuons, DiMuNoPV]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonNoPVDecision" : 50047 } )

        #--------------------------------------------

        # =====================================================================
        # Add some ``multi-muon''-lines
        # =====================================================================
        self.__multiMuonLines  ()

        
    ## define set of ``multi-muon''-lines 
    def __multiMuonLines  ( self ) :
        """
        Define set of ``multi-muon''-lines

        Four lines are defined:

        - at least three muons iwth high pt ad high chi2(ip)
        - at least two dimuons 
        - dimuon and muon iwth high-pt high chi2(ip) or
                              muon and detached dimuon
        - tau -> 3mu 
        
        The parameters:
        
        - MultiMu_GoodMuon_chi2_IP   : default    9 
        - MultiMu_TightMuon_PT       : default    1.25 * GeV 
        - MultiMu_TightMuon_chi2_IP  : default   25    
        - MultiMu_DiMuon_Chi2_VX     : default   12
        - MultiMu_DiMuon_DLS         : default    6 
        - MultiMu_Psi1S_MassWindow   : default  110 * MeV  
        - MultiMu_Psi2S_MassWindow   : default  120 * MeV 
        - MultiMu_Tau3Mu_MassWindow  : default  300 * MeV 
        - MultiMu_Tau3Mu_max_PT      : default    1 * GeV 

        In addition, ``dimuon+X''-lines:

        - dimuon + high-pt gamma
        - set of dimuon + prompt charm

        The parameters:
        
        - DiMuonGamma_Gamma_PT        : default  4   * GeV 
        - DiMuonCharm_Pion_Chi2_IP    : default  9 
        - DiMuonCharm_Kaon_Chi2_IP    : default  9 
        - DiMuonCharm_Proton_Chi2_IP  : default  9 
        - DiMuonCharm_Charm_PT        : default  2.5 * GeV 
        - DiMuonCharm_D0_MassWindow   : default 65   * MeV 
        - DiMuonCharm_D_MassWindow    : default 65   * MeV 
        - DiMuonCharm_Ds_MassWindow   : default 65   * MeV 
        - DiMuonCharm_Lc_MassWindow   : default 65   * MeV 

        """
        
        #
        ## pure techinical stuff: import all nesessary components
        # 
        from Configurables      import HltANNSvc , CombineParticles, FilterDesktop
        from Configurables      import LoKi__VoidFilter as Counter
        
        from HltLine.HltLine    import Hlt2Member , Hlt2Line , bindMembers 
        from HltTracking.HltPVs import PV3D

        #
        ## get main symbols: Muons & DiMuons 
        # 
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons   as Muons
        from Hlt2SharedParticles.TrackFittedDiMuon         import TrackFittedDiMuon     as DiMuons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPhotons as Photons 
        
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions   as Pions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons   as Kaons
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedProtons as Protons
        
        Preambulo0 = [ "from LoKiCore.functions           import *" 
                     , "from LoKiPhys.functions           import *"
                     , "from GaudiKernel.PhysicalConstants import c_light"
                     , "from LoKiArrayFunctors.functions import *"
                     , "from GaudiKernel.SystemOfUnits import GeV,MeV"
                     ]
        
        Preambulo  = Preambulo0 + [
            "goodTrack = (TRCHI2DOF < %(TrChi2Tight)g) " , 
            "goodMuon  = goodTrack                                     & ( BPVIPCHI2() > %(MultiMu_GoodMuon_Chi2_IP)g  ) " ,
            "tightMuon = goodTrack & ( PT > %(MultiMu_TightMuon_PT)g ) & ( BPVIPCHI2() > %(MultiMu_TightMuon_Chi2_IP)g ) " ,            
            # related to tau->3mu 
            "ctau      = BPVLTIME ( ) * c_light   " ,
            "chi2vx    = VFASPF(VCHI2) "            , 
            "min_m12   = 2* PDGM('mu+') + 3 * MeV " ,
            # related to dimuons 
            "psi             = ADMASS ( 'J/psi(1S)' ) < %(MultiMu_Psi1S_MassWindow)g "           ,
            "psi_prime       = ADMASS (   'psi(2S)' ) < %(MultiMu_Psi2S_MassWindow)g "           ,
            "dimuon_mass     = psi | psi_prime | ( M > 5 * GeV ) "                               ,
            "dimuon          = dimuon_mass &  ( PT > %(MultiMu_DiMuon_PT)g ) &( VFASPF ( VCHI2 ) < %(MultiMu_DiMuon_Chi2_VX)g ) " ,            
            "detached_dimuon = dimuon & ( BPVDLS > %(MultiMu_DiMuon_DLS)g ) "                    ,
            # related to dimuon+gamma
            "good_photons    = ( 'gamma' == ID ) & ( PT > %(DiMuonGamma_Gamma_PT)g ) "           ,
            # related to dimuon+charm
            "good_pions   = goodTrack & ( BPVIPCHI2() > %(DiMuonCharm_Pion_Chi2_IP)g   ) " ,
            "good_kaons   = goodTrack & ( BPVIPCHI2() > %(DiMuonCharm_Kaon_Chi2_IP)g   ) " ,
            "good_protons = goodTrack & ( BPVIPCHI2() > %(DiMuonCharm_Proton_Chi2_IP)g ) " ,
            "ctau_charm   = BPVLTIME ( 16 ) * c_light   "                                  ,
            ]
        
        Preambulo = [ p % self.getProps() for p in Preambulo ]
        
        Preambulo2 = [
            "from LoKiPhys.decorators          import *" ,
            "from LoKiArrayFunctors.decorators import *" ] + Preambulo 
        
        ## save some typing for the most trivial filters 
        def _filter_  ( Inputs  , Code = " ALL " , name = 'All' ) :
            " save some typing for the most trivial filters"
            if not isinstance ( Inputs , list ) : Inputs = [ Inputs ] 
            return [ Hlt2Member ( FilterDesktop         ,
                                  name                  ,
                                  Preambulo = Preambulo , 
                                  Inputs    = Inputs    ,
                                  Code      = Code      ) ] 
        
        # =====================================================================
        ## palette of "technical" algorithms/filters  
        # =====================================================================
        #
        ## check the presense of at least three muons in event
        Check_3mu = Counter (
            "Hlt2MultiMu:CheckFor3Mu"  ,
            Preambulo = Preambulo0 , 
            Code      = " 2.9 < CONTAINS ( '%s' ) " % Muons.outputSelection()
            )
        #
        ## check the presense of at least four muons in event
        Check_4mu = Counter (
            "Hlt2MultiMu:CheckFor4Mu"  ,
            Preambulo = Preambulo0 , 
            Code      = " 3.9 < CONTAINS ( '%s' ) " % Muons.outputSelection()
            )
        
        #
        ## select 'good' muons 
        GoodMuons = bindMembers (
            'MultiMu:'  , 
            Muons.members()      + _filter_ ( Inputs =    Muons  , Code =  'goodMuon' , name =  'GoodMuons' )
            )
        #
        ## select 'tight' muons 
        TightMuons = bindMembers  (
            'MultiMu:' ,
            GoodMuons.members () + _filter_ ( Inputs = GoodMuons , Code = 'tightMuon' , name = 'TightMuons' )
            )
        #
        ## check the presense of at least three "good" muons in event
        Check_3GoodMu  = Counter (
            "Hlt2MultiMu:ThreeGoodMu",
            Preambulo  = Preambulo0 ,
            Code       = " 2.9 < CONTAINS ( '%s' ) " % GoodMuons.outputSelection () 
            )
        #
        ## check the presense of at least three "tight" muons in event
        Check_3TightMu = Counter(
            "Hlt2MultiMu:ThreeTightMu",
            Preambulo  = Preambulo2 ,
            Code       = " 2.9 < CONTAINS ( '%s' )  " % TightMuons.outputSelection () 
            )
        #
        ## select 'our' dimuons 
        DiMuons  = bindMembers (
            'MultiMu:' ,
            DiMuons.members()    + _filter_ ( Inputs =   DiMuons , Code =    'dimuon' , name =    'DiMuons' ) 
            )
        
        ## select 'good' photons 
        GoodPhotons = bindMembers (
            'DiMuonGamma:'  , 
            Photons.members()    + _filter_ ( Inputs =   Photons  , Code =  'good_photons' , name =  'GoodPhotons' )
            )

        ## select 'good' pions 
        GoodPions = bindMembers (
            'DiMuonCharm:'  , 
            Pions.members()    + _filter_ ( Inputs =   Pions  , Code =  'good_pions' , name =  'GoodPions' )
            )
        ## select 'good' kaons 
        GoodKaons = bindMembers (
            'DiMuonCharm:'  , 
            Kaons.members()    + _filter_ ( Inputs =   Kaons  , Code =  'good_kaons' , name =  'GoodKaons' )
            )
        
        ## select 'good' protons 
        GoodProtons = bindMembers (
            'DiMuonCharm:'  , 
            Protons.members()  + _filter_ ( Inputs =  Protons , Code =  'good_protons' , name =  'GoodProtons' )
            )
        
        #
        ## check the presense of at least two dimuons 
        check_2dimu    = Counter (
            "Hlt2MultiMu:CheckFor2DiMu" ,
            Preambulo  = Preambulo  ,
            Code       = " 1.9 < NUMBER ( '%s' , dimuon ) " % DiMuons.outputSelection()
            )
        #
        ## Maker of tau+ -> mu+ mu+ mu- candidates 
        tau_3mu = Hlt2Member (
            CombineParticles , 
            'Combine'        , 
            Inputs          = [ GoodMuons ]               , 
            Preambulo       = Preambulo                   ,
            DecayDescriptor = '[ tau+ -> mu+ mu+ mu- ]cc' , 
            DaughtersCuts   = { "mu+" : 'goodMuon' }      , 
            CombinationCut  = """
            ( ADAMASS('tau+') < %(MultiMu_Tau3Mu_MassWindow)g ) &
            ( AM12            > min_m12   ) & 
            AHASCHILD ( PT    > %(MultiMu_Tau3Mu_max_PT)g     ) 
            """ % self.getProps() ,
            MotherCut       = " ( chi2vx < 25 ) & ( ctau > %(MultiMu_Tau3Mu_ctau)g * um) "% self.getProps() 
            )
        #
        ## maker of dimu plus mu cominations 
        mu_and_dimu     = Hlt2Member (
            CombineParticles ,
            'Combine'        ,
            Inputs          = [ DiMuons , Muons ]         , 
            Preambulo       = Preambulo                   ,
            DecayDescriptor = "[B_c+ -> J/psi(1S) mu+]cc" , 
            DaughtersCuts   = {
            'J/psi(1S)' : 'dimuon    & ( chi2vx <   10       ) ' ,
            'mu+'       : 'goodTrack'
            } ,
            CombinationCut  = """
            ( ACHILDCUT ( 1 , detached_dimuon ) & ACHILDCUT ( 2 , tightMuon ) )
            """,
            MotherCut       = " ALL  " 
            )
        
        ## maker of dimu plus photon cominations 
        mu_and_gamma        = Hlt2Member (
            CombineParticles ,
            'Combine'        ,
            Inputs            = [ DiMuons , GoodPhotons ]        , 
            Preambulo         = Preambulo                        ,
            DecayDescriptor   = "chi_b2(1P) -> J/psi(1S) gamma " , 
            DaughtersCuts     = {
            'J/psi(1S)' : 'dimuon  & ( chi2vx < 10 ) ' ,
            'gamma'     : 'good_photons'                
            } ,
            ## ??
            # ParticleCombiners = { '' : 'MomentumCombiner' } ,
            CombinationCut    = " AALL " ,
            MotherCut         = "  ALL " 
            )
        
        ## Maker of D0 -> K- pi+ candidates 
        d0_2kpi = Hlt2Member (
            CombineParticles , 
            'CharmCombine'   , 
            Inputs          = [ GoodKaons, GoodPions ]    , 
            Preambulo       = Preambulo                   ,
            DecayDescriptor = '[D0 -> K- pi+]cc' , 
            CombinationCut  = """
            ( APT           > %(DiMuonCharm_Charm_PT)g      ) &             
            ( ADAMASS('D0') < %(DiMuonCharm_D0_MassWindow)g ) 
            """ % self.getProps() ,
            MotherCut       = " ( chi2vx < 30 ) & ( ctau_charm > 100 * um ) "
            )
        
        ## Maker of D+ -> K- pi+ pi+ candidates 
        dp_2kpipi = Hlt2Member (
            CombineParticles , 
            'CharmCombine'   , 
            Inputs          = [ GoodKaons, GoodPions ]    , 
            Preambulo       = Preambulo                   ,
            DecayDescriptor = '[D+ -> K- pi+ pi+]cc' , 
            CombinationCut  = """
            ( APT           > %(DiMuonCharm_Charm_PT)g      ) &             
            ( ADAMASS('D+') < %(DiMuonCharm_D_MassWindow)g ) 
            """ % self.getProps() ,
            MotherCut       = " ( chi2vx < 30 ) & ( ctau_charm > 100 * um ) "
            )
        #
        
        ## Maker of Ds+ -> K- K+ pi+ pi+candidates 
        ds_2kkpi = Hlt2Member (
            CombineParticles , 
            'CharmCombine'   , 
            Inputs          = [ GoodKaons , GoodPions ]   , 
            Preambulo       = Preambulo                   ,
            DecayDescriptor = '[D_s+ -> K- K+ pi+]cc' , 
            CombinationCut  = """
            ( APT           > %(DiMuonCharm_Charm_PT)g      ) &
            ( AM12          < 1040 * MeV  )                   & 
            in_range ( PDGM('D+') - %(DiMuonCharm_D_MassWindow)g , AM , PDGM('D_s+') - %(DiMuonCharm_D_MassWindow)g )
            """ % self.getProps() ,
            MotherCut       = " ( chi2vx < 30 ) & ( ctau_charm > 100 * um ) "
            )
        #
        
        ## Maker of Lambda_c+ -> p K- pi+ pi+candidates 
        lc_2pkpi = Hlt2Member (
            CombineParticles , 
            'CharmCombine'   , 
            Inputs          = [ GoodKaons, GoodProtons , GoodPions ]    , 
            Preambulo       = Preambulo                   ,
            DecayDescriptor = '[Lambda_c+ -> p+ K- pi+]cc' , 
            CombinationCut  = """
            ( APT           > %(DiMuonCharm_Charm_PT)g      ) &
            ( ADAMASS('D+') < %(DiMuonCharm_Lc_MassWindow)g ) 
            """ % self.getProps() ,
            MotherCut       = " ( chi2vx < 30 ) & ( ctau_charm > 100 * um ) "
            )
        
        ## Maker of double dimuons 
        di_2mu = Hlt2Member (
            CombineParticles  , 
            'DiDiMuonCombine' , 
            Inputs          = [ DiMuons ]                            , 
            Preambulo       = Preambulo                              ,
            DecayDescriptor = ' chi_b0(2P) -> J/psi(1S) J/psi(1S)'   , 
            CombinationCut  = " AALL "                               , 
            MotherCut       = "  ALL "
            )
        
        ## Maker of trimuons 
        tri_muons = Hlt2Member (
            CombineParticles  , 
            'TriMuonCombine'  , 
            Inputs          = [ TightMuons ]                         , 
            Preambulo       = Preambulo                              ,
            DecayDescriptor = '[B_c+ -> mu+ mu+ mu-]cc'              , 
            CombinationCut  = " AALL "                               , 
            MotherCut       = "  ALL "
            )
        
        ## make dimuon + charm combinations
        def dimu_charm_ ( charm , decay ) :
            return Hlt2Member (
                CombineParticles ,
                'DiMuCharm'      ,
                Inputs            = [ DiMuons , charm ] , 
                DecayDescriptor   = decay               ,
                CombinationCut    = " AALL "            ,
                MotherCut         = "  ALL " 
                )
        
        # =====================================================================
        # three generic detached high-pt muons 
        # =====================================================================
        three_mu = Hlt2Line (
            "TriMuonDetached" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [   
            PV3D    ()     ,    ## recontruct PV
            Muons          ,    ## get muons  
            Check_3mu      ,    ## require at least 3 muons 
            GoodMuons      ,    ## select good muons
            Check_3GoodMu  ,    ## require at least 3 good muons            
            TightMuons     ,    ## select tight muons 
            Check_3TightMu ,    ## require at least 3 tight muons
            tri_muons      ]
            ## + _filter_ ( Inputs = TightMuons ) ## select/copy tight muons (again)
            )
        
        # =====================================================================
        # generic di-dimuon line
        # =====================================================================
        di_dimuon = Hlt2Line (
            "DoubleDiMuon" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [
            Muons          ,  ## get muons 
            Check_4mu      ,  ## require at least 4 muons
            DiMuons        ,  ## get dimuons 
            check_2dimu    ,  ## require at least two 'true' dimuons
            di_2mu
            ]
            ## + _filter_ ( Inputs = DiMuons ) ## select/copy dimuons (again)
            )
        
        # =====================================================================
        # dimuon + muon line:
        #                dimuon + tight muon
        #       detached dimuon + muon
        # =====================================================================        
        dimuon_and_muon = Hlt2Line (
            "DiMuonAndMuon" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [
            PV3D()         ,  ## recontruct PV
            Muons          ,  ## get muons 
            Check_3mu      ,  ## require at least 3 muons
            DiMuons        ,  ## get dimuons
            mu_and_dimu       ## good "dimu+mu" combination
            ], 
            )
        
        # =====================================================================
        # tau -> mu+ mu+ mu- line: 
        # =====================================================================
        line_tau_3mu = Hlt2Line (
            "TriMuonTau" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [  
            PV3D    ()    ,  ## recontruct PV
            Muons         ,  ## get muons  
            Check_3mu     ,  ## require at least 3 muons 
            GoodMuons     ,  ## select good muons 
            Check_3GoodMu ,  ## require at least 3 "good" muons 
            tau_3mu          ## check for tau->3mu candidates 
            ] , 
            )

        # =====================================================================
        # dimuon + high-pt gamma
        # =====================================================================        
        dimuon_and_photon = Hlt2Line (
            "DiMuonAndGamma" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [
            #
            Muons          ,  ## get muons 
            DiMuons        ,  ## get dimuons
            GoodPhotons    ,  ## get photons
            mu_and_gamma      ## good "dimu+photon" combinations 
            ], 
            )
        
        # =====================================================================
        # dimuon + D0 
        # =====================================================================        
        dimuon_and_D0 = Hlt2Line (
            "DiMuonAndD0" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [
            #
            PV3D()         ,
            Muons          ,  ## get muons 
            DiMuons        ,  ## get dimuons
            GoodKaons      ,  ## get kaons
            GoodPions      ,  ## get pions
            d0_2kpi        ,  ## make D0 -> K- pi+
            ## make dimuon + charm combinations 
            dimu_charm_ ( d0_2kpi , '[ chi_b2(1P) -> J/psi(1S) D0 ]cc' ) 
            ]
            )
        # =====================================================================
        # dimuon + D+
        # =====================================================================        
        dimuon_and_Dplus = Hlt2Line (
            "DiMuonAndDp" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [
            #
            PV3D()         ,
            Muons          ,  ## get muons 
            DiMuons        ,  ## get dimuons
            GoodKaons      ,  ## get kaons
            GoodPions      ,  ## get pions
            dp_2kpipi      ,  ## make D+ -> K- pi+ pi+
            ## make dimuon + charm combinations 
            dimu_charm_ ( dp_2kpipi , '[ chi_b2(2P) -> J/psi(1S) D+ ]cc' ) 
            ]
            )
        
        # =====================================================================
        # dimuon + Ds+
        # =====================================================================        
        dimuon_and_Ds = Hlt2Line (
            "DiMuonAndDs" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [
            #
            PV3D()         ,
            Muons          ,  ## get muons 
            DiMuons        ,  ## get dimuons
            GoodKaons      ,  ## get kaons
            GoodPions      ,  ## get pions
            ds_2kkpi       ,  ## make Ds+ -> K- K+ pi+
            ## make dimuon + charm combinations 
            dimu_charm_ ( ds_2kkpi , '[ chi_b2(2P) -> J/psi(1S) D_s+ ]cc' ) 
            ]
            )        

        # =====================================================================
        # dimuon + Lc+
        # =====================================================================        
        dimuon_and_Lc = Hlt2Line (
            "DiMuonAndLc" ,
            #
            prescale  = self.prescale  ,  ## prescale 
            postscale = self.postscale ,  ## postscale 
            # the main structure 
            algos     = [
            #
            PV3D()         ,
            Muons          ,  ## get muons 
            DiMuons        ,  ## get dimuons
            GoodKaons      ,  ## get kaons
            GoodProtons    ,  ## get protons
            GoodPions      ,  ## get pions
            lc_2pkpi       ,  ## make Lc+ -> p K- pi+
            ## make dimuon + charm combinations 
            dimu_charm_ ( lc_2pkpi , '[ chi_b2(2P) -> J/psi(1S) Lambda_c+ ]cc' ) 
            ]
            )
        
        # =====================================================================
        # define unique IDs
        # =====================================================================
        
        HltANNSvc().Hlt2SelectionID.update (
            {
            ## multi-muon lines 
            "Hlt2TriMuonDetachedDecision"      : 50214 ,
            "Hlt2DiMuonAndMuonDecision"        : 50215 ,
            "Hlt2DoubleDiMuonDecision"         : 50216 , 
            "Hlt2TriMuonTauDecision"           : 50217 ,
            ## dimuon + gamma
            "Hlt2DiMuonAndGammaDecision"       : 50218 ,
            ## dimuon + charm
            "Hlt2DiMuonAndD0Decision"          : 50219 ,
            "Hlt2DiMuonAndDpDecision"          : 50220 ,
            "Hlt2DiMuonAndDsDecision"          : 50221 ,
            "Hlt2DiMuonAndLcDecision"          : 50222 ,
            }
            )
        
        
# =============================================================================
if '__main__' == __name__ :
    print 100*'*'
    print __doc__
    print 100*'*'
# =============================================================================
# The END 
# =============================================================================
