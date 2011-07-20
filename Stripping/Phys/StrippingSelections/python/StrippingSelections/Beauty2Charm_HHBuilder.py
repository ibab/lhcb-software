#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Beauty2Charm_ComboEngine import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class HHBuilder(object):
    '''Produces all HH quasi-particles for the Beauty2Charm module.'''

    def __init__(self,pions,kaons,protons,ks,pi0,config):
        self.pions = filterInputs("HHPions",[pions],config['DAUGHTERS'])
        self.kaons = filterInputs("HHKaons",[kaons],config['DAUGHTERS'])
        self.protons = filterInputs("HHProtons",[protons],config['DAUGHTERS'])
        self.ks    = ks
        self.pi0   = pi0
        self.config = config
        self.pipi = self._makePiPi()
        self.kspi = self._makeKsPi()
        self.kpi0 = self._makeKPi0()
        self.pipi0 = self._makePiPi0()
        self.kpi = self._makeKPi()
        self.kk = self._makeKK()
        self.rho0 = self._makeRho0([self.pipi])
        self.rhoplus = self._makeRhoPlus([self.pipi0])
        self.kstplus_kspi = self._makeKstarPlus_KSpi([self.kspi])
        self.kstplus_kpi0 = self._makeKstarPlus_KPi0([self.kpi0])
        self.kstar0 = self._makeKstar0([self.kpi])
        self.phi = self._makePhi([self.kk])
        self.ppi = self._makePPi()
        self.pk = self._makePK()

    def _makeX2HH(self,name,decays,amass,config,inputs):
        ''' Makes all X -> HH selections with charged tracks only.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        # require at least one daughter to IP chi2 > D min FD chi2
        #comboCuts.append("AHASCHILD(MIPCHI2DV(PRIMARY) > %s)" \
        #                 % config['BPVVDCHI2_MIN'])
        comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        print 'makeX2HH',name,'combo:',comboCuts,'mom:',momCuts
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=inputs)

    def _makeXPLUS2HH(self,name,decays,amass,config,inputs):
        ''' Makes all X+ -> HH selections involving neutrals.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        # require at least one daughter to IP chi2 > D min FD chi2
        #comboCuts.append("AHASCHILD(MIPCHI2DV(PRIMARY) > %s)" \
        #                 % config['BPVVDCHI2_MIN'])
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = "ALL" #LoKiCuts(['BPVDIRA'],config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        print 'makeXPLUS2HH',name,'combo:',comboCuts,'mom:',momCuts
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=inputs)

    def _massWindow(self,which,name):
        return "ADAMASS('%s') < %s" % (name,self.config['MASS_WINDOW'][which])

    def _makePiPi(self):
        '''Makes X -> pi+pi-'''
        return self._makeX2HH('X2PiPi',['rho(770)0 -> pi+ pi-'],
                              '(AM < 6*GeV)',self.config,[self.pions])

    def _makeKPi(self):
        '''Makes X -> K+pi- + c.c.'''
        return self._makeX2HH('X2KPi',['[K*(892)0 -> K+ pi-]cc'],
                              '(AM < 6*GeV)',self.config,
                              [self.pions,self.kaons])
    
    def _makeKK(self):
        '''Makes X -> K+K-'''
        return self._makeX2HH('X2KK',['phi(1020) -> K+ K-'],
                              '(AM < 6*GeV)',self.config,[self.kaons])

    def _makeKsPi(self):
        '''Makes X -> Ks0pi- + c.c.'''
        return self._makeXPLUS2HH('X2KsPi',['[K*(892)+ -> KS0 pi+]cc'],
                              '(AM < 6*GeV)',self.config,
                              self.ks["DD"]+self.ks["LL"]+[self.pions]) 

    def _makeKPi0(self):
        '''Makes X -> K+pi0 + c.c.'''
        inputs = self.pi0["Merged"]+self.pi0["Resolved"]+[self.kaons]
        return self._makeXPLUS2HH('X2KPi0',['[K*(892)+ -> K+ pi0]cc'],
                                  '(AM < 6*GeV)',self.config,inputs)


    def _makePiPi0(self):
        '''Makes X -> pi+pi0'''
        inputs = [self.pions] +self.pi0["Merged"]+self.pi0["Resolved"]
        return self._makeXPLUS2HH('X2PiPi0',['[rho(770)+ -> pi+ pi0]cc'],
                                  '(AM < 6*GeV)',self.config,inputs)

    def _makeRhoPlus(self,pipi0):
        return filterSelection('RHO+',self._massWindow('RHO','rho(770)+'),
                               pipi0)

    def _makeKstarPlus_KSpi(self,kspi):
        return filterSelection('KST_KSPI',self._massWindow('KST','K*(892)+'),
                               kspi)

    def _makeKstarPlus_KPi0(self,kpi0):
        return filterSelection('KST_KPI0',self._massWindow('KST','K*(892)+'),
                               kpi0)

    def _makeRho0(self,pipi):
        return filterSelection('RHO0',self._massWindow('RHO','rho(770)0'),pipi)

    def _makeKstar0(self,kpi):
        return filterSelection('K*0',self._massWindow('KST','K*(892)0'),kpi)

    def _makePhi(self,kk):
        return filterSelection('PHI',self._massWindow('PHI','phi(1020)'),kk)

    def _makePPi(self):
        '''Makes X -> p+ pi- + c.c.'''
        return self._makeX2HH('X2PPi',['[Lambda0 -> p+ pi-]cc'],'(AM < 6*GeV)',
                              self.config,[self.pions,self.protons])

    def _makePK(self):
        '''Makes X -> p+ K- + c.c.'''
        return self._makeX2HH('X2PK',['[Lambda0 -> p+ K-]cc'],'(AM < 6*GeV)',
                              self.config,[self.kaons,self.protons])
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
