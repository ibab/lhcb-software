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
                   ,'UnbiasedDiMuonLowMinMass':  500      # MeV
                   ,'UnbiasedDiMuonLowPt'     : 1000      # MeV
                   ,'UnbiasedDiMuonLowMuPt'   :  500      # MeV
                   ,'UnbiasedDiMuonLowChi2'   :   25
                                       
                   ,'UnbiasedJPsiMassWindow'  :   70
                   ,'UnbiasedJPsiPt'          : 1000
                   ,'UnbiasedJPsiMuPt'        :  500
                   ,'UnbiasedJPsiVertexChi2'  :   25
                   
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
        unbiased dimuon low mass - prescaled
        '''
        from Hlt2SharedParticles.TrackFittedDiMuon import TrackFittedDiMuon
        Filter = Hlt2Member(FilterDesktop, 'Filter',
                            Code = ("(MM>%(UnbiasedDiMuonLowMinMass)s*MeV)"\
                                    "& (MAXTREE('mu-'==ABSID,TRCHI2DOF) < %(TrChi2Tight)s )"\
                                    "& (PT>%(UnbiasedDiMuonLowPt)s*MeV) "\
                                    "& (MINTREE('mu-'==ABSID,PT)>%(UnbiasedDiMuonLowMuPt)s*MeV) "\
                                    "& (VFASPF(VCHI2PDOF)<%(UnbiasedDiMuonLowChi2)s )"%  self.getProps()),
                            UseP2PVRelations = False,
                            Inputs  = [ TrackFittedDiMuon ])

        line = Hlt2Line('DiMuonLowMass'
                        , prescale = self.prescale 
                        , algos = [ TrackFittedDiMuon, Filter ]
                        , postscale = self.postscale
                        )
        #--------------------------------------------
        '''
        unbiased J/psi for Turbo stream 
        '''
        line.clone( 'DiMuonJPsiTurbo'
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
		    , Turbo = True
                              
                    )
        #--------------------------------------------
        def addPT( code, props, ptkey  ) :
            if props[ptkey] > 0 :   code = code + "& (PT>%s*MeV)"% props[ptkey]
            return code % props

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
            PV3D('Hlt2')         ,    ## recontruct PV
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
            PV3D('Hlt2')         ,  ## recontruct PV
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
            PV3D('Hlt2')        ,  ## recontruct PV
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
            PV3D('Hlt2')         ,
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
            PV3D('Hlt2')         ,
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
            PV3D('Hlt2')         ,
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
            PV3D('Hlt2')         ,
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
