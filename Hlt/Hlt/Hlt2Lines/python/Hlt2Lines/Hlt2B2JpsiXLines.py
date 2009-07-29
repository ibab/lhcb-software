# $Id: Hlt2B2JpsiXLines.py,v 1.5 2009-07-29 16:30:07 conti Exp $

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2JpsiXLinesConf(HltLinesConfigurableUser) :

    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        ###
        #    ID 50320 -  selection for Bd -> J/psi Ks, lifetime biased with Ks->LL
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBd2JpsiMuMuKsLLBiased.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsLLBiasedDecision" : 50320 } )
        ###
        #    ID 50330 -  selection for Bd -> J/psi Ks, lifetime biased with Ks->DD
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBd2JpsiMuMuKsDDBiased.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsDDBiasedDecision" : 50330 } )
        ###
        #    ID 50340 -  selection for Bd -> J/psi Ks, lifetime unbiased with Ks->LL
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBd2JpsiMuMuKsLLUnbiased.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsLLUnbiasedDecision" : 50340 } )
        ###
        #    ID 50350 -  selection for Bd -> J/psi Ks, lifetime unbiased with Ks->DD
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBd2JpsiMuMuKsDDUnbiased.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiMuMuKsDDUnbiasedDecision" : 50350 } )
        ###
        #    ID 50050 - B -> J/psi(mumu)X
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineB2JpsiX_MuMu.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2JpsiX_MuMuDecision" : 50050 } )


#####################################################################################
__author__ = "Tristan du Pree and Greig Cowan"
__date__ = "2009-06-18"


class Hlt2Bs2JpsiPhiPrescaledAndDetachedLinesConf(HltLinesConfigurableUser) :
    '''
    Prescaled and detached selection for Bs -> J/psi Phi
    '''

    __slots__ = {'BsLifetimeCut': 0.15 # ps
                 ,'Prescale'    : {'Hlt2Bs2JpsiPhiPrescaled' : 0.1
                                   ,'Hlt2Bs2JpsiPhiDetached' : 1. }
                 ,'Postscale'   : {'Hlt2Bs2JpsiPhiPrescaled' : 1.
                                   ,'Hlt2Bs2JpsiPhiDetached' : 1.} 
                 }

    def __apply_configuration__(self) :
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
        BsCuts = "(ADMASS('B_s0')<300*MeV) & (BPVLTFITCHI2()<14) & (VFASPF(VCHI2)<20)"
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
        BsCutsDetached = BsCuts + "& (BPVLTIME()>" + str(self.getProp('BsLifetimeCut')) + "*ps)"
        line.clone('Bs2JpsiPhiDetached'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , BsCombine = {"MotherCut": BsCutsDetached}
                   )

#####################################################################################
__author__ = "Geraldine Conti, Adlene Hicheur"
__date__ = "2009-07-19"


class Hlt2Bs2JpsiPhiLinesConf(HltLinesConfigurableUser) :
    '''
    Prescaled Bs -> J/psi Phi selection 
    '''

    __slots__ = { 'Prescale'    : { 'Hlt2Bs2JpsiPhi' : 0.05
                                   ,'Hlt2Bs2JpsiPhiSignal' : 1.}
                 ,'Postscale'   : { 'Hlt2Bs2JpsiPhi' : 1.
                                   ,'Hlt2Bs2JpsiPhiSignal' : 1.} 
                 }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiDecision" : 50370 } )
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
       
        line = Hlt2Line('Bs2JpsiPhi'
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


#####################################################################################
__author__ = "Geraldine Conti, Adlene Hicheur"
__date__ = "2009-07-19"


class Hlt2Bd2JpsiKstarLinesConf(HltLinesConfigurableUser) :
    '''
    Prescaled Bd -> J/psi K* selection 
    '''

    __slots__ = { 'Prescale'    : { 'Hlt2Bd2JpsiKstar' : 0.05
                                   ,'Hlt2Bd2JpsiKstarSignal' : 1.}
                 ,'Postscale'   : { 'Hlt2Bd2JpsiKstar' : 1.
                                   ,'Hlt2Bd2JpsiKstarSignal' : 1.} 
                 }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons, NoCutsPions
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarDecision" : 50360 } )
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
       
        line = Hlt2Line('Bd2JpsiKstar'
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


#####################################################################################
__author__ = "Geraldine Conti, Adlene Hicheur"
__date__ = "2009-07-19"


class Hlt2Bu2JpsiKLinesConf(HltLinesConfigurableUser) :
    '''
    Prescaled Bu -> J/psi K* selection 
    '''

    __slots__ = { 'Prescale'    : { 'Hlt2Bu2JpsiK' : 0.05
                                   ,'Hlt2Bu2JpsiKSignal' : 1.}
                 ,'Postscale'   : { 'Hlt2Bu2JpsiK' : 1.
                                   ,'Hlt2Bu2JpsiKSignal' : 1.} 
                 }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Hlt2SharedParticles.DiMuon import HighPtJpsi2MuMu
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKDecision" : 50380 } )
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
       
        line = Hlt2Line('Bu2JpsiK'
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


