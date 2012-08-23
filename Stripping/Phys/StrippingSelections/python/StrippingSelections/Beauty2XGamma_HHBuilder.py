#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
#from Beauty2Charm_Utils import *
from Beauty2XGamma_Utils import *
from Configurables import SubstitutePID
from Configurables import SubPIDMMFilter

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def subPID(name,p,d,inputs):
    sub = SubstitutePID(name+'SubPIDBeauty2XGamma',
                        Code="DECTREE('X0 -> X+ X-')")
    sub.MaxChi2PerDoF = -666
    sub.Substitutions = {'X0 -> ^X+ X-' : d[0],'X0 -> X+ ^X-' : d[1],
                         'X0 -> X+ X-'  : p}
    sel = Selection(name+'Beauty2XGammaSel',Algorithm=sub,
                    RequiredSelections=inputs)
    filter = "INTREE(ID=='%s') & INTREE(ID=='%s') & INTREE(ID=='%s')" \
             % (p,d[0],d[1])
    return filterSelection(name,filter,[sel])

def subPIDSels(decays,prefix,suffix,min,max,inputs):
    min = float(min.split('*')[0])
    max = float(max.split('*')[0])
    filter = SubPIDMMFilter(prefix+'SubPIDBeauty2XGamma'+suffix,Code='ALL',
                            MinMM=min,MaxMM=max,PIDs=decays)
    presel = Selection(prefix+'SubPIDSelBeauty2XGamma'+suffix,
                       Algorithm=filter,
                       RequiredSelections=inputs)
    mm =  "in_range(%s,MM,%s)" % (min,max)
    return filterSelection(prefix+suffix,mm,[presel])

