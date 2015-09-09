#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables import DaVinci__N3BodyDecays
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class HHHBuilder(object):
    '''Produces all HHH quasi-particles for the Beauty2Charm module.'''

    def __init__(self,pions,kaons,protons,config, config_pid):
        self.pions = filterInputs("HHHPions",[pions],config['PiDAUGHTERS'])
        self.kaons = filterInputs("HHHKaons",[kaons],config['KDAUGHTERS'])
        self.protons = filterInputs("HHHProtons",[protons],
                                    config['pDAUGHTERS'])
        self.config = config
        self.pipipi = [self._makePiPiPi()]

        #Different PID filtering for testing the FULL DST lines
        self.pipipi_tighterpi = [filterPID('X2PiPiPiPIDTIGHTERPI',self.pipipi,config['PID']['TIGHTERPI'])]
        
        self.kkpi = [self._makeKKPi()]
        self.kpipi = [self._makeKPiPi()]
        self.kpipiSS = [self._makeKPiPiSS()]

        #Really tight on the kaon (for FULL DST)
        self.kpipi_realtightk = [filterPID('X2KPiPiPIDREALTIGHTK',self.kpipi,config['PID']['REALTIGHTK'])]
        
        self.ppbarpi = [self._makeppbarPi()]
        self.ppbark = [self._makeppbarK()]
        self.pppbar = [self._makepppbar()]

        self.ppipi = [self._makepPiPi()]
        self.pkpi  = [self._makepKPi()]
        self.pkk   = [self._makepKK()]
        
        self.ppipiOS = [self._makepPiPiOS()]
        self.pkpiOS  = [self._makepKPiOS()]
        self.pkkOS   = [self._makepKKOS()]
        
        self.ppipi_pid = [filterPID('X2pPiPiTIGHTPID',self.ppipi,config_pid['TIGHTER'])]
        self.pkpi_pid =  [filterPID('X2pKPiTIGHTPID', self.pkpi,config_pid['TIGHTER'])]
        self.pkk_pid =   [filterPID('X2pKKTIGHTPID',  self.pkk,config_pid['TIGHTER'])]
        
        self.ppipi_pid_OS = [filterPID('X2pPiPiOSTIGHTPID',self.ppipiOS,config_pid['TIGHTER'])]
        self.pkpi_pid_OS =  [filterPID('X2pKPiOSTIGHTPID', self.pkpiOS,config_pid['TIGHTER'])]
        self.pkk_pid_OS =   [filterPID('X2pKKOSTIGHTPID',  self.pkkOS,config_pid['TIGHTER'])]
        
    def _makeX2HHH(self,name,decays,amass,config,inputs):
        ''' Makes all X -> HHH selections with charged tracks only.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        comboCuts12 = [LoKiCuts(['ADOCA12'],config).code()]
        comboCuts.append(LoKiCuts(['ADOCA13'],config).code())
        comboCuts.append(LoKiCuts(['ADOCA23'],config).code())
        numPassPtCut = "(ANUM(PT < %s) <= 1)" %(config['PTMIN1'])
        comboCuts.append(numPassPtCut)
        comboCuts = LoKiCuts.combine(comboCuts)
        comboCuts12 = LoKiCuts.combine(comboCuts12)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA','MIPCHI2DV',
                            'BPVVDRHO','BPVVDZ'],config).code()
        cp = DaVinci__N3BodyDecays(Combination12Cut=comboCuts12,CombinationCut=comboCuts,MotherCut=momCuts,
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

    def _makeKPiPiSS(self):
        '''Makes X -> K-pi+pi+ + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['K1'])
        return self._makeX2HHH('X2KPiPiSS',['[K_1(1270)+ -> K- pi+ pi+]cc'],
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

    def _makepppbar(self):
        '''Makes X -> p p pbar- + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PPH'])
        return self._makeX2HHH('X2pppbar',['[a_1(1260)+ -> p+ p+ p~-]cc'],
                              massWindow,self.config,
                              [self.protons])

    def _makepKPi(self):
        '''Makes X- -> p K-pi- + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PHH'])
        return self._makeX2HHH('X2pKPi',['[a_1(1260)- -> p+ K- pi-]cc'],
                              massWindow,self.config,
                              [self.pions,self.kaons,self.protons])

    def _makepKK(self):
        '''Makes X- -> p K-pi- + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PHH'])
        return self._makeX2HHH('X2pKK',['[a_1(1260)- -> p+ K- K-]cc'],
                              massWindow,self.config,
                              [self.kaons,self.protons])

    def _makepPiPi(self):
        '''Makes X- -> p pi-pi- + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PHH'])
        return self._makeX2HHH('X2pPiPi',['[a_1(1260)- -> p+ pi- pi-]cc'],
                              massWindow,self.config,
                              [self.pions,self.protons])

    def _makepKPiOS(self):
        '''Makes X+ -> p K+pi- + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PHH'])
        return self._makeX2HHH('X2pKPiOS',['[a_1(1260)+ -> p+ K+ pi-]cc'],
                              massWindow,self.config,
                              [self.pions,self.kaons,self.protons])

    def _makepKKOS(self):
        '''Makes X- -> p K+K- + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PHH'])
        return self._makeX2HHH('X2pKKOS',['[a_1(1260)+ -> p+ K+ K-]cc'],
                              massWindow,self.config,
                              [self.kaons,self.protons])

    def _makepPiPiOS(self):
        '''Makes X- -> p pi+pi- + c.c.'''
        massWindow = "(AM < %s)" % (self.config['MASS_WINDOW']['PHH'])
        return self._makeX2HHH('X2pPiPiOS',['[a_1(1260)+ -> p+ pi+ pi-]cc'],
                              massWindow,self.config,
                              [self.pions,self.protons])

        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
