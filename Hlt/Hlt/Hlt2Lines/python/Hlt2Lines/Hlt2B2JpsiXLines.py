# $Id: Hlt2B2JpsiXLines.py,v 1.2 2009-06-24 09:36:52 gcowan Exp $

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
        #    ID 50360 -  selection for Bd -> J/psi K*
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBd2JpsiKstar.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarDecision" : 50360 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarSignalDecision" : 50361 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarLowMassDecision" : 50362 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bd2JpsiKstarHighMassDecision" : 50363 } )
        ###
        #    ID 50370 -  selection for Bd -> J/psi Phi
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBs2JpsiPhi.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiDecision" : 50370 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiSignalDecision" : 50371 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiSwaveDecision" : 50372 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiLowMassDecision" : 50373 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiPhiHighMassDecision" : 50374 } )
        ###
        #    ID 50380 -  selection for Bu -> J/psi K
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBu2JpsiK.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKDecision" : 50380 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKSignalDecision" : 50381 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKLowMassDecision" : 50382 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bu2JpsiKHighMassDecision" : 50383 } )
        ###
        #    ID 50050 - B -> J/psi(mumu)X
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineB2JpsiX_MuMu.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2B2JpsiX_MuMuDecision" : 50050 } )
        ###
        #    ID 50250 -  selection for Bs -> J/psi eta
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBs2JpsiEta.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiEtaDecision" : 50250 } )
        ###
        #    ID 50260 -  selection for Bs -> J/psi eta_prime
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineBs2JpsiEtap.py")
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2JpsiEtapDecision" : 50260 } )


#####################################################################################
__author__ = "Tristan du Pree and Greig Cowan"
__date__ = "2009-06-18"

from HltLine.HltLine import Hlt2Line, Hlt2Member
from Hlt2SharedParticles.BasicParticles import Muons, NoCutsKaons
from Configurables import CombineParticles

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
        from Configurables import HltANNSvc
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
