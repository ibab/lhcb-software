from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2JetBuilder import Hlt2JetBuilder
from HltTracking.HltPVs import PV3D
from Inputs import Hlt2Muons

class FilterSV(Hlt2ParticleFilter):
  def __init__(self, inputs):

      pid  = "((ABSID=='K+') | (ID=='KS0') | (ABSID=='Lambda0'))"

      code = ("(MINTREE("+pid+",PT) > %(SV_TRK_PT)s)"
              "& (MINTREE(ISBASIC,TRGHOSTPROB) < %(GHOSTPROB)s) "
              "& (MINTREE((ABSID=='K+'),MIPCHI2DV(PRIMARY)) > %(SV_TRK_IPCHI2)s)"
              "& (HASVERTEX) & (VFASPF(VCHI2PDOF) < %(SV_VCHI2)s)"
              "& (BPVVDCHI2 > %(SV_FDCHI2)s)")
      Hlt2ParticleFilter.__init__(self, 'JetsSV', code, inputs, shared=True,dependencies = [PV3D('Hlt2')])


class FilterMuon(Hlt2ParticleFilter):
  def __init__(self):

      inputs = [Hlt2Muons]
      code = ("(PT > %(MU_PT)s) &  (PROBNNmu > %(MU_PROBNNMU)s)"
              "& (TRGHOSTPROB < %(GHOSTPROB)s)")
      Hlt2ParticleFilter.__init__(self,'JetsMuon', code, inputs,
                                  shared = True, dependencies = [PV3D('Hlt2')])


class SVSVCombiner(Hlt2Combiner) :
  def __init__(self, inputs):

      cc = "(abs(ACHILD(BPVPHI,1)-ACHILD(BPVPHI,2)) > (%(DPHI)s-1.0))"

      Hlt2Combiner.__init__(self, 'JetsSVSV', "D0  -> K*(892)0 K*(892)0", inputs,
                            dependencies = [PV3D('Hlt2')],
                            CombinationCut = cc,
                            MotherCut = '(ALL)',
                            Preambulo = [],
                            shared = True,ParticleCombiners={ '' : 'ParticleAdder'})


class SVMuCombiner(Hlt2Combiner) :
  def __init__(self, inputs):

      cc = "(abs(ACHILD(BPVPHI,(ABSID=='K*(892)0'))-ACHILD(PHI,(ABSID=='mu+'))) > (%(DPHI)s-1.0))"
      Hlt2Combiner.__init__(self, 'JetsSVMu', ["D0 -> K*(892)0 mu+","D0 -> K*(892)0 mu-"], inputs,
                            dependencies = [PV3D('Hlt2')],
                            CombinationCut = cc,
                            MotherCut = '(ALL)',
                            Preambulo = [],
                            shared = True,ParticleCombiners={ '' : 'ParticleAdder'})

class MuMuCombiner(Hlt2Combiner) :
  def __init__(self, inputs):

      cc = "(abs(ACHILD(PHI,1)-ACHILD(PHI,2)) > (%(DPHI)s-1.0))"
      Hlt2Combiner.__init__(self, 'JetsMuMu', ["[D0 -> mu+ mu+]cc","D0 -> mu+ mu-"], inputs,
                            dependencies = [PV3D('Hlt2')],
                            CombinationCut = cc,
                            MotherCut = '(ALL)',
                            Preambulo = [],
                            shared = True,ParticleCombiners={ '' : 'ParticleAdder'})

# Combine two jets with optional tags.
class DiJetCombiner(Hlt2Combiner) :
  def __init__(self, inputs, tag1 = None, tag2 = None):
      cc = "(APT > %(JET_PT)s) & (abs(ACHILD(PHI,1)-ACHILD(PHI,2)) > %(DPHI)s)"
      if tag1: cc += "& (ACHILD(INFO(" + str(tag1) + "),1))";
      if tag2: cc += "& (ACHILD(INFO(" + str(tag2) + "),2))";
      name = ""
      if tag1: name += "SV" if tag1 == 9600 else "Mu"
      if tag2: name += "SV" if tag2 == 9600 else "Mu"
      Hlt2Combiner.__init__(self, 'DiJet' + name, 
                            ["CLUSjet -> CELLjet CELLjet"], inputs,
                            dependencies = [PV3D('Hlt2')],
                            CombinationCut = cc,
                            MotherCut = '(ALL)',
                            Preambulo = [],
                            shared = True, ParticleCombiners = 
                            {'': 'ParticleAdder'})