class HHBuilder(object):
    '''Produces all HH quasi-particles for the Beauty2xGamma module.'''

    def __init__(self,pions,kaons,protons,ks,pi0,config,config_pid):
        self.pions = filterInputs("HHPions",[pions],config['DAUGHTERS'])
        self.kaons = filterInputs("HHKaons",[kaons],config['DAUGHTERS'])
        self.protons = filterInputs("HHProtons",[protons],config['pLAMBDADAUGHTERS'])
        self.lambdaPions = filterInputs("HHLambdaPions",[self.pions],config['piLAMBDADAUGHTERS'])
        self.ks = ks
        self.pi0 = pi0
        self.config = config
        self.pipi = self._makePiPi()
        self.kspi = self._makeKsPi()
        self.kpi0 = self._makeKPi0()
        self.pipi0 = self._makePiPi0()
        self.kpi = self._makeKPi(self.pipi)
        self.kk = self._makeKK(self.pipi)
        self.kstar0 = self._makeKstar0(self.kpi)
        self.ph = self._makePH()
        self.pPi = self._makePPi()
        self.pK = self._makePK() 
        self.phi = self._makePhi(self.kk)
        self.rho0 = self._makeRho0(self.pipi)
        #self.omega = self._makeOmega()
        # WS selections (ie doubly-charged ones)
        self.hh_ws = self._makeHHWS()
        # PID filtered selections
        self.pipi_pid = [filterPID('X2PiPiPID',self.pipi,config_pid)]
        self.kpi_pid = [filterPID('X2KPiPID',self.kpi,config_pid)]
        self.kk_pid = [filterPID('X2KKPID',self.kk,config_pid)]
        self.hh_ws_pid = [filterPID('X2HHWSPID',self.hh_ws,config_pid)]
        self.ph_pid = [filterPID('X2PHPID',self.ph,config_pid)]
        self.pPi_pid = [filterPID('X2PPiPID',self.pPi,config_pid)]
        self.pK_pid = [filterPID('X2PKPID',self.pK,config_pid)]

    def _makeX2HH(self,name,decays,amass,config,inputs):
        ''' Makes all X -> HH selections with charged tracks only.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        #comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        #momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2'],config).code() 
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        return Selection(name+'Beauty2XGamma',Algorithm=cp,RequiredSelections=inputs)

    def _makeXPLUS2HH(self,name,decays,amass,config,inputs,pi0=False):
        ''' Makes all X+ -> HH selections involving neutrals.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = "ALL" 
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        if pi0:
            cp = cp.configurable(name+'Beauty2XGammaCombiner')
            cp.ParticleCombiners.update({'':'MomentumCombiner'})
        return Selection(name+'Beauty2XGamma',Algorithm=cp,
                         RequiredSelections=inputs)

    def _massWindow(self,which,name):
        return "ADAMASS('%s') < %s" % (name,self.config['MASS_WINDOW'][which])

    def _makePiPi(self):
        '''Makes X -> pi+pi-'''
        return [self._makeX2HH('X2PiPi',['rho(770)0 -> pi+ pi-'],
                              '(AM < 3.0*GeV)',self.config,[self.pions])]

        #    def _makeOmega(self):
        #''' Makes Omega -> pi+ pi- '''
        #presel = self._makeX2HH('Omega2PiPi',['omega(782) -> pi+ pi-'],'(AM < 1000*MeV)',self.config,[self.pions])
        #mass = self._massWindow('OMEGA','omega(782)').replace('ADAMASS','ADMASS')
        #return [filterSelection('Omega2PiPi',mass,[presel])]

    def _makePiPiWSSels(self):
        '''Makes X -> pi+pi+ + c.c.'''
        plus = self._makeX2HH('X2PiPiWSPlus',['rho(770)+ -> pi+ pi+'],
                              '(AM < 3.0*GeV)',self.config,[self.pions])
        minus = self._makeX2HH('X2PiPiWSMinus',['rho(770)- -> pi- pi-'],
                               '(AM < 3.0*GeV)',self.config,[self.pions])
        return [plus,minus]

    def _makePiPiWS(self):
        sels = self._makePiPiWSSels()
        return [MergedSelection('X2PiPiWSBeauty2XGamma',
                                RequiredSelections=sels)]

    def _makeKPi(self,pipi):
        '''Makes X -> K+pi- + c.c.'''
        sel1 = self._makeX2HH('X2KPi',['K*(892)0 -> pi+ pi-'],
                              '(AM < 2.5*GeV)',
                              self.config,[self.pions])
        sel2 = self._makeX2HH('X2KPiBar',['K*(892)~0 -> pi+ pi-'],
                              '(AM < 2.5*GeV)',
                              self.config,[self.pions])
        decays = [['K+','pi-'],['pi+','K-']]
        filter = SubPIDMMFilter('X2KPiSubPIDBeauty2XGamma',Code='ALL',
                 MinMM=700,MaxMM=1100,PIDs=decays)
        presel1 = Selection('X2KPiSubPIDSelBeauty2XGamma',Algorithm=filter,
                  RequiredSelections=[sel1])
        presel2 = Selection('X2KPiBarSubPIDSelBeauty2XGamma',Algorithm=filter,
                  RequiredSelections=[sel2])

        return [MergedSelection('X2KPi', RequiredSelections=[presel1,presel2])]
   
    def _makeKK(self,pipi):
        '''Makes X -> K+K-.'''
        sel = self._makeX2HH('X2KK',['phi(1020) -> pi+ pi-'],
                             '(AM < 2.5*GeV)',
                              self.config,[self.pions])
        decays = [['K+','K-']]
        filter = SubPIDMMFilter('X2KKSubPIDBeauty2XGamma',Code='ALL',
                 MinMM=1000,MaxMM=1100,PIDs=decays)
        presel = Selection('X2KKSubPIDSelBeauty2XGamma',Algorithm=filter,
                 RequiredSelections=[sel])

        return [MergedSelection('X2KK', RequiredSelections=[presel])]

    def _makeHHWS(self):
        #from Beauty2Charm_DBuilder import subPIDSels
        protoSels = self._makePiPiWSSels()
        decays = [['pi+','pi+'],['pi+','K+'],['K+','pi+'],['K+','K+']]
        plus = subPIDSels(decays,'X2HHWSPlus','','0*MeV','5000*MeV',
                          [protoSels[0]])
        decays = [['pi-','pi-'],['pi-','K-'],['K-','pi-'],['K-','K-']]
        minus = subPIDSels(decays,'X2HHWSMinus','','0*MeV','5000*MeV',
                           [protoSels[1]])
        return [MergedSelection('X2HHWSBeauty2XGamma',
                                RequiredSelections=[plus,minus])]
        
    def _makeKsPi(self):
        '''Makes X -> Ks0pi- + c.c.'''
        ## Change the mass window to 4.5 GeV to improve the timing
        sel = self._makeXPLUS2HH('X2KsPi',['[K*(892)+ -> KS0 pi+]cc'],
                                '(AM < 2.5*GeV)',self.config, self.ks+[self.pions])
        #ll = self._makeXPLUS2HH('X2KsPiLL',['[K*(892)+ -> KS0 pi+]cc'],
        #                        '(AM < 2.5*GeV)',self.config,
        #                        self.ks["DD"]+[self.pions])
        return [MergedSelection('X2KsPiAllBeauty2XGamma', RequiredSelections=[sel])]

    def _makeKPi0(self):
        '''Makes X -> K+pi0 + c.c.'''
        m = self._makeXPLUS2HH('X2KPi0Merged',['[K*(892)+ -> K+ pi0]cc'],
                               '(AM < 2.5*GeV)',self.config,
                               self.pi0["Merged"]+[self.kaons],True)
        r = self._makeXPLUS2HH('X2KPi0Resolved',['[K*(892)+ -> K+ pi0]cc'],
                               '(AM < 2.5*GeV)',self.config,
                               self.pi0["Resolved"]+[self.kaons],True)
        return [MergedSelection('X2KPi0Beauty2XGamma',RequiredSelections=[m,r])]

    def _makePiPi0(self):
        '''Makes X -> pi+pi0'''        
        m = self._makeXPLUS2HH('X2PiPi0Merged',['[rho(770)+ -> pi+ pi0]cc'],
                               'AM < 2.5*GeV',self.config,
                               [self.pions]+self.pi0["Merged"],True)
        r = self._makeXPLUS2HH('X2PiPi0Resolved',['[rho(770)+ -> pi+ pi0]cc'],
                               'AM < 2.5*GeV',self.config,
                               [self.pions]+self.pi0["Resolved"],True)
        return [MergedSelection('X2PiPi0Beauty2XGamma',
                                RequiredSelections=[m,r])]

    def _makeRho0(self,pipi):
        mass = self._massWindow('RHO','rho(770)0').replace('ADAMASS','ADMASS')
        return [filterSelection('RHO',mass,pipi)]

    def _makeKstar0(self,kpi):
        mass = self._massWindow('KST','K*(892)0').replace('ADAMASS','ADMASS')
        return [filterSelection('K*0',mass,kpi)]

    def _makePhi(self,kk):
        mass = self._massWindow('PHI','phi(1020)').replace('ADAMASS','ADMASS')
        return [filterSelection('PHI',mass,kk)]
   
    def _makePPi(self):
        ''' Makes X -> p+ pi- + c.c. '''
        sel = self._makeX2HH('X2PPi',['Lambda0 -> pi+ pi-'],
                             '(AM < 2.5*GeV)',
                             self.config,[self.lambdaPions])
        decays = [['p+','pi-'],['pi+','p~-']]
        filter = SubPIDMMFilter('X2PPiSubPIDBeauty2XGamma',Code='ALL',
                                MinMM=1000,MaxMM=2500,PIDs=decays)
        presel = Selection('X2PPiSubPIDSelBeauty2XGamma',Algorithm=filter,
                           RequiredSelections=[sel])
        filter="INTREE((ABSID=='p+') & (P > %s) & (PT > %s) & (PIDp > %s)) & (( (M < 1130*MeV) & INTREE((ABSID=='pi+')) & (PT > 1500) ))" \
              % (self.config['pLAMBDADAUGHTERS']['P_MIN'], self.config['pLAMBDADAUGHTERS']['PT_MIN'], self.config['pLAMBDADAUGHTERS']['PIDp_MIN']) 
        return [filterSelection('X2PPi',filter,[presel])]

    def _makePK(self):
        '''Makes X -> p+ K- + c.c.'''
        sel = self._makeX2HH('X2PK',['Lambda(1520)0 -> pi+ pi-'],
                             '(AM < 2.5*GeV)',
                             self.config,[self.lambdaPions])
        decays = [['p+','K-'],['K+','p~-']]
        filter = SubPIDMMFilter('X2PKSubPIDBeauty2XGamma',Code='ALL',
                                MinMM=1000,MaxMM=2500,PIDs=decays)
        presel = Selection('X2PKSubPIDSelBeauty2XGamma',Algorithm=filter,
                           RequiredSelections=[sel])
        filter="INTREE((ABSID=='p+') & (P > %s) & (PT > %s) & (PIDp > %s)) & ( ((M < 1565*MeV) & INTREE((ABSID=='K+') & (PT > 800) & (PIDK > %s)) & (PT > 2000) ) | ( (M<2500*MeV) & INTREE((ABSID=='K+') & (PT > 1000) & (PIDK > %s)) & (PT > 2250) )) & (P > 10000) " \
              % (self.config['pLAMBDADAUGHTERS']['P_MIN'], self.config['pLAMBDADAUGHTERS']['PT_MIN'], self.config['pLAMBDADAUGHTERS']['PIDp_MIN'], self.config['kLAMBDADAUGHTERS']['PIDK_MIN'], self.config['kLAMBDADAUGHTERS']['PIDK_MIN'])
        return [filterSelection('X2PK',filter,[presel])]

    def _makePH(self):
        '''Makes X -> p+ h- + c.c.'''
        sel = self._makeX2HH('X2PH',['Lambda0 -> pi+ pi-'],
                             '(AM < 2.5*GeV)',
                             self.config,[self.lambdaPions])
        decays = [['p+','pi-'],['pi+','p~-'],['p+','K-'],['K+','p~-']]
        filter = SubPIDMMFilter('X2PHSubPIDBeauty2XGamma',Code='ALL',
                                MinMM=1000,MaxMM=2500,PIDs=decays)
        presel = Selection('X2PHSubPIDSelBeauty2XGamma',Algorithm=filter,
                           RequiredSelections=[sel])
        filter="INTREE((ABSID=='p+') & (P > %s) & (PT > %s) & (PIDp > %s)) & (( (M < 1130*MeV) & INTREE((ABSID=='pi+')) & (PT > 1500) ) | ((M < 1565*MeV) & INTREE((ABSID=='K+') & (PT > 800) & (PIDK > %s)) & (PT > 2000) ) | ( (M<2500*MeV) & INTREE((ABSID=='K+') & (PT > 1000) & (PIDK > %s)) & (PT > 2250) )) & (P > 10000) " \
              % (self.config['pLAMBDADAUGHTERS']['P_MIN'], self.config['pLAMBDADAUGHTERS']['PT_MIN'], self.config['pLAMBDADAUGHTERS']['PIDp_MIN'], self.config['kLAMBDADAUGHTERS']['PIDK_MIN'], self.config['kLAMBDADAUGHTERS']['PIDK_MIN'])
        return [filterSelection('X2PH',filter,[presel])]
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\l#
