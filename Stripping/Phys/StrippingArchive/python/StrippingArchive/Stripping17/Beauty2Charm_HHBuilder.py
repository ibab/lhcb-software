#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Configurables import SubstitutePID
from Configurables import SubPIDMMFilter

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def subPID(name,p,d,inputs):
    sub = SubstitutePID(name+'SubPIDBeauty2Charm',
                        Code="DECTREE('X0 -> X+ X-')")
    sub.MaxChi2PerDoF = -666
    sub.Substitutions = {'X0 -> ^X+ X-' : d[0],'X0 -> X+ ^X-' : d[1],
                         'X0 -> X+ X-'  : p}
    sel = Selection(name+'Beauty2CharmSel',Algorithm=sub,
                    RequiredSelections=inputs)
    filter = "INTREE(ID=='%s') & INTREE(ID=='%s') & INTREE(ID=='%s')" \
             % (p,d[0],d[1])
    return filterSelection(name,filter,[sel])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class HHBuilder(object):
    '''Produces all HH quasi-particles for the Beauty2Charm module.'''

    def __init__(self,pions,kaons,protons,ks,pi0,config,config_pid):
        self.pions = filterInputs("HHPions",[pions],config['DAUGHTERS'])
        self.kaons = filterInputs("HHKaons",[kaons],config['DAUGHTERS'])
        self.protons = filterInputs("HHProtons",[protons],config['DAUGHTERS'])
        self.ks = ks
        self.pi0 = pi0
        self.config = config
        self.pipi = self._makePiPi()
        self.kspi = self._makeKsPi()
        self.kpi0 = self._makeKPi0()
        self.pipi0 = self._makePiPi0()
        self.kpi = self._makeKPi(self.pipi)
        self.kk = self._makeKK(self.pipi)
        self.rhoplus = self._makeRhoPlus()
        self.kstplus_kspi = self._makeKstarPlus_KSpi()
        self.kstplus_kpi0 = self._makeKstarPlus_KPi0()
        self.kstar0 = self._makeKstar0(self.kpi)
        self.ph = self._makePH()
        self.phi = self._makePhi(self.kk)
        self.rho0 = self._makeRho0(self.pipi)
        # WS selections (ie doubly-charged ones)
        self.hh_ws = self._makeHHWS()
        # PID filtered selections
        self.pipi_pid = [filterPID('X2PiPiPID',self.pipi,config_pid)]
        self.kpi_pid = [filterPID('X2KPiPID',self.kpi,config_pid)]
        self.kk_pid = [filterPID('X2KKPID',self.kk,config_pid)]
        self.hh_ws_pid = [filterPID('X2HHWSPID',self.hh_ws,config_pid)]
        self.ph_pid = [filterPID('X2PHPID',self.ph,config_pid)]

    def _makeX2HH(self,name,decays,amass,config,inputs):
        ''' Makes all X -> HH selections with charged tracks only.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=inputs)

    def _makeXPLUS2HH(self,name,decays,amass,config,inputs,pi0=False):
        ''' Makes all X+ -> HH selections involving neutrals.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = "ALL" 
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        if pi0:
            cp = cp.configurable(name+'Beauty2CharmCombiner')
            cp.ParticleCombiners.update({'':'MomentumCombiner'})
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=inputs)

    def _massWindow(self,which,name):
        return "ADAMASS('%s') < %s" % (name,self.config['MASS_WINDOW'][which])

    def _makePiPi(self):
        '''Makes X -> pi+pi-'''
        return [self._makeX2HH('X2PiPi',['rho(770)0 -> pi+ pi-'],
                              '(AM < 5.2*GeV)',self.config,[self.pions])]

    def _makePiPiWS(self):
        '''Makes X -> pi+pi+ + c.c.'''
        plus = self._makeX2HH('X2PiPiWSPlus',['rho(770)+ -> pi+ pi+'],
                              '(AM < 5.2*GeV)',self.config,[self.pions])
        minus = self._makeX2HH('X2PiPiWSMinus',['rho(770)- -> pi- pi-'],
                               '(AM < 5.2*GeV)',self.config,[self.pions])
        return [plus,minus]

    def _makeKPi(self,pipi):
        '''Makes X -> K+pi- + c.c.'''
        sel1 = subPID('X2KPi','K*(892)0',['K+','pi-'],pipi)
        sel2 = subPID('X2KPiBar','K*(892)~0',['pi+','K-'],pipi)
        return [sel1,sel2]
   
    def _makeKK(self,pipi):
        '''Makes X -> K+K-.'''
        return [subPID('X2KK','phi(1020)',['K+','K-'],pipi)]

    def _makeHHWS(self):
        from Beauty2Charm_DBuilder import subPIDSels
        protoSels = self._makePiPiWS()
        decays = [['pi+','pi+'],['pi+','K+'],['K+','pi+'],['K+','K+']]
        plus = subPIDSels(decays,'X2HHWSPlus','','0*MeV','5200*MeV',
                          [protoSels[0]])
        decays = [['pi-','pi-'],['pi-','K-'],['K-','pi-'],['K-','K-']]
        minus = subPIDSels(decays,'X2HHWSMinus','','0*MeV','5200*MeV',
                           [protoSels[1]])
        return [plus,minus]
        
    def _makeKsPi(self):
        '''Makes X -> Ks0pi- + c.c.'''
        return [self._makeXPLUS2HH('X2KsPi',['[K*(892)+ -> KS0 pi+]cc'],
                                   '(AM < 5.2*GeV)',self.config,
                                   self.ks["DD"]+self.ks["LL"]+[self.pions])]

    def _makeKPi0(self):
        '''Makes X -> K+pi0 + c.c.'''
        inputs = self.pi0["Merged"]+self.pi0["Resolved"]+[self.kaons]
        return [self._makeXPLUS2HH('X2KPi0',['[K*(892)+ -> K+ pi0]cc'],
                                   '(AM < 5*GeV)',self.config,inputs,True)]

    def _makePiPi0(self):
        '''Makes X -> pi+pi0'''
        inputs = [self.pions] +self.pi0["Merged"]+self.pi0["Resolved"]
        return [self._makeXPLUS2HH('X2PiPi0',['[rho(770)+ -> pi+ pi0]cc'],
                                   'AM < 5*GeV',self.config,inputs,True)]

    def _makeRhoPlus(self):
        inputs = [self.pions] +self.pi0["Merged"]+self.pi0["Resolved"]
        return [self._makeXPLUS2HH('RHO2PiPi0',['[rho(770)+ -> pi+ pi0]cc'],
                                   self._massWindow('RHO','rho(770)+'),
                                   self.config,inputs,True)]

    def _makeKstarPlus_KSpi(self):
        inputs = self.ks["DD"]+self.ks["LL"]+[self.pions]
        return [self._makeXPLUS2HH('Kst2KsPi',['[K*(892)+ -> KS0 pi+]cc'],
                                   self._massWindow('KST','K*(892)+'),
                                   self.config,inputs)]

    def _makeKstarPlus_KPi0(self):
        inputs = self.pi0["Merged"]+self.pi0["Resolved"]+[self.kaons]
        return [self._makeXPLUS2HH('Kst2KPi0',['[K*(892)+ -> K+ pi0]cc'],
                                   self._massWindow('KST','K*(892)+'),
                                   self.config,inputs,True)]

    def _makeRho0(self,pipi):
        mass = self._massWindow('RHO','rho(770)0').replace('ADAMASS','ADMASS')
        return [filterSelection('RHO',mass,pipi)]

    def _makeKstar0(self,kpi):
        mass = self._massWindow('KST','K*(892)0').replace('ADAMASS','ADMASS')
        return [filterSelection('K*0',mass,kpi)]

    def _makePhi(self,kk):
        mass = self._massWindow('PHI','phi(1020)').replace('ADAMASS','ADMASS')
        return [filterSelection('PHI',mass,kk)]

    def _makePH(self):
        '''Makes X -> p+ h- + c.c.'''
        sel = self._makeX2HH('X2PPi',['Lambda0 -> pi+ pi-'],
                             '(AM < 5*GeV)',
                             self.config,[self.pions])
        decays = [['p+','pi-'],['pi+','p~-'],['p+','K-'],['K+','p~-']]
        filter = SubPIDMMFilter('X2PHSubPIDBeauty2Charm',Code='ALL',
                                MinMM=0,MaxMM=5000,PIDs=decays)
        presel = Selection('X2PHSubPIDSelBeauty2Charm',Algorithm=filter,
                           RequiredSelections=[sel])
        filter="INTREE((ABSID=='p+') & (P > %s)) & (M < 5.2*GeV)" \
                % self.config['pP_MIN']
        return [filterSelection('X2PH',filter,[presel])]
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
