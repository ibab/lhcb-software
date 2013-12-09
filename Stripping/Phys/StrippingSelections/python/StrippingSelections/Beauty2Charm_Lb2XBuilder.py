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
        self.xic_pkpi = [self._makeXic2pKpi()]
        self.xic0_pkkpi = [self._makeXic02pKKpi()]
        
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
    
    def _makeXic2pKpi(self):
        '''Makes Xic -> p K pi + cc'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        comboCuts = [LoKiCuts(['ASUMPT'],self.config).code(),
                     "(ADAMASS('Xi_c+') < %s*%s) " % (dm+10,units),
                     hasTopoChild()]
        comboCuts.append(LoKiCuts(['AMAXDOCA'],self.config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Xi_c+') < %s*%s) " % (dm,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[Xi_c+ -> p+ K- pi+]cc"])
        return Selection('Xic2PKPiBeauty2Charm',Algorithm=cp,
                         RequiredSelections=[self.pions,self.kaons,
                                             self.protons])

    def _makeXic02pKKpi(self):
        '''Makes Xic0 -> p K K pi + cc'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        comboCuts = [LoKiCuts(['ASUMPT'],self.config).code(),
                     "(ADAMASS('Xi_c0') < %s*%s) " % (dm+10,units),
                     hasTopoChild()]
        comboCuts.append(LoKiCuts(['AMAXDOCA'],self.config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Xi_c0') < %s*%s) " % (dm,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[Xi_c0 -> p+ K- K+ pi+]cc"])
        return Selection('Xic02PKKPiBeauty2Charm',Algorithm=cp,
                         RequiredSelections=[self.pions,self.kaons,
                                             self.protons])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Lb2XBuilder(object):
    '''Makes all Lambda_b -> X lines.'''

    def __init__(self,lc,d,hh,topoPions,topoKaons,protons,hhh,dst,lambda0,config):
        self.lc = lc.pkpi
        self.lc_pid = lc.pkpi_pid
        self.xic = lc.xic_pkpi
        self.xic0 = lc.xic0_pkkpi
        self.d = d
        self.d0 = d.hh
        self.hh = hh
        self.hhh = hhh
        self.dst = dst
        self.lambda0 = lambda0
        self.topoPions = [topoPions]
        self.topoKaons = [topoKaons]
        self.protons = [protons]
        self.config = deepcopy(config)
        self.config['AM_MIN'] = '5200*MeV'
        self.lines = []
        # Lb -> Lc+- H-+ (+WS)
        self._makeLb2LcH()
        # Lb -> Xic+- H-+ (+WS)
        self._makeLb2XicH()

        # Xib- -> Xic0 H- (+WS)
        self._makeXib2Xic0H()
        # Xib0 -> Xic+ D- (+WS)
        self._makeXib02XicD()
        # Xib- -> Xic0 D- (+WS)
        self._makeXib2Xic0D()
        # Xib0 -> Xic+ HHH- (+WS) (HHH = 3pi, Kpipi)
        self._makeXib02XicHHH()
        #Xib- -> Xic0 HHH- (+WS) (HHH = 3pi, Kpipi)
        self._makeXib2Xic0HHH()

        # Sb+- -> D0(HH) p+-
        self._makeSb2D0P()
        # Sb -> D-+(HHH) p+-
        self._makeSb02DP()
        # Lb -> D0(HH) p+- H-+
        self._makeLb2D0PH()
        # Lb -> Lc+- 3Pi, KPiPi, ppbarPi, ppbarK (+WS)
        self._makeLb2LcHHH()
        # Lb -> Lc D (+WS)
        self._makeLb2LcD()

        # bc baryon -> Lc D0
        self._makeX2LcD0()
        # bc baryon -> Lc D0 H (+WS)
        self._makeX2LcD0H()
        # Xib -> Lc HH (+WS)
        self._makeXib2LcHH()
        
        # Lb -> Lc D* (+WS)
        self._makeLb2LcDst()
        # X -> Lc Lc (+WS)
        self._makeX2LcLc()
        # Lb -> Lc 5pi
        self._makeLb2Lc5Pi()
        # Lb -> D0 Lambda0
        self._makeLb2D0Lambda0()

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

    def _makeLb2XicH(self):
        '''Make RS and WS Lb -> Xi_c+ H (H=pi,K) + cc.'''
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'Lb2XicPi': ["[Lambda_b0 -> Xi_c+ pi-]cc"],
                  'Lb2XicK' : ["[Lambda_b0 -> Xi_c+ K-]cc"]}
        inputs = {'Lb2XicPi': self.xic+pions, 'Lb2XicK': self.xic+kaons}
        rs = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        decays = {'Lb2XicPiWS': ["[Lambda_b0 -> Xi_c+ pi+]cc"],
                  'Lb2XicKWS' : ["[Lambda_b0 -> Xi_c+ K+]cc"]}
        inputs = {'Lb2XicPiWS':self.xic+pions, 'Lb2XicKWS':self.xic+kaons}
        ws = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeXib2Xic0H(self):
        '''Make RS and WS Xi_b- -> Xi_c0 H (H=pi,K) + cc.'''
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'Xib2Xic0Pi': ["[Xi_b- -> Xi_c0 pi-]cc"],
                  'Xib2Xic0K' : ["[Xi_b- -> Xi_c0 K-]cc"]}
        inputs = {'Xib2Xic0Pi': self.xic0+pions, 'Xib2Xic0K': self.xic0+kaons}
        rs = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config)
        decays = {'Xib2Xic0PiWS': ["[Xi_b- -> Xi_c0 pi+]cc"],
                  'Xib2Xic0KWS' : ["[Xi_b- -> Xi_c0 K+]cc"]}
        inputs = {'Xib2Xic0PiWS':self.xic0+pions, 'Xib2Xic0KWS':self.xic0+kaons}
        ws = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeXib02XicD(self):
        '''Make RS and WS Xi_b0 -> Xi_c+ D (D=D-,Ds) + cc.'''
        decays = {'Xib02XicD': ["[Xi_b0 -> Xi_c+ D-]cc"]}
        inputs = {'Xib02XicD': self.xic+self.d.hhh_cf_pid}
        rs = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        decays = {'Xib02XicDWS': ["[Xi_b0 -> Xi_c+ D+]cc"]}
        inputs = {'Xib02XicDWS':self.xic+self.d.hhh_cf_pid}
        ws = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeXib2Xic0D(self):
        '''Make RS and WS Xi_b- -> Xi_c0 D (D=D-,Ds) + cc.'''
        decays = {'Xib2Xic0D': ["[Xi_b- -> Xi_c0 D-]cc"]}
        inputs = {'Xib2Xic0D': self.xic0+self.d.hhh_cf_pid}
        rs = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config)
        decays = {'Xib2Xic0DWS': ["[Xi_b- -> Xi_c0 D+]cc"]}
        inputs = {'Xib2Xic0DWS':self.xic0+self.d.hhh_cf_pid}
        ws = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeXib02XicHHH(self):
        '''Make RS and WS Xi_b0 -> Xi_c+ HHH (HHH=3pi,Kpipi) + cc.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        decays = {'Xib02XicPiPiPi': ["[Xi_b0 -> Xi_c+ a_1(1260)-]cc"],
                  'Xib02XicKPiPi': ["[Xi_b0 -> Xi_c+ K_1(1270)-]cc"]}
        inputs = {'Xib02XicPiPiPi': self.xic+pipipi,
                  'Xib02XicKPiPi': self.xic+kpipi}
        rs = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        decays = {'Xib02XicPiPiPiWS': ["[Xi_b0 -> Xi_c+ a_1(1260)+]cc"],
                  'Xib02XicKPiPiWS': ["[Xi_b0 -> Xi_c+ K_1(1270)+]cc"]}
        inputs = {'Xib02XicPiPiPiWS': self.xic+pipipi,
                  'Xib02XicKPiPiWS': self.xic+kpipi}
        ws = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeXib2Xic0HHH(self):
        '''Make RS and WS Xi_b- -> Xi_c0 HHH- (HHH=3pi,Kpipi) + cc.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        decays = {'Xib2Xic0PiPiPi': ["[Xi_b- -> Xi_c0 a_1(1260)-]cc"],
                  'Xib2Xic0KPiPi': ["[Xi_b- -> Xi_c0 K_1(1270)-]cc"]}
        inputs = {'Xib2Xic0PiPiPi': self.xic0+pipipi,
                  'Xib2Xic0KPiPi': self.xic0+kpipi}
        rs = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config)
        decays = {'Xib2Xic0PiPiPiWS': ["[Xi_b- -> Xi_c0 a_1(1260)+]cc"],
                  'Xib2Xic0KPiPiWS': ["[Xi_b- -> Xi_c0 K_1(1270)+]cc"]}
        inputs = {'Xib2Xic0PiPiPiWS': self.xic0+pipipi,
                  'Xib2Xic0KPiPiWS': self.xic0+kpipi}
        ws = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeLb2LcHHH(self):
        '''Make RS and WS Lb -> Lc HHH (H=pi,K) + cc.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        kkpi = self.hhh.kkpi
        ppbarpi = self.hhh.ppbarpi
        ppbark = self.hhh.ppbark
        decays = {'Lb2LcPiPiPi'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcKPiPi'   : ["[Lambda_b0 -> Lambda_c+ K_1(1270)-]cc"],
                  'Lb2LcppbarPi' : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcppbarK'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcKKPi'    : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"]}
        inputs = {'Lb2LcPiPiPi'  : self.lc_pid+pipipi,
                  'Lb2LcKPiPi'   : self.lc_pid+kpipi,
                  'Lb2LcppbarPi' : self.lc_pid+ppbarpi,
                  'Lb2LcppbarK'  : self.lc_pid+ppbark,
                  'Lb2LcKKPi'    : self.lc_pid+kkpi}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        decays = {'Lb2LcPiPiPiWS'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcKPiPiWS'   : ["[Lambda_b0 -> Lambda_c+ K_1(1270)+]cc"],
                  'Lb2LcppbarPiWS' : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcppbarKWS'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcKKPiWS'    : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"]}
        inputs = {'Lb2LcPiPiPiWS'  : self.lc_pid+pipipi,
                  'Lb2LcKPiPiWS'   : self.lc_pid+kpipi,
                  'Lb2LcppbarPiWS' : self.lc_pid+ppbarpi,
                  'Lb2LcppbarKWS'  : self.lc_pid+ppbark,
                  'Lb2LcKKPiWS'    : self.lc_pid+kkpi}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeLb2D0PH(self):
        '''Makes RS Lb -> D0(HH) p+- H-+ + c.c. and WS lines'''
        decs = ["Lambda_b0 -> D0 Lambda0","Lambda_b0 -> D0 Lambda~0"]
        decays = {'Lb2D0PH': decs}
        inputs = {'Lb2D0PH': self.d0+self.hh.ph_pid}
        rs = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'Lb2D0PHWS': decs}
        inputs = {'Lb2D0PHWS': self.d0+self.hh.ph_ws}
        ws = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))
    
    def _makeLb2D0Lambda0(self):
        '''Makes RS Lb -> D0(HH) Lambda0 + c.c.'''
        decs = ["Lambda_b0 -> D0 Lambda0","Lambda_b0 -> D0 Lambda~0"]
        decays = {'Lb2D0Lambda0DD': decs}
        inputs = {'Lb2D0Lambda0DD': self.d0 + self.lambda0["DD"]}
        lb_dd = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(lb_dd,1.0))
        decays = {'Lb2D0Lambda0LL': decs}
        inputs = {'Lb2D0Lambda0LL': self.d0 + self.lambda0["LL"]}
        lb_ll = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(lb_ll,1.0))
    
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
        
    def _makeLb2LcD(self):
        '''Makes RS + WS Lb -> Lc D + c.c.'''
        decays = {'Lb2LcD': ["[Lambda_b0 -> Lambda_c+ D-]cc"]}
        inputs = {'Lb2LcD': self.d.hhh_pid+self.lc_pid}
        rs = makeB2XSels(decays,'D2HHHPID',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'Lb2LcDWS': ["[Lambda_b0 -> Lambda_c+ D+]cc"]}
        inputs = {'Lb2LcDWS': self.d.hhh_pid+self.lc_pid}
        ws = makeB2XSels(decays,'D2HHHPID',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeX2LcD0(self):
        '''Makes RS X -> Lc D0 + c.c.'''
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X2LcD0': ["[B+ -> Lambda_c+ D0]cc"]}
        inputs = {'X2LcD0': self.d.kpi+self.lc_pid}
        rs = makeB2XSels(decays,'D02KPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeX2LcD0H(self):
        '''Makes RS + WS X -> Lc D0 H + c.c.'''
        pions = self.topoPions
        kaons = self.topoKaons
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X2LcD0Pi': ["[B0 -> Lambda_c+ D0 pi-]cc"],
                  'X2LcD0K': ["[B0 -> Lambda_c+ D0 K-]cc"]}
        inputs = {'X2LcD0Pi': self.d.kpi+self.lc_pid+pions,
                  'X2LcD0K': self.d.kpi+self.lc_pid+kaons}
        rs = makeB2XSels(decays,'D02KPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'X2LcD0PiWS': ["[B0 -> Lambda_c+ D0 pi+]cc"],
                  'X2LcD0KWS': ["[B0 -> Lambda_c+ D0 K+]cc"]}
        inputs = {'X2LcD0PiWS': self.d.kpi+self.lc_pid+pions,
                  'X2LcD0KWS': self.d.kpi+self.lc_pid+kaons}
        ws = makeB2XSels(decays,'D02KPi',inputs,config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeXib2LcHH(self):
        '''Makes RS + WS Xib -> Lc HH + c.c.'''
        decays = {'Xib2LcPiPi': ["Xi_b+ -> Lambda_c+ rho(770)0","Xi_b- -> Lambda_c~- rho(770)0"],
                  'Xib2LcKPi': ["Xi_b+ -> Lambda_c+ K*(892)0","Xi_b- -> Lambda_c~- K*(892)0",
                                "Xi_b+ -> Lambda_c+ K*(892)~0","Xi_b- -> Lambda_c~- K*(892)~0"],
                  'Xib2LcKK': ["Xi_b+ -> Lambda_c+ phi(1020)","Xi_b- -> Lambda_c~- phi(1020)"]
                  }
        inputs = {'Xib2LcPiPi': self.lc_pid+self.hh.pipi,
                  'Xib2LcKPi': self.lc_pid+self.hh.kpi,
                  'Xib2LcKK': self.lc_pid+self.hh.kk}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'Xib2LcHHWS': ["[Xi_b+ -> Lambda_c+ rho(770)-]cc"]}
        inputs = {'Xib2LcHHWS': self.lc_pid+self.hh.hh_ws}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))
        
    def _makeLb2LcDst(self):
        '''Makes RS + WS Lb -> Lc D* + c.c.'''
        decays = {'Lb2LcDst': ["[Lambda_b0 -> Lambda_c+ D*(2010)-]cc"]}
        inputs = {'Lb2LcDst': self.dst.d0pi_pid+self.lc_pid}
        rs = makeB2XSels(decays,'Dstar2D0PiPID',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'Lb2LcDstWS': ["[Lambda_b0 -> Lambda_c+ D*(2010)+]cc"]}
        inputs = {'Lb2LcDstWS': self.dst.d0pi_pid+self.lc_pid}
        ws = makeB2XSels(decays,'Dstar2D0PiPID',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeX2LcLc(self):
        config = deepcopy(self.config)
        config['AM_MIN' ] = '4800*MeV'                
        decays = {'X2LcLc': ["[B0 -> Lambda_c+ Lambda_c~-]cc"]}
        inputs = {'X2LcLc': self.lc_pid}
        rs = makeB2XSels(decays,'',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'X2LcLcWS': ["[B0 -> Lambda_c+ Lambda_c+]cc"]}
        inputs = {'X2LcLcWS': self.lc_pid}
        ws = makeB2XSels(decays,'',inputs,config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeLb2Lc5Pi(self):        
        decays = {'Lb2Lc5Pi':
                  ["[Lambda_b0 -> Lambda_c+ a_1(1260)- rho(770)0]cc"]}
        inputs = {'Lb2Lc5Pi': self.lc_pid + self.hhh.pipipi + self.hh.pipi_pid}
        lb2lc5pi = makeB2XSels(decays,'Lc2PKPiPID',inputs,self.config)
        self.lines.append(ProtoLine(lb2lc5pi,1.0))
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#


