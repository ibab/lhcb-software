###
#  @author F. Dettori Francesco.Dettori@cern.ch

#  @date 2016-02-12
#
#  Please contact the abovementioned responsibles before editing this file
#
##
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter

from HltTracking.HltPVs import PV3D
from Inputs import Hlt2Muons, Hlt2Pions, Hlt2Kaons, Hlt2Protons, Hlt2Electrons


class KPiPiPiCombiner(Hlt2Combiner):
    def __init__(self,name):
        
        daughters_cuts = { "pi+" : "(TRCHI2DOF< %(TrChi2)s) & (TRGHOSTPROB< %(TrGP)s) & "+\
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

        inputs = [ Hlt2Pions]
        decay = "[K+ -> pi+ pi+ pi-]cc"
        Hlt2Combiner.__init__(self, name, decay ,
                              inputs,
                              dependencies = [ PV3D('Hlt2') ],
                              DaughtersCuts = daughters_cuts,
                              CombinationCut = combination_cuts,
                              MotherCut = mother_cuts,
                              Preambulo = [])

    

