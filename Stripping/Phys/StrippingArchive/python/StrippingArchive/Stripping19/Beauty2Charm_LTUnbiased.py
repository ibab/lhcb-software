#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from StandardParticles import StdAllNoPIDsKaons

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class LTUnbiasedBuilder(object):
    '''Makes all lifetime unbiased decays for the Beauty2Charm module.'''
    
    def __init__(self,d,config):
        self.config = deepcopy(config)
        self.kaons = [topoInputs('PromptKaons',[StdAllNoPIDsKaons])]
        self.d = d
        self.lines = []  
        self._makeB02DH('D2HHH',self.d.hhh_cf_pid)

    def _makeSels(self,decays,xtag,inputs):
        sels = []
        for tag, decay in decays.iteritems():
            comboCuts = LoKiCuts(['SUMPT','AM'],self.config).code()
            momCuts = [LoKiCuts(['VCHI2DOF'],self.config).code(),
                       hasTopoChildren()]
            momCuts = LoKiCuts.combine(momCuts)
            b2x = CombineParticles(DecayDescriptors=decay,
                                   CombinationCut=comboCuts,
                                   MotherCut=momCuts)
            sel = Selection(tag+xtag+'Beauty2Charm',Algorithm=b2x,
                            RequiredSelections=inputs[tag])
            sels.append(sel)
        return sels

    def _makeB02DH(self,dname,d2x):
        decays = {'B02DKLTUB' : ["[B0 -> D- K+]cc"]}
        inputs = {'B02DKLTUB' : d2x+self.kaons}
        sel = self._makeSels(decays,dname,inputs)
        self.lines.append(ProtoLine(sel,1.0))

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
