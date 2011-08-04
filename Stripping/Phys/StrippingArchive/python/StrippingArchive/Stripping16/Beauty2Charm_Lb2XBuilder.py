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

    def __init__(self,pions,kaons,protons,config):
        self.pions = pions
        self.kaons = kaons
        self.protons = protons
        self.config = config
        self.pkpi = [self._makeLc2pKpi()]
        
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

    def __init__(self,lc,d,hh,topoPions,topoKaons,df,hhh,config):
        self.lc = lc.pkpi
        self.d0 = d.hh
        self.hh = hh
        self.hhh = hhh
        self.flc = df.flc
        self.topoPions = [topoPions]
        self.topoKaons = [topoKaons]
        self.config = deepcopy(config)
        self.config['AM_MIN'] = '5200*MeV'
        self.lines = []
        # Lb -> Lc+- H-+ (+WS)
        self._makeLb2LcH()
        # Lb -> D0(HH) p+- H-+
        self._makeLb2D0PH()
        # Lb -> Lc+- 3Pi, KPiPi, ppbaPi, ppbarK (+WS)
        self._makeLb2LcHHH()

    def _makeLb2LcH(self):
        '''Make RS and WS Lb -> Lc H (H=pi,K) + cc.'''
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'Lb2LcPi': ["[Lambda_b0 -> Lambda_c+ pi-]cc"],
                  'Lb2LcK' : ["[Lambda_b0 -> Lambda_c+ K-]cc"]}
        inputs = {'Lb2LcPi': self.lc+pions, 'Lb2LcK': self.lc+kaons}
        rs = makeB2XMerged('Lb2LcH',decays,'Lc2PKPi',inputs,self.config)
        decays = {'Lb2LcPiWS': ["[Lambda_b0 -> Lambda_c+ pi+]cc"],
                  'Lb2LcKWS' : ["[Lambda_b0 -> Lambda_c+ K+]cc"]}
        inputs = {'Lb2LcPiWS':self.lc+pions, 'Lb2LcKWS':self.lc+kaons}
        ws = makeB2XMerged('Lb2LcHWS',decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs['TOS'],1.0))
        self.lines.append(ProtoLine(rs['TIS'],1.0))
        self.lines.append(ProtoLine(ws['TOS'],0.1))

    def _makeLb2LcHHH(self):
        '''Make RS and WS Lb -> Lc HHH (H=pi,K) + cc.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        ppbarpi = self.hhh.ppbarpi
        ppbark = self.hhh.ppbark
        decays = {'Lb2LcPiPiPi'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcKPiPi'   : ["[Lambda_b0 -> Lambda_c+ K_1(1270)-]cc"],
                  'Lb2LcppbarPi' : ["[Lambda_b0 -> Lambda_c+ Xi_c~-]cc"],
                  'Lb2LcppbarK'  : ["[Lambda_b0 -> Lambda_c+ Xi_c~-]cc"]}
        inputs = {'Lb2LcPiPiPi'  : self.flc+pipipi,
                  'Lb2LcKPiPi'   : self.flc+kpipi,
                  'Lb2LcppbarPi' : self.flc+ppbarpi,
                  'Lb2LcppbarK'  : self.flc+ppbark}
        rs = makeB2XMerged('Lb2LcHHH',decays,'Lc2PKPi',inputs,self.config)
        decays = {'Lb2LcPiPiPiWS'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcKPiPiWS'   : ["[Lambda_b0 -> Lambda_c+ K_1(1270)+]cc"],
                  'Lb2LcppbarPiWS' : ["[Lambda_b0 -> Lambda_c+ Xi_c+]cc"],
                  'Lb2LcppbarKWS'  : ["[Lambda_b0 -> Lambda_c+ Xi_c+]cc"]}
        inputs = {'Lb2LcPiPiPiWS'  : self.flc+pipipi,
                  'Lb2LcKPiPiWS'   : self.flc+kpipi,
                  'Lb2LcppbarPiWS' : self.flc+ppbarpi,
                  'Lb2LcppbarKWS'  : self.flc+ppbark}
        ws = makeB2XMerged('Lb2LcHHHWS',decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs['TOS'],1.0))
        self.lines.append(ProtoLine(rs['TIS'],1.0))
        self.lines.append(ProtoLine(ws['TOS'],0.1))

    def _makeLb2D0PH(self):
        '''Makes RS Lb -> D0(HH) p+- H-+ + c.c.'''
        decs = ["Lambda_b0 -> D0 Lambda0","Lambda_b0 -> D0 Lambda~0"]
        decays = {'Lb2D0PH': decs}
        inputs = {'Lb2D0PH': self.d0+self.hh.ph}
        rs = makeB2XMerged('Lb2D0PH',decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(rs['TOS'],1.0))
        self.lines.append(ProtoLine(rs['TIS'],1.0))

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#


