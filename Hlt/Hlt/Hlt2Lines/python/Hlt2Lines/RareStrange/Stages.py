###
#  @author F. Dettori Francesco.Dettori@cern.ch

#  @date 2015-03-08
#
#  Please contact the abovementioned responsibles before editing this file
#
##
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

from HltTracking.HltPVs import PV3D
from Inputs import Hlt2Muons, Hlt2Pions, Hlt2Kaons, Hlt2Protons, Hlt2Electrons

class SigmaPMuMuCombiner(Hlt2Combiner):
    def __init__(self,name):
        
        daughters_cuts = { "mu+" : "(TRCHI2DOF< %(TrChi2)s) & (TRGHOSTPROB< %(TrGP)s) &" + \
                           "(MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)",
                           "p+" : "(TRCHI2DOF< %(TrChi2)s) & (TRGHOSTPROB< %(TrGP)s) & " + \
                           "(PIDp > %(pPIDp)s) & (MIPCHI2DV(PRIMARY)>%(pMinIpChi2)s)"
                           }
        
        combination_cuts = "(ADAMASS('Sigma+') < %(SigmaMassWin)s ) & " + \
                           "(AMAXDOCA('')      < %(SigmaMaxDOCA)s )"
        

        mother_cuts = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s) & " + \
                      "(PT                > %(SigmaMinPt)s ) & " + \
                      "(ADMASS('Sigma+')  < %(SigmaMassWin)s ) & "\
                      "(BPVDIRA           > %(SigmaMinDIRA)s) & "\
                      "(BPVIPCHI2()       < %(SigmaMaxIpChi2)s) & "\
                      "(BPVLTIME()        > %(SigmaMinTauPs)s )"

        inputs = [Hlt2Muons, Hlt2Protons]
        decay = "[Sigma+ -> p+ mu+ mu-]cc"
        Hlt2Combiner.__init__(self, name, decay ,
                              inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = daughters_cuts,
                              CombinationCut = combination_cuts,
                              MotherCut = mother_cuts,
                              Preambulo = [])


class KPiMuMuCombiner(Hlt2Combiner):
    def __init__(self,name):
        
        daughters_cuts = { "mu+" : "(TRCHI2DOF< %(TrChi2)s) & (TRGHOSTPROB< %(TrGP)s) &" + \
                                   "(MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)",
                           "pi+" : "(TRCHI2DOF< %(TrChi2)s) & (TRGHOSTPROB< %(TrGP)s) & "+\
                                   "(MIPCHI2DV(PRIMARY)>%(piMinIpChi2)s)",
                           }
        
        combination_cuts = "(ADAMASS('K+') < %(KMassWin)s ) & " + \
                           "(AMAXDOCA('')      < %(KMaxDOCA)s )"
        

        mother_cuts = "(VFASPF(VCHI2/VDOF)< %(KVtxChi2)s) & " + \
                      "(PT                > %(KMinPt)s ) & " + \
                      "(ADMASS('K+')  < %(KMassWin)s ) & "\
                      "(BPVDIRA           > %(KMinDIRA)s) & "\
                      "(BPVIPCHI2()       < %(KMaxIpChi2)s) & "\
                      "(BPVLTIME()        > %(KMinTauPs)s )"

        inputs = [Hlt2Muons, Hlt2Pions]
        decay = "[K+ -> pi+ mu+ mu-]cc"
        Hlt2Combiner.__init__(self, name, decay ,
                              inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = daughters_cuts,
                              CombinationCut = combination_cuts,
                              MotherCut = mother_cuts,
                              Preambulo = [])
        
    
class KPiMuMuSSCombiner(Hlt2Combiner):
    def __init__(self,name):
        
        daughters_cuts = { "mu+" : "(TRCHI2DOF< %(TrChi2)s) & (TRGHOSTPROB< %(TrGP)s) &" + \
                                   "(MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)",
                           "pi+" : "(TRCHI2DOF< %(TrChi2)s) & (TRGHOSTPROB< %(TrGP)s) & "+\
                                   "(MIPCHI2DV(PRIMARY)>%(piMinIpChi2)s)",
                           }
        
        combination_cuts = "(ADAMASS('K+') < %(KMassWin)s ) & " + \
                           "(AMAXDOCA('')      < %(KMaxDOCA)s )"
        

        mother_cuts = "(VFASPF(VCHI2/VDOF)< %(KVtxChi2)s) & " + \
                      "(PT                > %(KMinPt)s ) & " + \
                      "(ADMASS('K+')  < %(KMassWin)s ) & "\
                      "(BPVDIRA           > %(KMinDIRA)s) & "\
                      "(BPVIPCHI2()       < %(KMaxIpChi2)s) & "\
                      "(BPVLTIME()        > %(KMinTauPs)s )"

        inputs = [Hlt2Muons, Hlt2Pions]
        decay = "[K+ -> pi- mu+ mu+]cc"
        Hlt2Combiner.__init__(self, name, decay ,
                              inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = daughters_cuts,
                              CombinationCut = combination_cuts,
                              MotherCut = mother_cuts,
                              Preambulo = [])
        
    




