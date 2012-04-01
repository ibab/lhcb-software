#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class HHHBuilder(object):
    '''Produces all HHH quasi-particles for the Beauty2Charm module.'''

    def __init__(self,pions,kaons,protons,config):
        self.pions = filterInputs("HHHPions",[pions],config['PiDAUGHTERS'])
        self.kaons = filterInputs("HHHKaons",[kaons],config['KDAUGHTERS'])
        self.protons = filterInputs("HHHProtons",[protons],
                                    config['pDAUGHTERS'])
        self.config = config
        self.pipipi = [self._makePiPiPi()]
        self.kkpi = [self._makeKKPi()]
        self.kpipi = [self._makeKPiPi()]
        self.ppbarpi = [self._makeppbarPi()]
        self.ppbark = [self._makeppbarK()]
        
    def _makeX2HHH(self,name,decays,amass,config,inputs):
        ''' Makes all X -> HHH selections with charged tracks only.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        numPassPtCut = "(ANUM(PT < %s) <= 1)" %(config['PTMIN1'])
        comboCuts.append(numPassPtCut)
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA','MIPCHI2DV',
                            'BPVVDRHO','BPVVDZ'],config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=inputs)

    def _makePiPiPi(self):
        '''Makes X -> pi+pi-pi+'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['A1'])
        return self._makeX2HHH('X2PiPiPi',['[a_1(1260)+ -> pi+ pi- pi+]cc'],
                              massWindow,self.config,[self.pions])

    def _makeKKPi(self):
        '''Makes X -> K+K-pi+'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['A1'])
        return self._makeX2HHH('X2KKPi',['[a_1(1260)+ -> K+ K- pi+]cc'],
                              massWindow,self.config,[self.pions,self.kaons])

    def _makeKPiPi(self):
        '''Makes X -> K+pi-pi+ + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['K1'])
        return self._makeX2HHH('X2KPiPi',['[K_1(1270)+ -> K+ pi- pi+]cc'],
                              massWindow,self.config,
                              [self.pions,self.kaons])

    def _makeppbarPi(self):
        '''Makes X -> p pbar-pi+ + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PPH'])
        return self._makeX2HHH('X2ppbarPi',['[a_1(1260)+ -> p+ p~- pi+]cc'],
                              massWindow,self.config,
                              [self.pions,self.protons])

    def _makeppbarK(self):
        '''Makes X -> p pbar-K+ + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PPH'])
        return self._makeX2HHH('X2ppbarK',['[a_1(1260)+ -> p+ p~- K+]cc'],
                              massWindow,self.config,
                              [self.kaons,self.protons])
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
