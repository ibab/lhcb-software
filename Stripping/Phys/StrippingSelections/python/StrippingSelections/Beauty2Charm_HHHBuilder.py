#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Beauty2Charm_ComboEngine import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class HHHBuilder(object):
    '''Produces all HHH quasi-particles for the Beauty2Charm module.'''

    def __init__(self,pions,kaons,protons,config):
        self.pions = filterInputs("HHHPions",[pions],config['PiDAUGHTERS'])
        self.kaons = filterInputs("HHHKaons",[kaons],config['KDAUGHTERS'])
        self.protons = filterInputs("HHHProtons",[protons],config['pDAUGHTERS'])
        self.config = config
        self.pipipi = self._makePiPiPi()
        self.kpipi = self._makeKPiPi()
        self.ppbarpi = self._makeppbarPi()
        self.ppbark = self._makeppbarK()

    def _makeX2HHH(self,name,decays,amass,config,inputs):
        ''' Makes all X -> HHH selections with charged tracks only.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        # require at least one daughter to IP chi2 > D min FD chi2
        #comboCuts.append("AHASCHILD(MIPCHI2DV(PRIMARY) > %s)" \
        #                 % config['BPVVDCHI2_MIN'])
        comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        print 'makeX2HHH',name,'combo:',comboCuts,'mom:',momCuts
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=inputs)



    def _makePiPiPi(self):
        '''Makes X -> pi+pi-pi+'''
        return self._makeX2HHH('X2PiPiPi',['[a_1(1260)+ -> pi+ pi- pi+]cc'],
                              '(AM < 3*GeV)',self.config,[self.pions])

    def _makeKPiPi(self):
        '''Makes X -> K+pi-pi+ + c.c.'''
        return self._makeX2HHH('X2KPiPi',['[K_1(1270)+ -> K+ pi- pi+]cc'],
                              '(AM < 3*GeV)',self.config,
                              [self.pions,self.kaons])

    def _makeppbarPi(self):
        '''Makes X -> p pbar-pi+ + c.c.'''
        return self._makeX2HHH('X2ppbarPi',['[Xi_c+ -> p+ p~- pi+]cc'],
                              '(AM < 5*GeV)',self.config,
                              [self.pions,self.protons])

    def _makeppbarK(self):
        '''Makes X -> p pbar-K+ + c.c.'''
        return self._makeX2HHH('X2ppbarK',['[Xi_c+ -> p+ p~- K+]cc'],
                              '(AM < 5*GeV)',self.config,
                              [self.kaons,self.protons])

        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
