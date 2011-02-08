## @file
#
#  Hlt2 dimuon selections
#
#  @author J. Albrecht, Leandro de Paula, Antonio Perez-Calero
#  @date 2009-02-13
#
#   a) unbiased dimuon selections, i.e. dimuon selections without
#           cuts correlated to the Bs lifetime (J. Albrecht)
#
#   b) biased dimuon and dimuon without PV (Leandro de Paula)
#
##
from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser



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
                   
                   ,'UnbiasedPsi2SMassWindow' :   70      # MeV
                   ,'UnbiasedPsi2SPt'         : 1000      # MeV
                   ,'UnbiasedPsi2SMuPt'       : 1500      # MeV
                   ,'UnbiasedPsi2SVertexChi2' :   25
                   
                   ,'UnbiasedBmmMinMass'      : 5200      # MeV
                   ,'UnbiasedBmmVertexChi2'   :   25

                   ,'UnbiasedZmmMinMass'      :40000      # MeV
                   ,'UnbiasedZmmPt'           :    0      # MeV

                   ,'DYPt'                    :    0      # MeV
                   ,'DY1MinMass'              : 2500      # MeV
                   ,'DY2MinMass'              : 5000      # MeV
                   ,'DY3MinMass'              :10000      # MeV 
                   ,'DY4MinMass'              :20000      # MeV
                   
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
                   ,'PromptJPsiMinMass'       : 2900      # MeV
                   ,'PromptJPsiPt'            :    0
                   ,'PromptJPsiHighPt'        : 2000      # MeV
                   ,'PromptJPsiMuPt'          :  700      # MeV
                   ,'PromptJPsiVChi2'         :   15

                   ,'DetachedDiMuonMinMass'   : 1000 #MeV
                   ,'DetachedDiMuonPt'        :    0  #MeV
                   ,'DetachedDiMuonMuPt'      :  500#Mev
                   ,'DetachedDiMuonVertexChi2':  25
                   ,'DetachedDiMuonDLS'       :   5

                   ,'DetachedHeavyDiMuonMinMass': 2950 # MeV
                   ,'DetachedHeavyDiMuonDLS'    : 3
                   ,'DetachedHavyDiMuonPt'      : 0

                   ,'NoPVPt'                    : 1500 # MeV
                   ,'NoPVMass'                  : 1000 # MeV

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
                                   , InputLocations  = [ BiKalmanFittedMuons ]
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
                               , InputLocations  = [ TrackFittedDiMuon ]
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
        prompt JPsi for cross section analysis
        '''
        line.clone( 'PromptJPsi'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>%(PromptJPsiMinMass)s*MeV)"\
                                 "& (PT>%(PromptJPsiPt)s*MeV) "\
                                 "& (MINTREE('mu-'==ABSID,PT)>%(PromptJPsiMuPt)s*MeV) "\
                                 "& (VFASPF(VCHI2PDOF)<%(PromptJPsiVChi2)s )"\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"%  self.getProps()
                                 , 'PostMonitor' :
                                   Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_out',nbins=25)
                                 }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2PromptJPsiDecision":  50212 } )

        #--------------------------------------------
        '''
        prompt JPsi for cross section analysis high JPsi PT
        '''
        line.clone( 'PromptJPsiHighPT'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>%(PromptJPsiMinMass)s*MeV)"\
                                 "& (PT>%(PromptJPsiHighPt)s*MeV) "\
                                 "& (MINTREE('mu-'==ABSID,PT)>%(PromptJPsiMuPt)s*MeV) "\
                                 "& (VFASPF(VCHI2PDOF)<%(PromptJPsiVChi2)s )"\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"%  self.getProps()
                                 , 'PostMonitor' :
                                    Hlt2Monitor( "M","M(#mu#mu)",3097,200,'M_out',nbins=25)
                                 }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2PromptJPsiHighPTDecision":  50213 } )
            
            
        #--------------------------------------------
        '''
        unbiased dimuon low mass - prescaled
        '''
        line.clone( 'DiMuonLowMass'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>%(UnbiasedDiMuonLowMinMass)s*MeV)"\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
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
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
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
        unbiased Psi(2S)
        '''
        line.clone( 'DiMuonPsi2S'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(ADMASS(3686.09*MeV)<%(UnbiasedPsi2SMassWindow)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
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
        unbiased Bmm
        '''
        line.clone( 'DiMuonB'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>%(UnbiasedBmmMinMass)s*MeV) "\
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
        '''
        unbiased Zmm
        '''
        line.clone( 'DiMuonZ'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "(MM>%(UnbiasedZmmMinMass)s*MeV) "\
                                 "& (PT>%(UnbiasedZmmPt)s*MeV) "% self.getProps() 
                                 , 'PostMonitor' :
                                 Hlt2Monitor( "M","M(#mu#mu)",90000,10000,'M_out',nbins=25)
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
                    , Filter = { 'Code': "  (MM>%(DY1MinMass)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                                 "& (PT>%(DYPt)s*MeV) " % self.getProps() }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY1Decision":   50206 } )

        #--------------------------------------------
        '''
        unbiased Drell-Yan 2
        '''
        line.clone( 'DiMuonDY2'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "  (MM>%(DY2MinMass)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                                 "& (PT>%(DYPt)s*MeV) " % self.getProps() }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY2Decision":   50207 } )

        #----------------------------------------------------------------------------------------

        '''
        unbiased Drell-Yan 3
        '''
        line.clone( 'DiMuonDY3'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "  (MM>%(DY3MinMass)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                                 "& (PT>%(DYPt)s*MeV) " % self.getProps() }
                    , postscale = self.postscale
                    )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDY3Decision":   50208 } )

        #--------------------------------------------
        '''
        unbiased Drell-Yan 4
        '''
        line.clone( 'DiMuonDY4'
                    , prescale = self.prescale 
                    , Filter = { 'Code': "  (MM>%(DY4MinMass)s*MeV) "\
                                 "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                                 "& (PT>%(DYPt)s*MeV) " % self.getProps() }
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
                                   , InputLocations = [ TrackFittedDiMuon ]
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
                                    , InputLocations = [ SimpleDiMuon ]
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
        ''' Inclusive Biased Jpsi
            Roel Aaij, roel.aaij@cern.ch

            This line is uses track fitted dimuons and cuts on a decay length significance of 3.

        '''

        filter = Hlt2Member(   FilterDesktop 
                               , "Filter"
                               , Code = "(MM>%(DetachedDiMuonMinMass)s*MeV)"\
                               "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                               " & (PT>%(DetachedDiMuonPt)s*MeV)"\
                               " & (MINTREE('mu-'==ABSID,PT)>%(DetachedDiMuonMuPt)s*MeV) "\
                               "& (VFASPF(VCHI2PDOF)<%(DetachedDiMuonVertexChi2)s )"\
                               "& (BPVDLS>%(DetachedDiMuonDLS)s )"%  self.getProps() 
                               , InputLocations  = [ TrackFittedDiMuon ]
                               )
        
        #--------------------------------------------
        '''
        detached heavy dimuon 
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
                                                    "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2)s )"\
                                                    " & (PT>%(DetachedHavyDiMuonPt)s*MeV)"\
                                                    " & (MINTREE('mu-'==ABSID,PT)>%(DetachedDiMuonMuPt)s*MeV) "\
                                                    "& (VFASPF(VCHI2PDOF)<%(DetachedDiMuonVertexChi2)s )"\
                                                    "& (BPVDLS>%(DetachedHeavyDiMuonDLS)s )"%  self.getProps() 
                                                    }
                    , postscale = self.postscale
                    )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDetachedHeavyDecision" : 50046 } )



        from Hlt2SharedParticles.TrackFittedDiMuon import DetachedTrackFittedJpsi2MuMu
        
        DiMuonBiasedJpsi = Hlt2Line ( 'DiMuonDetachedJPsi'
                                      , prescale = self.prescale 
                                      , algos = [ PV3D(), DetachedTrackFittedJpsi2MuMu ]
                                      , postscale = self.postscale
                                      )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonDetachedJPsiDecision" : 50044 } )

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
                               , InputLocations  = [ BiKalmanFittedMuons ]
                              )

        line = Hlt2Line('DiMuonNoPV'
                        , prescale = self.prescale
                        , HLT = Hlt1NoPVMuons
                        , algos = [BiKalmanFittedMuons, DiMuNoPV]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2DiMuonNoPVDecision" : 50047 } )

        #--------------------------------------------

