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
        self.kstar0 = self._makeKstar0(self.kpi)
        self.ph = self._makePH()
        self.phi = self._makePhi(self.kk)
        self.rho0 = self._makeRho0(self.pipi)
        self.rhopm = self._makeRhoPM(self.pipi0)
        self.kstarpm = self._makeKstarPM(self.kpi0)
        self.ppbar = self._makePPbar()
        # WS selections (ie doubly-charged ones)
        self.hh_ws = self._makeHHWS()
        self.ph_ws = self._makePHWS()
        self.ppbar_ws = self._makePPbarWS()
        # PID filtered selections
        self.pipi_pid = [filterPID('X2PiPiPID',self.pipi,config_pid)]
        self.kpi_pid = [filterPID('X2KPiPID',self.kpi,config_pid)]

        #Different PID filterings for testing with the FULL DST lines
        self.pipi_pid_tightpi = [filterPID('X2PiPiPIDTIGHTPI',self.pipi,config_pid['TIGHTPI'])]
        #tighter on the kaon
        self.kpi_pid_tighter = [filterPID('X2KPiPIDTIGHTER',self.kpi,config_pid['TIGHTER'])]
        
        self.kk_pid = [filterPID('X2KKPID',self.kk,config_pid)]
        self.hh_ws_pid = [filterPID('X2HHWSPID',self.hh_ws,config_pid)]
        self.ph_pid = [filterPID('X2PHPID',self.ph,config_pid)]
        self.ppbar_pid = [filterPID('X2PPbarPID',self.ppbar,config_pid['TIGHT'])]
        self.ppbar_ws_pid = [filterPID('X2PPbarWSPID',self.ppbar_ws,config_pid['TIGHT'])]
        self.ppbar_pid_tighter = [filterPID('X2PPbarPIDTIGHTER',self.ppbar,config_pid['TIGHTER'])]
        self.ppbar_ws_pid_tighter = [filterPID('X2PPbarWSPIDTIGHTER',self.ppbar_ws,config_pid['TIGHTER'])]

        self.ppbar_Mcut = [filterSelection('X2PPbarMCUT','( M < 3.5*GeV )',self.ppbar)]
        self.ppbar_Mcut_pid_tighter2 = [filterPID('X2PPbarMCUTPIDTIGHTER2',self.ppbar_Mcut,config_pid['TIGHTER2'])]

        self.rho0_pid = [filterPID('RHO02PiPiPID',self.rho0,config_pid)]
        self.kstar0_pid = [filterPID('Kstar02KPiPID',self.kstar0,config_pid)]
        self.phi_pid = [filterPID('PHI2KKPID',self.phi,config_pid)]

    def _makeX2HH(self,name,decays,amass,config,inputs):
        ''' Makes all X -> HH selections with charged tracks only.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        comboCuts.append(LoKiCuts(['ADOCA12'],config).code())
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
            #cp.ParticleCombiners.update({'':'MomentumCombiner'})
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=inputs)

    def _massWindow(self,which,name):
        return "ADAMASS('%s') < %s" % (name,self.config['MASS_WINDOW'][which])

    def _makePiPi(self):
        '''Makes X -> pi+pi-'''
        return [self._makeX2HH('X2PiPi',['rho(770)0 -> pi+ pi-'],
                              '(AM < 5.2*GeV)',self.config,[self.pions])]

    def _makePiPiWSSels(self):
        '''Makes X -> pi+pi+ + c.c.'''
        plus = self._makeX2HH('X2PiPiWSPlus',['rho(770)+ -> pi+ pi+'],
                              '(AM < 5.2*GeV)',self.config,[self.pions])
        minus = self._makeX2HH('X2PiPiWSMinus',['rho(770)- -> pi- pi-'],
                               '(AM < 5.2*GeV)',self.config,[self.pions])
        return [plus,minus]

    def _makePiPiWS(self):
        sels = self._makePiPiWSSels()
        return [MergedSelection('X2PiPiWSBeauty2Charm',
                                RequiredSelections=sels)]

    def _makeKPi(self,pipi):
        '''Makes X -> K+pi- + c.c.'''
        sel1 = subPID('X2KPi','K*(892)0',['K+','pi-'],pipi)
        sel2 = subPID('X2KPiBar','K*(892)~0',['pi+','K-'],pipi)
        return [MergedSelection('X2KPiBeauty2Charm',
                                RequiredSelections=[sel1,sel2])]
   
    def _makeKK(self,pipi):
        '''Makes X -> K+K-.'''
        return [subPID('X2KK','phi(1020)',['K+','K-'],pipi)]

    def _makeHHWS(self):
        from Beauty2Charm_DBuilder import subPIDSels
        protoSels = self._makePiPiWSSels()
        decays = [['pi+','pi+'],['pi+','K+'],['K+','pi+'],['K+','K+']]
        plus = subPIDSels(decays,'X2HHWSPlus','','0*MeV','5200*MeV',
                          [protoSels[0]])
        decays = [['pi-','pi-'],['pi-','K-'],['K-','pi-'],['K-','K-']]
        minus = subPIDSels(decays,'X2HHWSMinus','','0*MeV','5200*MeV',
                           [protoSels[1]])
        return [MergedSelection('X2HHWSBeauty2Charm',
                                RequiredSelections=[plus,minus])]
        
    def _makeKsPi(self):
        '''Makes X -> Ks0pi- + c.c.'''
        dd = self._makeXPLUS2HH('X2KsPiDD',['[K*(892)+ -> KS0 pi+]cc'],
                                '(AM < 5.2*GeV)',self.config,
                                self.ks["DD"]+[self.pions])
        ll = self._makeXPLUS2HH('X2KsPiLL',['[K*(892)+ -> KS0 pi+]cc'],
                                '(AM < 5.2*GeV)',self.config,
                                self.ks["LL"]+[self.pions])
        return [MergedSelection('X2KsPiBeauty2Charm',
                                RequiredSelections=[dd,ll])]

    def _makeKPi0(self):
        '''Makes X -> K+pi0 + c.c.'''
        m = self._makeXPLUS2HH('X2KPi0Merged',['[K*(892)+ -> K+ pi0]cc'],
                               '(AM < 5*GeV)',self.config,
                               self.pi0["Merged"]+[self.kaons],True)
        r = self._makeXPLUS2HH('X2KPi0Resolved',['[K*(892)+ -> K+ pi0]cc'],
                               '(AM < 5*GeV)',self.config,
                               self.pi0["Resolved"]+[self.kaons],True)
        return [MergedSelection('X2KPi0Beauty2Charm',RequiredSelections=[m,r])]

    def _makePiPi0(self):
        '''Makes X -> pi+pi0'''        
        m = self._makeXPLUS2HH('X2PiPi0Merged',['[rho(770)+ -> pi+ pi0]cc'],
                               'AM < 5*GeV',self.config,
                               [self.pions]+self.pi0["Merged"],True)
        r = self._makeXPLUS2HH('X2PiPi0Resolved',['[rho(770)+ -> pi+ pi0]cc'],
                               'AM < 5*GeV',self.config,
                               [self.pions]+self.pi0["Resolved"],True)
        return [MergedSelection('X2PiPi0Beauty2Charm',
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

    def _makeRhoPM(self,pipi):
        mass = self._massWindow('RHO','rho(770)+').replace('ADAMASS','ADMASS')
        return [filterSelection('RHOPM',mass,pipi)]

    def _makeKstarPM(self,kpi):
        mass = self._massWindow('KST','K*(892)+').replace('ADAMASS','ADMASS')
        return [filterSelection('K*PM',mass,kpi)]
                                
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

    def _makePHWS(self):
        '''Makes X -> p+ h+ + c.c.'''
        psel = self._makeX2HH('X2PPiPlusWS',['Lambda0 -> pi+ pi+'],
                             '(AM < 5*GeV)',
                             self.config,[self.pions])
        decays = [['p+','pi+'],['pi+','p+'],['p+','K+'],['K+','p+']]
        pfilter = SubPIDMMFilter('X2PHPlusWSSubPIDBeauty2Charm',Code='ALL',
                                 MinMM=0,MaxMM=5000,PIDs=decays)
        psel = Selection('X2PHPlusWSSubPIDSelBeauty2Charm',Algorithm=pfilter,
                         RequiredSelections=[psel])
        filter="INTREE((ABSID=='p+') & (P > %s)) & (M < 5.2*GeV)" \
                % self.config['pP_MIN']
        psel = filterSelection('X2PHPlusWS',filter,[psel])
        msel = self._makeX2HH('X2PPiMinusWS',['Lambda0 -> pi- pi-'],
                              '(AM < 5*GeV)',
                              self.config,[self.pions])
        decays = [['p~-','pi-'],['pi-','p~-'],['p~-','K-'],['K-','p~-']]
        mfilter = SubPIDMMFilter('X2PHMinusWSSubPIDBeauty2Charm',Code='ALL',
                                 MinMM=0,MaxMM=5000,PIDs=decays)
        msel = Selection('X2PHMinusWSSubPIDSelBeauty2Charm',Algorithm=mfilter,
                         RequiredSelections=[msel])
        msel = filterSelection('X2PHMinusWS',filter,[msel])
        return [psel,msel]                                        

    def _makePPbar(self):
        '''Makes X -> p+ pbar-'''
        sel = self._makeX2HH('X2PPbar', ['rho(770)0 -> pi+ pi-'],
                             '(AM < 5*GeV)',
                             self.config,[self.pions])
        decays = [['p+','p~-']]
        filter = SubPIDMMFilter('X2PPbarSubPIDBeauty2Charm',Code='ALL',
                                MinMM=0,MaxMM=5000,PIDs=decays)
        presel = Selection('X2PPbarSubPIDSelBeauty2Charm',Algorithm=filter,
                           RequiredSelections=[sel])
        filter="INTREE((ABSID=='p+') & (P > %s)) & (M < 5.2*GeV)" \
                % self.config['pP_MIN']
        return [filterSelection('X2PPbar',filter,[presel])]

    def _makePPbarWS(self):
        '''Makes X -> p+ p+ + c.c.'''
        psel = self._makeX2HH('X2PPbarPlusWS',['rho(770)+ -> pi+ pi+'],
                             '(AM < 5*GeV)',
                             self.config,[self.pions])
        decays = [['p+','p+']]
        pfilter = SubPIDMMFilter('X2PPbarPlusWSSubPIDBeauty2Charm',Code='ALL',
                                 MinMM=0,MaxMM=5000,PIDs=decays)
        psel = Selection('X2PPbarPlusWSSubPIDSelBeauty2Charm',Algorithm=pfilter,
                         RequiredSelections=[psel])
        filter="INTREE((ABSID=='p+') & (P > %s)) & (M < 5.2*GeV)" \
                % self.config['pP_MIN']
        psel = filterSelection('X2PPbarPlusWS',filter,[psel])
        msel = self._makeX2HH('X2PPbarMinusWS',['rho(770)- -> pi- pi-'],
                              '(AM < 5*GeV)',
                              self.config,[self.pions])
        decays = [ ['p~-','p~-'] ]
        mfilter = SubPIDMMFilter('X2PPbarMinusWSSubPIDBeauty2Charm',Code='ALL',
                                 MinMM=0,MaxMM=5000,PIDs=decays)
        msel = Selection('X2PPbarMinusWSSubPIDSelBeauty2Charm',Algorithm=mfilter,
                         RequiredSelections=[msel])
        msel = filterSelection('X2PPbarMinusWS',filter,[msel])
        return [psel,msel]                                        

                                                            
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
