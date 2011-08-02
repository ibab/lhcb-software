#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Beauty2Charm_ComboEngine import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DFilter(object):
    '''Produces filtered D mesons for the Beauty2Charm module.'''

    def __init__(self,d,dst,lc,config):
        self.hhh = d.hhh
        self.hh = d.hh
        self.d0pi = dst.d0pi
        self.pkpi = lc.pkpi
        self.config = config
        self.tightCuts = [LoKiCuts(['MIPCHI2DV','VCHI2DOF','BPVVDCHI2',
                                    'MIPCHI2DV','BPVDIRA','BPVVDRHO','BPVVDZ'],
                                   config).code()]
        self.fhhh = self._makeFiltCharm(self.hhh,'DFilt','D')
        self.fhh = self._makeFiltCharm(self.hh,'D0Filt','D0')
        self.fdst = self._makeFiltCharm(self.d0pi,'DstFilt','Dst')
        self.flc = self._makeFiltCharm(self.pkpi,'LcFilt','Lc')
        self.hh_biased = self._makeBiased('D2HH',d.hh)
        self.hhh_biased = self._makeBiased('D2HHH',d.hhh)
        self.dst_biased = self._makeBiased('Dst2DH',dst.d0pi)

    def _makeFiltCharm(self,input,name,tag):                        
        x = self.tightCuts
        dCuts = deepcopy(x)
        childPIDp = "(NINGENERATION( ('p+'==ABSID) & (PIDp < %s), 1) < 1 )"\
                    %(self.config['pPIDp_MIN'])
        childPIDK = "(NINGENERATION( ('K+'==ABSID) & (PIDK < %s), 1) < 1 )"\
                    %(self.config['KPIDK_MIN'])
        childPIDpi = "(NINGENERATION( ('pi+'==ABSID) & (PIDK > %s), 1) < 1 )"\
                     %(self.config['piPIDK_MAX'])
        dCuts.append(childPIDK )
        dCuts.append(childPIDpi )
        dCuts.append(childPIDp )        # Create Tight D+- Mesons
        dCuts.append(self._massWindow(tag))
        dCuts = LoKiCuts.combine(dCuts)
        return [filterSelection(name,dCuts,input)]
        
    def _massWindow(self,which):
        return "(M > %s) & (M < %s)" % (self.config['MM_MIN'][which],
                                        self.config['MM_MAX'][which])

    def _makeBiased(self,name,inputs):
        return [filterSelection(name+'Biased','(MIPCHI2DV(PRIMARY) > 4)',
                                inputs)]
    
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

