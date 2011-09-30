#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class LcBuilder(object):
    '''Produces all Lambda_c baryons for the Beauty2Charm module.'''

    def __init__(self,pions,kaons,protons,config,config_pid):
        self.pions = pions
        self.kaons = kaons
        self.protons = protons
        self.config = config
        self.pkpi = [self._makeLc2pKpi()]
        self.pkpi_pid = [filterPID('Lc2pKPiPID',self.pkpi,config_pid)]
        
    def _makeLc2pKpi(self):
        '''Makes Lc -> p K pi + cc'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        comboCuts = [LoKiCuts(['ASUMPT'],self.config).code(),
                     "(ADAMASS('Lambda_c+') < %s*%s) " % (dm+10,units),
                     hasTopoChild()]
        comboCuts.append(LoKiCuts(['AMAXDOCA'],self.config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Lambda_c+') < %s*%s) " % (dm,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[Lambda_c+ -> p+ K- pi+]cc"])
        return Selection('Lc2PKPiBeauty2Charm',Algorithm=cp,
                         RequiredSelections=[self.pions,self.kaons,
                                             self.protons])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Lb2XBuilder(object):
    '''Makes all Lambda_b -> X lines.'''

    def __init__(self,lc,d,hh,topoPions,topoKaons,protons,hhh,config):
        self.lc = lc.pkpi
        self.lc_pid = lc.pkpi_pid
        self.d = d
        self.d0 = d.hh
        self.hh = hh
        self.hhh = hhh
        self.topoPions = [topoPions]
        self.topoKaons = [topoKaons]
        self.protons = [protons]
        self.config = deepcopy(config)
        self.config['AM_MIN'] = '5200*MeV'
        self.lines = []
        # Lb -> Lc+- H-+ (+WS)
        self._makeLb2LcH()
        # Sb+- -> D0(HH) p+-
        self._makeSb2D0P()
        # Sb -> D-+(HHH) p+-
        self._makeSb02DP()
        # Lb -> D0(HH) p+- H-+
        self._makeLb2D0PH()
        # Lb -> Lc+- 3Pi, KPiPi, ppbarPi, ppbarK (+WS)
        self._makeLb2LcHHH()

    def _makeLb2LcH(self):
        '''Make RS and WS Lb -> Lc H (H=pi,K) + cc.'''
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'Lb2LcPi': ["[Lambda_b0 -> Lambda_c+ pi-]cc"],
                  'Lb2LcK' : ["[Lambda_b0 -> Lambda_c+ K-]cc"]}
        inputs = {'Lb2LcPi': self.lc+pions, 'Lb2LcK': self.lc+kaons}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        decays = {'Lb2LcPiWS': ["[Lambda_b0 -> Lambda_c+ pi+]cc"],
                  'Lb2LcKWS' : ["[Lambda_b0 -> Lambda_c+ K+]cc"]}
        inputs = {'Lb2LcPiWS':self.lc+pions, 'Lb2LcKWS':self.lc+kaons}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        decays = {'Lb2LcPiNoIP': ["[Lambda_b0 -> Lambda_c+ pi-]cc"]}
        inputs = {'Lb2LcPiNoIP': self.lc_pid+pions}
        noip = makeB2XSels(decays,'Lc2PKPi',inputs,self.config,False)
        decays = {'Lb2LcPiNoIPWS': ["[Lambda_b0 -> Lambda_c+ pi+]cc"]}
        inputs = {'Lb2LcPiNoIPWS': self.lc_pid+pions}
        noip_ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config,False)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))
        self.lines.append(ProtoLine(noip,1.0))
        self.lines.append(ProtoLine(noip_ws,0.1))

    def _makeLb2LcHHH(self):
        '''Make RS and WS Lb -> Lc HHH (H=pi,K) + cc.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        ppbarpi = self.hhh.ppbarpi
        ppbark = self.hhh.ppbark
        decays = {'Lb2LcPiPiPi'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcKPiPi'   : ["[Lambda_b0 -> Lambda_c+ K_1(1270)-]cc"],
                  'Lb2LcppbarPi' : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcppbarK'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"]}
        inputs = {'Lb2LcPiPiPi'  : self.lc_pid+pipipi,
                  'Lb2LcKPiPi'   : self.lc_pid+kpipi,
                  'Lb2LcppbarPi' : self.lc_pid+ppbarpi,
                  'Lb2LcppbarK'  : self.lc_pid+ppbark}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        decays = {'Lb2LcPiPiPiWS'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcKPiPiWS'   : ["[Lambda_b0 -> Lambda_c+ K_1(1270)+]cc"],
                  'Lb2LcppbarPiWS' : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcppbarKWS'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"]}
        inputs = {'Lb2LcPiPiPiWS'  : self.lc_pid+pipipi,
                  'Lb2LcKPiPiWS'   : self.lc_pid+kpipi,
                  'Lb2LcppbarPiWS' : self.lc_pid+ppbarpi,
                  'Lb2LcppbarKWS'  : self.lc_pid+ppbark}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeLb2D0PH(self):
        '''Makes RS Lb -> D0(HH) p+- H-+ + c.c.'''
        decs = ["Lambda_b0 -> D0 Lambda0","Lambda_b0 -> D0 Lambda~0"]
        decays = {'Lb2D0PH': decs}
        inputs = {'Lb2D0PH': self.d0+self.hh.ph_pid}
        rs = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
    
    def _makeSb02DP(self):
        '''Make RS and WS Sb0 -> D+- p-+ + cc.'''
        protons = self.protons
        decays = {'Sb02DP': ["[Sigma_b0 -> D- p+]cc"]}
        inputs = {'Sb02DP': self.d.hhh_pid+protons}
        rs = makeB2XSels(decays,'D2HHHPID',inputs,self.config)
        decays = {'Sb02DPWS': ["[Sigma_b0 -> D+ p+]cc"]}
        inputs = {'Sb02DPWS': self.d.hhh_pid+protons}
        ws = makeB2XSels(decays,'D2HHHPID',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeSb2D0P(self):
        '''Make Sb+- -> D0 p+- + cc.'''
        protons = self.protons
        decays = {'Sb2D0P': ["Sigma_b+ -> D0 p+","Sigma_b- -> D0 p~-"]}
        inputs = {'Sb2D0P': self.d.hh_pid+protons}
        rs = makeB2XSels(decays,'D2HHPID',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#


