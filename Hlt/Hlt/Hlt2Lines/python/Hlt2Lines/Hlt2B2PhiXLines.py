# $Id: Hlt2B2PhiXLines.py,v 1.9 2010-08-22 22:46:02 gligorov Exp $

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2B2PhiXLinesConf(HltLinesConfigurableUser) :
    '''
    Selection for Bs -> Phi Phi

    @author KIM Young Min  <s0679231@sms.ed.ac.uk>
    @date 29-07-2009
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
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons
        from Configurables import CombineParticles
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Bs2PhiPhiDecision" : 55375 } )

        # Make the phi
        phiCombine = Hlt2Member( CombineParticles
                                 , "PhiCombine"
                                 , DecayDescriptor = "phi(1020) -> K+ K-"
                                 , Inputs = [BiKalmanFittedKaons] #[Hlt2GoodKaons]
                                 , InputPrimaryVertices = "None"
                                 , UseP2PVRelations = False
                                 , DaughtersCuts = { "K+" : "(PT>%(KaonPtCut)s)"% self.getProps() }
                                 , CombinationCut = "(ADAMASS('phi(1020)')<%(PhiMassWindow)s*MeV)"% self.getProps()
                                 , MotherCut = "(ALL)" #"(M<1100*MeV) & (PT>500*MeV)"
                                 )
								 
        # Make the Bs
        BsCombine = Hlt2Member(  CombineParticles
                                 , "BsCombine"
                                 , DecayDescriptor = "B_s0 ->  phi(1020) phi(1020)"
                                 , Inputs  = [phiCombine]
                                 , CombinationCut = "(ADAMASS('B_s0')<%(BsMassWindow)s*MeV) & (ACHILD(PT,1)*ACHILD(PT,2)>%(BsDaughtersPtProductCut)s) "% self.getProps()
                                 , MotherCut = "(BPVDIRA>%(BsDIRACut)s) & (VFASPF(VCHI2)<%(BsVertexChi2Cut)s)"%self.getProps()
                                 )
       
        line = Hlt2Line('Bs2PhiPhi'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [PV3D(), BiKalmanFittedKaons, phiCombine, BsCombine]
                        )
