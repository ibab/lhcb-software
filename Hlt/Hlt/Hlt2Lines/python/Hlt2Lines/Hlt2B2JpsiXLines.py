# $Id: Hlt2B2JpsiXLines.py,v 1.12 2010-01-12 08:43:39 dvaneijk Exp $

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt2B2JpsiXLinesConf(HltLinesConfigurableUser) :

    __slots__ = {'BsLifetimeCut': 0.15 # ps
                 ,'Prescale'    : { 'Hlt2Bs2JpsiPhiPrescaled' : 1.
                                   ,'Hlt2Bs2JpsiPhiDetached' : 1.
				   ,'Hlt2Bs2JpsiPhiWide' : 0.05
                                   ,'Hlt2Bs2JpsiPhiSignal' : 1.
				   ,'Hlt2Bd2JpsiKstarWide' : 0.05
                                   ,'Hlt2Bd2JpsiKstarSignal' : 1.
       				   ,'Hlt2Bu2JpsiKWide' : 0.05
                                   ,'Hlt2Bu2JpsiKSignal' : 1.
	                           ,'Hlt2Bd2JpsiMuMuKsLLBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsLLUnbiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDUnbiased' : 1.
                                   ,'Hlt2B2JpsiX_MuMu' : 1.
                                    }
                 ,'Postscale'   : { 'Hlt2Bs2JpsiPhiPrescaled' : 0.1
                                   ,'Hlt2Bs2JpsiPhiDetached' : 1.
                 		   ,'Hlt2Bs2JpsiPhiWide' : 1.
                                   ,'Hlt2Bs2JpsiPhiSignal' : 1.
				   ,'Hlt2Bd2JpsiKstarWide' : 1.
                                   ,'Hlt2Bd2JpsiKstarSignal' : 1.
                  		   ,'Hlt2Bu2JpsiKWide' : 1.
                                   ,'Hlt2Bu2JpsiKSignal' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsLLBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsLLUnbiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDBiased' : 1.
                                   ,'Hlt2Bd2JpsiMuMuKsDDUnbiased' : 1.
                                   ,'Hlt2B2JpsiX_MuMu' : 1.
                                    }
                 }
    
    def __apply_configuration__(self) :
	self.__makeHlt2Bs2JpsiPhiPrescaledAndDetachedLines()
        self.__makeHlt2Bs2JpsiPhiLines()
        self.__makeHlt2Bd2JpsiKstarLines()
        self.__makeHlt2Bu2JpsiKLines()
    	self.__makeHlt2Bd2JpsiMuMuKsLLBiasedLines()
    	self.__makeHlt2Bd2JpsiMuMuKsLLUnbiasedLines()
    	self.__makeHlt2Bd2JpsiMuMuKsDDBiasedLines()
    	self.__makeHlt2Bd2JpsiMuMuKsDDUnbiasedLines()	
	self.__makeHlt2B2JpsiX_MuMuLines()

    def __makeHlt2Bs2JpsiPhiPrescaledAndDetachedLines(self):
        '''
        Prescaled and detached selection for Bs -> J/psi Phi
        
        @author Greig Cowan
        @author Tristan du Pree
        @date 2009-05-19
        '''
    
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import Muons, NoCutsKaons
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiPrescaledDecision" : 50375 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiDetachedDecision" : 50376 } )
        
        # Make the J/psi
        # We should use the HltSharedParticles for this, but they are not quite what
        # we want. Discussion required.
        jPsiCombine = Hlt2Member( CombineParticles
                                  , "JpsiCombine"
                                  , DecayDescriptor = "J/psi(1S) -> mu+ mu-"
                                  , InputLocations = [Muons]
                                  , MotherCut = "(ADMASS('J/psi(1S)')<100*MeV)"
                                  )

        # Make the phi
        phiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , InputLocations = [NoCutsKaons]
                                 , MotherCut = "(M<1100*MeV) & (PT>500*MeV)"
                                 )
        
        # Make the Bs
        BsCuts = "(ADMASS('B_s0')<300*MeV) & (BPVLTFITCHI2()<36) & (VFASPF(VCHI2)<100)"
        BsCombine = Hlt2Member( CombineParticles
                                , "BsCombine"
                                , DecayDescriptor = "[B_s0 -> J/psi(1S) phi(1020)]cc"
                                , InputLocations  = [jPsiCombine, phiCombine]
                                , MotherCut = BsCuts
                                )
       
        line = Hlt2Line('Bs2JpsiPhiPrescaled'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [Muons, jPsiCombine, NoCutsKaons, phiCombine, BsCombine]
                        )

        # Now do the detached 
        # Note: we should _share_ the J/psi and phi between these two lines!!
        BsCutsDetached = BsCuts + "& (BPVLTIME()>" + str(self.getProp('BsLifetimeCut')) + "*ps)"
        line.clone('Bs2JpsiPhiDetached'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [Muons, jPsiCombine, NoCutsKaons, phiCombine, BsCombine]
                   , BsCombine = {"MotherCut": BsCutsDetached}
                   )


    def __makeHlt2Bs2JpsiPhiLines(self):
        '''
        Prescaled Bs -> J/psi Phi selection

        @author Geraldine Conti
        @author Adlene Hicheur
        @date 2009-07-19
        '''
    
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiWideDecision" : 50370 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiSignalDecision" : 50371 } )
    
        # Make the phi
        PhiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , InputLocations = [NoCutsKaons]
                                 , CombinationCut = "(AM<1045*MeV) & (APT>1000*MeV)"
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<25)"
                                 )

        # Make the Bs
        BsCombine = Hlt2Member( CombineParticles
                                , "BsCombine"
                                , DecayDescriptor = "[B_s0 -> J/psi(1S) phi(1020)]cc"
                                , InputLocations  = [HighPtJpsi2MuMu, PhiCombine]
                                , CombinationCut = "(ADAMASS('B_s0')<300*MeV)"
                                , MotherCut = "(VFASPF(VCHI2/VDOF)<6)"
                                )
        
        line = Hlt2Line('Bs2JpsiPhiWide'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [HighPtJpsi2MuMu, NoCutsKaons, PhiCombine, BsCombine]
                        )
        
        # Now do the Signal selection
        line.clone('Bs2JpsiPhiSignal'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , BsCombine = {"CombinationCut" :  "(ADAMASS('B_s0')<100*MeV)"}
                   )
        

    def __makeHlt2Bd2JpsiKstarLines(self):
        '''
        Prescaled Bd -> J/psi K* selection 
        
        @author Geraldine Conti
        @author Adlene Hicheur
        @date 2009-07-19
        '''
        
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons, NoCutsPions
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarWideDecision" : 50360 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarSignalDecision" : 50361 } )
        
        # Make the Kstar
        KstarCombine = Hlt2Member( CombineParticles
                                   , "KstarCombine"
                                   , DecayDescriptor = "K*(892)0 -> K+ pi-"
                                   , InputLocations = [NoCutsKaons, NoCutsPions]
                                   , CombinationCut = "(ADAMASS('K*(892)0')<120*MeV) & (APT>1000*MeV)"
                                   , MotherCut = "(VFASPF(VCHI2/VDOF)<25)"
                                   )
        
        # Make the Bd
        BdCombine = Hlt2Member( CombineParticles
                                , "BdCombine"
                                , DecayDescriptor = "B0 -> J/psi(1S) K*(892)0"
                                , InputLocations  = [HighPtJpsi2MuMu, KstarCombine]
                                , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                                , MotherCut = "(VFASPF(VCHI2/VDOF)<6) & (PT>2000*MeV)"
                                )
        
        line = Hlt2Line('Bd2JpsiKstarWide'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [HighPtJpsi2MuMu, NoCutsKaons, NoCutsPions, KstarCombine, BdCombine]
                        )
        
        # Now do the Signal selection
        line.clone('Bd2JpsiKstarSignal'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , BdCombine = {"CombinationCut" :  "(ADAMASS('B0')<100*MeV)"}
                   )
        
    def __makeHlt2Bu2JpsiKLines(self):
        '''
        Prescaled Bu -> J/psi K selection 
        
        @author Geraldine Conti
        @author Adlene Hicheur
        @date 2009-07-19
        '''
        
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKWideDecision" : 50380 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKSignalDecision" : 50381 } )
        
        # Make the Bu
        BuCombine = Hlt2Member( CombineParticles
                                , "BuCombine"
                                , DecayDescriptor = "[B+ -> J/psi(1S) K+]cc"
                                , InputLocations  = [HighPtJpsi2MuMu, NoCutsKaons]
                                , CombinationCut = "(ADAMASS('B+')<300*MeV)"
                                , MotherCut = "(VFASPF(VCHI2/VDOF)<6)"
                                , DaughtersCuts = {"K+" : "(PT>1300*MeV)"}
                                )
        
        line = Hlt2Line('Bu2JpsiKWide'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [HighPtJpsi2MuMu, NoCutsKaons, BuCombine]
                        )
    
        # Now do the Signal selection
        line.clone('Bu2JpsiKSignal'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , BuCombine = {"CombinationCut" :  "(ADAMASS('B+')<100*MeV)"}
                   )

    def __makeHlt2Bd2JpsiMuMuKsLLBiasedLines(self):
        '''
	@author S Amato
	@author C Gobel
	@author F Rodrigues
	'''
	from HltLine.HltLine import Hlt2Line, Hlt2Member
        from HltLine.HltReco import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.Ks import KsLL
        from Configurables import CombineParticles, FilterDesktop
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsLLBiasedDecision" : 50320 } )
        
        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 #,Code = "(MM>3037*MeV) & (MM<3157*MeV) & (MINTREE('mu-'==ABSID,PT)>0.5*GeV) & (BPVVDCHI2 > 4) "
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (BPVVDCHI2>7)  "
                                 , InputLocations = [ Jpsi2MuMu ]
                                 )

        filterKS = Hlt2Member( FilterDesktop # type
                               , "FilterKS"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = "VFASPF(VCHI2/VDOF)<16 "
                               , InputLocations = [ KsLL ]
                               )

        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(VFASPF(VCHI2/VDOF)<55) & (BPVIPCHI2()< 50) "
                               , InputLocations = ['%FilterJpsi','%FilterKS' ]
                               )

        line = Hlt2Line('Bd2JpsiMuMuKsLLBiased'
		        , prescale = self.prescale
                   	, postscale = self.postscale	
                        , algos = [   Jpsi2MuMu, PV3D, filterJpsi
                                       , KsLL,      filterKS
                                       , combineB
                                       ]
                        )


    def __makeHlt2Bd2JpsiMuMuKsLLUnbiasedLines(self):
        '''
        @author S Amato
        @author C Gobel
        @author F Rodrigues
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.Ks import KsLL
        from Configurables import CombineParticles, FilterDesktop
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsLLUnbiasedDecision" : 50340 } )
        
        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (MINTREE('mu-'==ABSID,PT)>0.8*GeV)"
                                 , InputLocations = [ Jpsi2MuMu ]
                                 )
        
        filterKS = Hlt2Member( FilterDesktop # type
                               , "FilterKS"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = "(PT > 1.0*GeV)"
                               , InputLocations = [ KsLL ]
                               )
        
        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(PT>0.3*GeV) & (VFASPF(VCHI2/VDOF)<25)"
                               , InputLocations = ['%FilterJpsi','%FilterKS' ]
                               )
        
        line = Hlt2Line('Bd2JpsiMuMuKsLLUnbiased'
                        , prescale = self.prescale
                   	, postscale = self.postscale
			,  algos = [   Jpsi2MuMu , filterJpsi
                                       , KsLL,       filterKS
                                       , combineB
                                       ]
                        )

    def __makeHlt2Bd2JpsiMuMuKsDDBiasedLines(self):
        '''
        @author S Amato
        @author C Gobel
        @author F Rodrigues
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from HltLine.HltReco import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.V0 import KsDD
        from Configurables import GaudiSequencer,  FilterDesktop
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsDDBiasedDecision" : 50330 } )

        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (BPVVDCHI2>7)  "
                                 , InputLocations = [ Jpsi2MuMu ]
                                 )

        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (BPVIPCHI2()< 45)"
                               , InputLocations = ['%FilterJpsi',KsDD ]
                               )
        line = Hlt2Line('Bd2JpsiMuMuKsDDBiased'
                        , prescale = self.prescale
                   	, postscale = self.postscale
                        ,  algos = [   Jpsi2MuMu , PV3D, filterJpsi
                                       , KsDD
                                       , combineB
                                       ]
                        )

    def __makeHlt2Bd2JpsiMuMuKsDDUnbiasedLines(self):
        '''
        @author S Amato
        @author C Gobel
        @author F Rodrigues
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from HltLine.HltReco import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import Jpsi2MuMu
        from Hlt2SharedParticles.V0 import KsDD
        from Configurables import GaudiSequencer, FilterDesktop
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsDDUnbiasedDecision" : 50350 } )

        filterJpsi = Hlt2Member( FilterDesktop # type
                                 , "FilterJpsi"      # name -- to be bound to the line name: Hlt2LineNameFilter
                                 , Code = "(MM>3037*MeV) & (MM<3157*MeV) & (MINTREE('mu-'==ABSID,PT)>0.8*GeV)"
                                 , InputLocations = [ Jpsi2MuMu ]
                                 )

        filterKS = Hlt2Member( FilterDesktop # type
                               , "FilterKS"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , Code = "(PT > 0.8*GeV)"
                               , InputLocations = [KsDD ]
                               )

        combineB = Hlt2Member( CombineParticles # type
                               , "CombineB"      # name -- to be bound to the line name: Hlt2LineNameFilter
                               , DecayDescriptor = "B0 -> J/psi(1S) KS0"
                               , CombinationCut = "(ADAMASS('B0')<300*MeV)"
                               , MotherCut = "(PT>0.3*GeV) & (VFASPF(VCHI2/VDOF)<20) & (BPVIPCHI2()< 30)"
                               , InputLocations = [filterJpsi,filterKS ]
                               )

        line = Hlt2Line('Bd2JpsiMuMuKsDDUnbiased'
                   	, prescale = self.prescale
                   	, postscale = self.postscale
                        ,  algos = [   Jpsi2MuMu, PV3D, filterJpsi
                                       , KsDD, filterKS
                                       , combineB
                                       ]
                        )

    def __makeHlt2B2JpsiX_MuMuLines(self):
        '''
        @author P Koppenburg
        '''
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.DiMuon import DiMuon
        from Configurables import FilterDesktop
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2JpsiX_MuMuDecision" : 50050 } )

        filter = Hlt2Member( FilterDesktop # type
                             , "Filter"      # name -- to be bound to the line name: Hlt2LineNameFilter
                             , Code = "(ADMASS('J/psi(1S)')<50*MeV) & (MINTREE('mu-'==ABSID,PT)>1.5*GeV)"
                             , InputLocations = [ DiMuon ]
                             )
        # @todo Add some IP cuts                   

        Hlt2Line('B2JpsiX_MuMu' 
		   	, prescale = self.prescale
                   	, postscale = self.postscale
		   	, algos = [ DiMuon , filter ]
			)
