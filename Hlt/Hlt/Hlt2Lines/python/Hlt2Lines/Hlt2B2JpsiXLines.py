

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
