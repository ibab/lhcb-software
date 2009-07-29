# $Id: Hlt2B2PhiXLines.py,v 1.1 2009-07-29 11:33:46 kimym Exp $

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2PhiXLinesConf(HltLinesConfigurableUser) :
    '''
    Selection for Bs -> Phi Phi
    '''

    __slots__ = { 'KaonPtCut'               : 500 #MeV
                 ,'PhiMassWindow'           : 20  #MeV 
                 ,'BsMassWindow'            : 500 #MeV
                 ,'BsDaughtersPtProductCut' : 6000000 #MeV^2
                 ,'BsDIRACut'               : 0.999
                 ,'BsVertexChi2Cut'         : 50
                 ,'Prescale'                : {'Hlt2Bs2PhiPhi' : 0.1 }
                 ,'Postscale'               : {'Hlt2Bs2PhiPhi' : 1.  }
                 }

    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line, Hlt2Member
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.BasicParticles import NoCutsKaons
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiPhiDecision" : 55375 } )

        # Make the phi
        phiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , InputLocations = [NoCutsKaons] #[Hlt2GoodKaons]
                                 , DaughtersCuts = { "K+" : "(PT>"+ str(self.getProp('KaonPtCut')) +")" }
                                 , CombinationCut = "(ADAMASS('phi(1020)')<"+ str(self.getProp('PhiMassWindow')) +"*MeV)"
                                 , MotherCut = "(ALL)" #"(M<1100*MeV) & (PT>500*MeV)"
                                 )
								 
        # Make the Bs
        BsCombine = Hlt2Member(  CombineParticles
                                 , "BsCombine"
                                 , DecayDescriptor = "B_s0 ->  phi(1020) phi(1020)"
                                 , InputLocations  = [phiCombine]
                                 , CombinationCut = "(ADAMASS('B_s0')<"+ str(self.getProp('BsMassWindow')) +"*MeV) & (ACHILD(PT,1)*ACHILD(PT,2)>"+ str(self.getProp('BsDaughtersPtProductCut')) +")"
                                 , MotherCut = "(BPVDIRA>"+ str(self.getProp('BsDIRACut')) +") & (VFASPF(VCHI2)<"+ str(self.getProp('BsVertexChi2Cut')) +")"
                                 )
       
        line = Hlt2Line('Bs2PhiPhi'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [NoCutsKaons, phiCombine, BsCombine]
                        )
