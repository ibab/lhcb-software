from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from HltTracking.HltPVs import PV3D
from Inputs import UnbiasedPhi2KK, Hlt2Muons 

class DisplPhiPhi(Hlt2Combiner):
  def __init__(self, name):

    inputs = [UnbiasedPhi2KK]

    dc = {}
    dc['phi(1020)'] =  ("(PT > %(PhiPT)s) " 
                        "& (MINTREE('K+'==ABSID,PT) > %(KPT)s) " 
                        "& (MINTREE('K+'==ABSID,BPVIPCHI2()) > %(KIPChi2)s) " 
                        "& (MAXTREE('K+'==ABSID,TRGHOSTPROB) < %(GhostProb)s) " 
                        "& (MINTREE('K+'==ABSID,PROBNNK) > %(KProbNNk)s) " 
                        "& (VFASPF(VCHI2PDOF) < %(VChi2)s) " 
                        "& (in_range( PDGM('phi(1020)') - %(PhiMassWindow)s , M , PDGM('phi(1020)') + %(PhiMassWindow)s ) )") 

    mc =  ("(HASVERTEX)"
           "& (VFASPF(VCHI2) < %(VChi2)s) "
           "& (BPVVDCHI2 > %(FDChi2)s)") 

    Hlt2Combiner.__init__(self, name, "B0 -> phi(1020) phi(1020)", inputs,
                          dependencies = [PV3D('Hlt2')],
                          tistos = 'TisTosSpec',
                          DaughtersCuts  = dc,
                          #CombinationCut = cc,
                          MotherCut      = mc,
                          Preambulo = [])
   

class SharedDiMuonNoIP(Hlt2Combiner) :
  def __init__(self, name):

    inputs = [Hlt2Muons]
    
    dc = {}
    dc['mu+'] = ("(PT > %(MuPT)s) " 
                 "& (P > %(MuP)s) " 
                 "& (TRGHOSTPROB < %(GhostProb)s) " 
                 "& (PROBNNmu > %(MuProbNNmu)s) ") 
    cc = "(AMAXDOCA('') < %(DOCA)s)" 
    mc = "(VFASPF(VCHI2PDOF) < %(VChi2)s) " 

    Hlt2Combiner.__init__(self, name, "KS0 -> mu+ mu-", inputs,
                          dependencies = [PV3D('Hlt2')],
                          DaughtersCuts  = dc,
                          CombinationCut = cc,
                          MotherCut      = mc,
                          Preambulo = [],
                          shared = True)

     
class QuadMuonNoIP(Hlt2Combiner):
  def __init__(self, name, inputs):

    cc = "APT > %(PT)s"
    mc =  ("(HASVERTEX)" 
           "& (VFASPF(VCHI2) < %(VChi2)s) ") 
    Hlt2Combiner.__init__(self, name, "B0 -> KS0 KS0", inputs,
                          dependencies = [PV3D('Hlt2')],
                          #DaughtersCuts  = dc,
                          CombinationCut = cc,
                          MotherCut      = mc,
                          Preambulo = [])


class DisplDiMuon(Hlt2ParticleFilter):
  def __init__(self, name, inputs):

    code = ("(MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(MuIPChi2)s)"
            "& (MINTREE('mu+'==ABSID,PROBNNmu) > %(MuProbNNmu)s)" 
            "& (PT > %(PT)s)" 
            "& (HASVERTEX)" 
            "& (BPVIPCHI2() < %(IPChi2)s)" 
            "& (BPVVDCHI2 > %(FDChi2)s)") 
    Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [PV3D('Hlt2')])


class DisplDiMuonNoPoint(Hlt2ParticleFilter):
  def __init__(self, name, inputs):

    code = ("(MINTREE('mu+'==ABSID,BPVIPCHI2()) > %(MuIPChi2)s)" 
            "& (MINTREE('mu+'==ABSID,PROBNNmu) > %(MuProbNNmu)s)" 
            "& (PT > %(PT)s)" 
            "& (HASVERTEX)" 
            "& (BPVVDCHI2 > %(FDChi2)s)") 
    Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [PV3D('Hlt2')])



class PrmptDiMuon(Hlt2ParticleFilter):
  def __init__(self, name, inputs, highmass = False):

    code = ''
    if highmass: code = '(M > %(M)s) &'
    code += ("(MINTREE('mu+'==ABSID,PT) > %(MuPT)s) & (MINTREE('mu+'==ABSID,P) > %(MuP)s)" 
             "& (MINTREE('mu+'==ABSID,BPVIPCHI2()) < %(MuIPChi2)s)" 
             "& (MINTREE('mu+'==ABSID,PROBNNmu) > %(MuProbNNmu)s)" 
             "& (PT > %(PT)s)"  
             "& (HASVERTEX)" 
             "& (BPVVDCHI2 < %(FDChi2)s)") 
    Hlt2ParticleFilter.__init__(self, name, code, inputs, dependencies = [PV3D('Hlt2')])


class PrmptDiMuonSS(Hlt2Combiner) :
  def __init__(self, name):

    inputs = [Hlt2Muons]
    
    dc = {}
    dc['mu+'] = ("(PT > %(MuPT)s) " 
                 "& (P > %(MuP)s) " 
                 "& (BPVIPCHI2() < %(MuIPChi2)s) " 
                 "& (TRGHOSTPROB < %(GhostProb)s) " 
                 "& (PROBNNmu > %(MuProbNNmu)s) ") 
    cc = "(APT > %(PT)s) &  (AMAXDOCA('') < %(DOCA)s)" 
    mc = ("(VFASPF(VCHI2PDOF) < %(VChi2)s) " 
          "& (PT > %(PT)s)" 
          "& (HASVERTEX)" 
          "& (BPVVDCHI2 < %(FDChi2)s)")

    Hlt2Combiner.__init__(self, name, "[KS0 -> mu+ mu+]cc", inputs,
                          dependencies = [PV3D('Hlt2')],
                          DaughtersCuts  = dc,
                          CombinationCut = cc,
                          MotherCut      = mc,
                          Preambulo = [])
