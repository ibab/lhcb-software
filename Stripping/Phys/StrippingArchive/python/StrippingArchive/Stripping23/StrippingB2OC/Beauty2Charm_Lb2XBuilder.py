#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables import DaVinci__N3BodyDecays, DaVinci__N4BodyDecays
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
        self.xic_pkpi_pid = [filterPID('Xic2pKPiPID',self.xic_pkpi,config_pid)]
        self.xic0_pkkpi = [self._makeXic02pKKpi()]
        self.xic0_pkkpi_pid = [filterPID('Xic02pKKPiPID',self.xic0_pkkpi,config_pid)]
        self.omegac0_pkkpi = [self._makeOmegac02pKKpi()]
        self.omegac0_pkkpi_pid = [filterPID('Omegac02pKKPiPID',self.omegac0_pkkpi,config_pid)]
        
    def _makeLc2pKpi(self):
        '''Makes Lc -> p K pi + cc'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        comboCuts = [LoKiCuts(['ASUMPT'],self.config).code(),
                     "(ADAMASS('Lambda_c+') < %s*%s) " % (dm+10,units),
                     hasTopoChild()]
        comboCuts12 = [LoKiCuts(['ADOCA12'],self.config).code()]
        comboCuts.append(LoKiCuts(['ADOCA13'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA23'],self.config).code())
        comboCuts12 = LoKiCuts.combine(comboCuts12)
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Lambda_c+') < %s*%s) " % (dm,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = DaVinci__N3BodyDecays(Combination12Cut=comboCuts12,CombinationCut=comboCuts,MotherCut=momCuts,
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
        comboCuts12 = [LoKiCuts(['ADOCA12'],self.config).code()]
        comboCuts.append(LoKiCuts(['ADOCA13'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA23'],self.config).code())
        comboCuts12 = LoKiCuts.combine(comboCuts12)
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Xi_c+') < %s*%s) " % (dm,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = DaVinci__N3BodyDecays(Combination12Cut=comboCuts12,CombinationCut=comboCuts,MotherCut=momCuts,
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
        comboCuts12 = [LoKiCuts(['ADOCA12'],self.config).code()]
        comboCuts123 = [LoKiCuts(['ADOCA13'],self.config).code()]
        comboCuts123.append(LoKiCuts(['ADOCA23'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA14'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA24'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA34'],self.config).code())
        comboCuts12 = LoKiCuts.combine(comboCuts12)
        comboCuts123 = LoKiCuts.combine(comboCuts123)
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Xi_c0') < %s*%s) " % (dm,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = DaVinci__N4BodyDecays(Combination12Cut=comboCuts12, Combination123Cut=comboCuts123,
                              CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[Xi_c0 -> p+ K- K- pi+]cc"])
        return Selection('Xic02PKKPiBeauty2Charm',Algorithm=cp,
                         RequiredSelections=[self.pions,self.kaons,
                                             self.protons])

    def _makeOmegac02pKKpi(self):
        '''Makes Omegac0 -> p K K pi + cc'''   
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        comboCuts = [LoKiCuts(['ASUMPT'],self.config).code(),
                     "(ADAMASS('Omega_c0') < %s*%s) " % (dm+10,units),
                     hasTopoChild()]
        comboCuts12 = [LoKiCuts(['ADOCA12'],self.config).code()] 
        comboCuts123 = [LoKiCuts(['ADOCA13'],self.config).code()]
        comboCuts123.append(LoKiCuts(['ADOCA23'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA14'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA24'],self.config).code())
        comboCuts.append(LoKiCuts(['ADOCA34'],self.config).code()) 
        comboCuts12 = LoKiCuts.combine(comboCuts12)
        comboCuts123 = LoKiCuts.combine(comboCuts123)
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Omega_c0') < %s*%s) " % (dm,units),   
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = DaVinci__N4BodyDecays(Combination12Cut=comboCuts12, Combination123Cut=comboCuts123,
                              CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[Omega_c0 -> p+ K- K- pi+]cc"])
        return Selection('Omegac02PKKPiBeauty2Charm',Algorithm=cp,  
                         RequiredSelections=[self.pions,self.kaons,
                                             self.protons])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class XiccBuilder(object):
    '''Produces Xi_cc baryons for the Beauty2Charm module.'''

    def __init__(self,lc,pions,config):
        self.lc = lc.pkpi
        self.pions = pions
        self.config = config
        self.xic = lc.xic_pkpi
        self.xic0 = lc.xic0_pkkpi
        self.xiccpp = [self._makeXiccpp2Xicpi()]
        self.xiccp = [self._makeXiccp2Xic0pi()]
        
    def _makeXiccpp2Xicpi(self):
        '''Makes Xi_cc++-> Xi_c+ pi+ + cc'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        comboCuts = [LoKiCuts(['ASUMPT'],self.config).code(),
                     "(ADAMASS('Xi_cc++') < %s*%s) " % (dm+410,units)]
        comboCuts.append(LoKiCuts(['ADOCA12'],self.config).code())  
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Xi_cc++') < %s*%s) " % (dm+400,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[Xi_cc++ -> Xi_c+ pi+]cc"])
        return Selection('Xiccpp2XicPiXic2PKPiPBeauty2Charm',Algorithm=cp,
                         RequiredSelections=self.xic+[self.pions])

    def _makeXiccp2Xic0pi(self):
        '''Makes Xi_cc+ -> Xi_c0 pi+ + cc'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        comboCuts = [LoKiCuts(['ASUMPT'],self.config).code(),
                     "(ADAMASS('Xi_cc+') < %s*%s) " % (dm+410,units)]
        comboCuts.append(LoKiCuts(['ADOCA12'],self.config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = ["(ADMASS('Xi_cc+') < %s*%s) " % (dm+400,units),
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                            self.config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[Xi_cc+ -> Xi_c0 pi+]cc"])
        return Selection('Xiccp2Xic0PiXic02PKKPiPBeauty2Charm',Algorithm=cp,
                         RequiredSelections=self.xic0+[self.pions])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class Lb2XBuilder(object):
    '''Makes all Lambda_b -> X lines.'''

    def __init__(self,lc,xicc,d,hh,topoPions,topoKaons,protons,pions,kaons,ks,hhh,dst,lambda0,config):
        self.lc = lc.pkpi
        self.lc_pid = lc.pkpi_pid
        self.xic = lc.xic_pkpi
        self.xic_pid = lc.xic_pkpi_pid
        self.xic0 = lc.xic0_pkkpi
        self.xic0_pid = lc.xic0_pkkpi_pid
        self.omegac0 = lc.omegac0_pkkpi
        self.omegac0_pid = lc.omegac0_pkkpi_pid
        self.xiccp = xicc.xiccp
        self.xiccpp = xicc.xiccpp
        self.d = d
        self.d0 = d.hh
        self.hh = hh
        self.hhh = hhh
        self.dst = dst
        self.ds = d.ds_hhh_pid_tight
        self.lambda0 = lambda0
        self.pions = [pions]
        self.kaons = [kaons]    
        self.topoPions = [topoPions]
        self.topoKaons = [topoKaons]
        self.protons = [protons]
        self.ks = ks
        self.config = deepcopy(config)
        self.config['AM_MIN'] = '5200*MeV'
        self.lines = []
        # Lb -> Lc+- H-+ (+WS)
        self._makeLb2LcH()
        # Lb -> Xic+- H-+ (+WS)
        self._makeLb2XicH()

        ### Xib- -> Xic0 H- (+WS)
        self._makeXib2Xic0H()
        ### Omegab- -> Omegac0 H- (+WS)
        self._makeOmegab2Omegac0H()
        ### Xib0 -> Xic+ D- (+WS)
        self._makeXib02XicD()
        ### Xib- -> Xic0 D- (+WS)
        self._makeXib2Xic0D()
        ### Xib0 -> Xic+ HHH- (+WS) (HHH = 3pi, Kpipi)
        self._makeXib02XicHHH()
        ###Xib- -> Xic0 HHH- (+WS) (HHH = 3pi, Kpipi)
        self._makeXib2Xic0HHH()
        ### Xb- -> Lc+ Ds- K-
        self._makeXib2LcDsK()
        ### Xb- -> Lc+ Ds- K- Pi+
        self._makeXib02LcDsKPi()
        ### B- -> Lc+ Ds- pbar
        self._makeB2LcDspbar()

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

        # Lb  -> Lc D K*
        self._makeLb2LcDKst()
        self._makeLb2LcDstKst()        

        ### bc baryon -> Lc D0
        self._makeX2LcD0()
        ### bc baryon -> Lc D0 H (+WS)
        self._makeX2LcD0H()
        ### bc baryon -> Xi_cc+ pi-
        self._makeX02XiccH()
        ### bc baryon -> Xi_cc++ pi-
        self._makeX2XiccH()
        ### Xib -> Lc H+H-, LcH-H- (+WS)
        self._makeX2LcHH()
        ### bc baryon -> Lc HHHH (+WS)
        self._makeX2LcKPiPiPi()
        ### Xib,Omega_b -> Lc HH (+WS)
        self._makeXib2XicHH()
        
        # Lb -> Lc D* (+WS)
        self._makeLb2LcDst()
        # X -> Lc Lc (+WS)
        self._makeX2LcLc()
        # Lb -> Lc 5pi
        self._makeLb2Lc5Pi()
        # Lb -> D0 Lambda0
        self._makeLb2D0Lambda0()
        
        # Lb -> D+ p h- h-
        self._makeLb2DpHH()
        # Xib -> D0 p h- h-
        self._makeXib2D0pHH()
        
        # B -> Lc pbar h-
        self._makeB2LcpbarH()
        # B-> Lc pbar
        self._makeB2Lcpbar()

        # B0->Lc pbar Ks
        self._makeB02LcpbarKS('DD')
        self._makeB02LcpbarKS('LL')        

        # Lb -> D(s)+ Lambda0 pi-
        self._makeLb2DLambda0pi()
        # Xib -> D0 Lambda0 pi-
        self._makeXib2D0Lambda0pi()
        # B0 -> Lambdac+ Lambda0bar h-
        self._makeB2LcLambda0barH()

    def _makeLb2LcH(self):
        '''Make RS and WS Lb -> Lc H (H=pi,K) + cc.'''
        pions = self.topoPions
        kaons = self.topoKaons
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
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

    def _makeB2Lcpbar(self):
        '''Make RS and WS B -> Lc pbar + cc.'''
        protons = self.protons
        config = deepcopy(self.config)
        config['AM_MIN'] = '4750.*MeV'
        decays = {'B2Lcpbar': ["[B0 -> Lambda_c+ p~-]cc"] }
        inputs = {'B2Lcpbar': self.lc+protons }
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        decays = {'B2LcpbarWS': ["[B0 -> Lambda_c+ p+]cc"]}
        inputs = {'B2LcpbarWS':self.lc+protons }
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

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
        decays = {'Lb2XicPiNoIP': ["[Lambda_b0 -> Xi_c+ pi-]cc"]}
        inputs = {'Lb2XicPiNoIP': self.xic_pid+pions}
        noip = makeB2XSels(decays,'Xic2PKPi',inputs,self.config,False)
        decays = {'Lb2XicPiNoIPWS': ["[Lambda_b0 -> Xi_c+ pi+]cc"]}
        inputs = {'Lb2XicPiNoIPWS':self.xic_pid+pions}
        noip_ws = makeB2XSels(decays,'Xic2PKPi',inputs,self.config,False)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))
        self.lines.append(ProtoLine(noip,1.0))
        self.lines.append(ProtoLine(noip_ws,0.1))

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
        decays = {'Xib2Xic0PiNoIP': ["[Xi_b- -> Xi_c0 pi-]cc"]}
        inputs = {'Xib2Xic0PiNoIP': self.xic0_pid+pions}
        noip = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config,False)                
        decays = {'Xib2Xic0PiNoIPWS': ["[Xi_b- -> Xi_c0 pi+]cc"]}
        inputs = {'Xib2Xic0PiNoIPWS':self.xic0_pid+pions}  
        noip_ws = makeB2XSels(decays,'Xic02PKKPi',inputs,self.config,False)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))
        self.lines.append(ProtoLine(noip,1.0))
        self.lines.append(ProtoLine(noip_ws,0.1))

    def _makeOmegab2Omegac0H(self):
        '''Make RS and WS Omega_b- -> Omega_c0 H (H=pi,K) + cc.''' 
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'Omegab2Omegac0Pi': ["[Omega_b- -> Omega_c0 pi-]cc"],    
                  'Omegab2Omegac0K' : ["[Omega_b- -> Omega_c0 K-]cc"]}      
        inputs = {'Omegab2Omegac0Pi': self.omegac0+pions, 'Omegab2Omegac0K': self.omegac0+kaons}
        rs = makeB2XSels(decays,'Omegac02PKKPi',inputs,self.config)
        decays = {'Omegab2Omegac0PiWS': ["[Omega_b- -> Omega_c0 pi+]cc"],
                  'Omegab2Omegac0KWS' : ["[Omega_b- -> Omega_c0 K+]cc"]}
        inputs = {'Omegab2Omegac0PiWS':self.omegac0+pions, 'Omegab2Omegac0KWS':self.omegac0+kaons}
        ws = makeB2XSels(decays,'Omegac02PKKPi',inputs,self.config)
        decays = {'Omegab2Omegac0PiNoIP': ["[Omega_b- -> Omega_c0 pi-]cc"]}
        inputs = {'Omegab2Omegac0PiNoIP': self.omegac0_pid+pions} 
        noip = makeB2XSels(decays,'Omegac02PKKPi',inputs,self.config,False)
        decays = {'Omegab2Omegac0PiNoIPWS': ["[Omega_b- -> Omega_c0 pi+]cc"]}
        inputs = {'Omegab2Omegac0PiNoIPWS':self.omegac0_pid+pions}  
        noip_ws = makeB2XSels(decays,'Omegac02PKKPi',inputs,self.config,False)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))
        self.lines.append(ProtoLine(noip,1.0))
        self.lines.append(ProtoLine(noip_ws,0.1))     
     
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
        '''Make RS and WS X_b ->Lc HHH (H=pi,K) + cc.'''
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        kkpi = self.hhh.kkpi
        ppbarpi = self.hhh.ppbarpi
        ppbark = self.hhh.ppbark
        pppbar = self.hhh.pppbar
        
        ppipi = self.hhh.ppipiOS
        pkpi = self.hhh.pkpiOS
        pkk = self.hhh.pkkOS
        
        decays = {'Lb2LcPiPiPi'   : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcKPiPi'    : ["[Lambda_b0 -> Lambda_c+ K_1(1270)-]cc"],
                  'Lb2LcppbarPi'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcppbarK'   : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"],
                  'Lb2LcKKPi'     : ["[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc"]}
        inputs = {'Lb2LcPiPiPi'   : self.lc_pid+pipipi,
                  'Lb2LcKPiPi'    : self.lc_pid+kpipi,
                  'Lb2LcppbarPi'  : self.lc_pid+ppbarpi,
                  'Lb2LcppbarK'   : self.lc_pid+ppbark,
                  'Lb2LcKKPi'     : self.lc_pid+kkpi}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        decays = {'Lb2LcPiPiPiWS'   : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcKPiPiWS'    : ["[Lambda_b0 -> Lambda_c+ K_1(1270)+]cc"],
                  'Lb2LcppbarPiWS'  : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcppbarKWS'   : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"],
                  'Lb2LcKKPiWS'     : ["[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc"]}
        inputs = {'Lb2LcPiPiPiWS'   : self.lc_pid+pipipi,
                  'Lb2LcKPiPiWS'    : self.lc_pid+kpipi,
                  'Lb2LcppbarPiWS'  : self.lc_pid+ppbarpi,
                  'Lb2LcppbarKWS'   : self.lc_pid+ppbark,
                  'Lb2LcKKPiWS'     : self.lc_pid+kkpi}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

        config = deepcopy(self.config)
        config['AM_MIN'] = '4750.*MeV'
        decays = {'B02Lcpbarpbarp': ["[B0 -> Lambda_c+ a_1(1260)-]cc"], 
                  'B02LcpbarPiPi':  ["[B0 -> Lambda_c+ a_1(1260)-]cc"], 
                  'B02LcpbarKPi':   ["[B0 -> Lambda_c+ a_1(1260)-]cc"], 
                  'B02LcpbarKK':    ["[B0 -> Lambda_c+ a_1(1260)-]cc"] 
                 }
        inputs = {'B02Lcpbarpbarp': self.lc_pid+pppbar, 
                  'B02LcpbarPiPi': self.lc_pid+ppipi, 
                  'B02LcpbarKPi': self.lc_pid+pkpi, 
                  'B02LcpbarKK': self.lc_pid+pkk, 
                 }
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        decays = {'B02LcpbarpbarpWS': ["[B0 -> Lambda_c+ a_1(1260)+]cc"], 
                  'B02LcpbarPiPiWS':  ["[B0 -> Lambda_c+ a_1(1260)+]cc"], 
                  'B02LcpbarKPiWS':   ["[B0 -> Lambda_c+ a_1(1260)+]cc"], 
                  'B02LcpbarKKWS':    ["[B0 -> Lambda_c+ a_1(1260)+]cc"], 
                 }
        inputs = {'B02LcpbarpbarpWS': self.lc_pid+pppbar, 
                  'B02LcpbarPiPiWS':  self.lc_pid+ppipi, 
                  'B02LcpbarKPiWS':   self.lc_pid+pkpi, 
                  'B02LcpbarKKWS':    self.lc_pid+pkk, 
                  }
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))


    def _makeLb2D0PH(self):
        '''Makes RS Lb -> D0(HH) p+- H-+ + c.c. and WS lines'''
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
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

    def _makeLb2DLambda0pi(self):
        '''Makes RS/WS Lb -> D+(HHH) Lambda0 pi- + c.c.'''
        decays = {'Lb2DLambda0DDpi': ["[Lambda_b0 -> D+ Lambda0 pi-]cc"] }
        inputs = {'Lb2DLambda0DDpi': self.d.hhh_cf_pid + self.lambda0["DD"] + self.pions }
        lb_dd = makeB2XSels(decays,'D2HHH',inputs,self.config)
        self.lines.append(ProtoLine(lb_dd,1.0))
        decays = {'Lb2DLambda0LLpi': ["[Lambda_b0 -> D+ Lambda0 pi-]cc"]}
        inputs = {'Lb2DLambda0LLpi': self.d.hhh_cf_pid + self.lambda0["LL"] + self.pions }
        lb_ll = makeB2XSels(decays,'D2HHH',inputs,self.config)
        self.lines.append(ProtoLine(lb_ll,1.0))
        decays = {'Lb2DLambda0DDpiWS': ["[Lambda_b0 -> D+ Lambda0 pi+]cc"] }
        inputs = {'Lb2DLambda0DDpiWS': self.d.hhh_cf_pid + self.lambda0["DD"] + self.pions }
        lb_dd = makeB2XSels(decays,'D2HHH',inputs,self.config)
        self.lines.append(ProtoLine(lb_dd,0.1))
        decays = {'Lb2DLambda0LLpiWS': ["[Lambda_b0 -> D+ Lambda0 pi+]cc"]}
        inputs = {'Lb2DLambda0LLpiWS': self.d.hhh_cf_pid + self.lambda0["LL"] + self.pions }
        lb_ll = makeB2XSels(decays,'D2HHH',inputs,self.config)
        self.lines.append(ProtoLine(lb_ll,0.1))
    
    def _makeXib2D0Lambda0pi(self):
        '''Makes Xib -> D0(HH) Lambda0 pi- + c.c.'''
        decays = {'Xib2D0Lambda0DDpi': ["Xi_b- -> D0 Lambda0 pi-","Xi_b~+ -> D0 Lambda~0 pi+"] }
        inputs = {'Xib2D0Lambda0DDpi': self.d0 + self.lambda0["DD"] + self.pions }
        lb_dd = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(lb_dd,1.0))
        decays = {'Xib2D0Lambda0LLpi': ["Xi_b- -> D0 Lambda0 pi-","Xi_b~+ -> D0 Lambda~0 pi+"]}
        inputs = {'Xib2D0Lambda0LLpi': self.d0 + self.lambda0["LL"] + self.pions }
        lb_ll = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(lb_ll,1.0))
        decays = {'Xib2D0Lambda0DDpiWS': ["Xi_b- -> D0 Lambda~0 pi-","Xi_b~+ -> D0 Lambda0 pi+"] }
        inputs = {'Xib2D0Lambda0DDpiWS': self.d0 + self.lambda0["DD"] + self.pions }
        lb_dd = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(lb_dd,0.1))
        decays = {'Xib2D0Lambda0LLpiWS': ["Xi_b- -> D0 Lambda~0 pi-","Xi_b~+ -> D0 Lambda0 pi+"]}
        inputs = {'Xib2D0Lambda0LLpiWS': self.d0 + self.lambda0["LL"] + self.pions }
        lb_ll = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(lb_ll,0.1))

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

    def _makeLb2LcDKst(self):
        '''Makes RS Lb -> Lc D+ Kst c.c.'''
        decays = {'Lb2LcDKst': ["[Lambda_b0 -> Lambda_c+ D- K*(892)0]cc"]}
        inputs = {'Lb2LcDKst': self.d.hhh_pid+self.lc_pid + self.hh.kpi}
        rs = makeB2XSels(decays,'',inputs,self.config)   
        self.lines.append(ProtoLine(rs,1.0))

    def _makeLb2LcDstKst(self):
        '''Makes RS Lb -> Lc D*(2010)+ Kst c.c.'''
        decays = {'Lb2LcDstKst': ["[Lambda_b0 -> Lambda_c+ D*(2010)- K*(892)0]cc"]}
        inputs = {'Lb2LcDstKst': self.dst.d0pi_pid+self.lc_pid + self.hh.kpi}
        rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0)) 

    def _makeX2LcD0(self):
        '''Makes RS X -> Lc D0 + c.c.'''
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X2LcD0': ["B+ -> Lambda_c+ D0","B- -> Lambda_c~- D0"]}
        inputs = {'X2LcD0': self.d.kpi+self.lc_pid}
        rs = makeB2XSels(decays,'D02KPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeX2LcD0H(self):
        '''Makes RS + WS X -> Lc D0 H + c.c. Need loose pions here as they may be soft''' 
        pions = self.pions
        kaons = self.kaons
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X2LcD0Pi': ["B0 -> Lambda_c+ D0 pi-", "B0 -> Lambda_c~- D0 pi+"],
                  'X2LcD0K': ["B0 -> Lambda_c+ D0 K-", "B0 -> Lambda_c~- D0 K+"]}
        inputs = {'X2LcD0Pi': self.d.kpi+self.lc_pid+pions,
                  'X2LcD0K': self.d.kpi+self.lc_pid+kaons}
        rs = makeB2XSels(decays,'D02KPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'X2LcD0PiWS': ["B0 -> Lambda_c+ D0 pi+","B0 -> Lambda_c~- D0 pi-"],
                  'X2LcD0KWS': ["B0 -> Lambda_c+ D0 K+","B0 -> Lambda_c~- D0 K-"]}
        inputs = {'X2LcD0PiWS': self.d.kpi+self.lc_pid+pions,
                  'X2LcD0KWS': self.d.kpi+self.lc_pid+kaons}
        ws = makeB2XSels(decays,'D02KPi',inputs,config)
        self.lines.append(ProtoLine(ws,0.1))


    def _makeX2LcHH(self):
        '''Makes RS + WS Xib -> Lc H+H-, LC H-H- + c.c.'''
        pions = self.pions
        kaons = self.kaons
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X2LcPiPiSS': ["[B- -> Lambda_c+ pi- pi-]cc"],
                  'X2LcKPiSS': ["[B- -> Lambda_c+ K- pi-]cc"],                                
                  'X2LcKKSS': ["[B- -> Lambda_c+ K- K-]cc"],
                  'X2LcPiPiOS': ["[B+ -> Lambda_c+ pi- pi+]cc"],
                  'X2LcKPiOS': ["[B+ -> Lambda_c+ K- pi+]cc"],
                  'X2LcPiKOS': ["[B+ -> Lambda_c+ K+ pi-]cc"],                                
                  'X2LcKKOS': ["[B+ -> Lambda_c+ K- K+]cc"]
                  }
        inputs = {'X2LcPiPiSS': self.lc_pid+pions,
                  'X2LcKPiSS': self.lc_pid+kaons+pions,
                  'X2LcKKSS': self.lc_pid+kaons,
                  'X2LcPiPiOS': self.lc_pid+pions,
                  'X2LcKPiOS': self.lc_pid+kaons+pions,
                  'X2LcPiKOS': self.lc_pid+kaons+pions,
                  'X2LcKKOS': self.lc_pid+kaons}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))

        decays = {'X2LcPiPiWS': ["[B+ -> Lambda_c+ pi+ pi+]cc"],
                  'X2LcKPiWS': ["[B+ -> Lambda_c+ K+ pi+]cc"],                                
                  'X2LcKKWS': ["[B+ -> Lambda_c+ K+ K+]cc"]
                  }
        inputs = {'X2LcPiPiWS': self.lc_pid+pions,
                  'X2LcKPiWS': self.lc_pid+kaons+pions,
                  'X2LcKKWS': self.lc_pid+kaons}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))


    def _makeB2LcpbarH(self):
        '''Makes RS + WS B- -> Lc+ pbar- h- + c.c.'''
        pions = self.pions
        kaons = self.kaons
        protons = self.protons
        config = deepcopy(self.config)
        config['AM_MIN'] = '4750.*MeV'
        decays = {'B2LcpbarPiSS': ["[B- -> Lambda_c+ p~- pi-]cc"],
                  'B2LcpbarKSS':  ["[B- -> Lambda_c+ p~- K-]cc"],
                  }
        inputs = {'B2LcpbarPiSS': self.lc_pid+pions+protons,
                  'B2LcpbarKSS':  self.lc_pid+kaons+protons}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))

        decays = {'B2LcpbarPiWS': ["[B+ -> Lambda_c+ p~- pi+]cc", "[B+ -> Lambda_c+ p+ pi-]cc"],
                  'B2LcpbarKWS':  ["[B+ -> Lambda_c+ p~- K+]cc", "[B+ -> Lambda_c+ p+ K-]cc"],
                  }
        inputs = {'B2LcpbarPiWS': self.lc_pid+pions+protons,
                  'B2LcpbarKWS':  self.lc_pid+kaons+protons}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeB02LcpbarKS(self,ks):
        '''Makes RS + WS B0 -> Lc+ pbar- Ks + c.c.'''
        protons = self.protons
        config = deepcopy(self.config)
        config['AM_MIN'] = '4750.*MeV'
        decays = {'B02LcpbarKS'+ks: ["[B0 -> Lambda_c+ p~- KS0]cc"]}
        inputs = {'B02LcpbarKS'+ks: self.lc_pid+self.ks[ks]+protons}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))                  
        decays = {'B02LcpbarKSWS'+ks: ["[B0 -> Lambda_c+ p+ KS0]cc"]}
        inputs = {'B02LcpbarKSWS'+ks: self.lc_pid+self.ks[ks]+protons}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeB2LcLambda0barH(self):
        '''Makes RS + WS B0 -> Lc+ Lambda0bar h- + c.c.'''
        pions = self.pions
        kaons = self.kaons
        lambdadd = self.lambda0["DD"]
        lambdall = self.lambda0["LL"]
        lambdas  = lambdadd + lambdall
        config = deepcopy(self.config)
        config['AM_MIN'] = '4750.*MeV'
        decays = {'B02LcLambda0Pi': ["[B0 -> Lambda_c+ Lambda~0 pi-]cc"],
                  'B02LcLambda0K':  ["[B0 -> Lambda_c+ Lambda~0 K-]cc"],
                  }
        inputs = {'B02LcLambda0Pi': self.lc_pid+pions+lambdas,
                  'B02LcLambda0K':  self.lc_pid+kaons+lambdas}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        self.lines.append(ProtoLine(rs,1.0))

        decays = {'B02LcLambda0PiWS': ["[B0 -> Lambda_c+ Lambda~0 pi+]cc"],
                  'B02LcLambda0KWS':  ["[B0 -> Lambda_c+ Lambda~0 K+]cc"],
                 }
        inputs = {'B02LcLambda0PiWS': self.lc_pid+pions+lambdas,
                  'B02LcLambda0KWS':  self.lc_pid+kaons+lambdas}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeX2LcKPiPiPi(self):
        '''Makes RS + WS Xib -> Lc HH + c.c.'''
        pions = self.pions
        kaons = self.kaons
        pipipi = self.hhh.pipipi
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X2LcKPiPiPi': ["[B+ -> Lambda_c+ a_1(1260)+ K- ]cc"]}
        inputs = {'X2LcKPiPiPi': self.lc_pid+pipipi+kaons}
        rs = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))

        decays = {'X2LcKPiPiPiWS': ["[B+ -> Lambda_c+ a_1(1260)+ K+]cc"]}
        inputs = {'X2LcKPiPiPiWS': self.lc_pid+pipipi+kaons}
        ws = makeB2XSels(decays,'Lc2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))


    def _makeXib2XicHH(self):
        '''Makes RS + WS Xib -> Lc HH + c.c.'''
        pions = self.pions
        kaons = self.kaons
        decays = {'Xib2XicPiPi': ["[Xi_b- -> Xi_c+ pi- pi-]cc"],
                  'Xib2XicKPi': ["[Xi_b- -> Xi_c+ K- pi-]cc"],                                
                  'Xib2XicKK': ["[Xi_b- -> Xi_c+ K- K-]cc"]
                  }
        inputs = {'Xib2XicPiPi': self.xic+pions,
                  'Xib2XicKPi': self.xic+kaons+pions,
                  'Xib2XicKK': self.xic+kaons}
        rs = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'Xib2XicPiPiWS': ["[Xi_b- -> Xi_c+ pi+ pi+]cc"],
                  'Xib2XicKPiWS': ["[Xi_b- -> Xi_c+ K+ pi+]cc"],                                
                  'Xib2XicKKWS': ["[Xi_b- -> Xi_c+ K+ K+]cc"]
                  }
        inputs = {'Xib2XicPiPiWS': self.xic+pions,
                  'Xib2XicKPiWS': self.xic+kaons+pions,
                  'Xib2XicKKWS': self.xic+kaons}
        ws = makeB2XSels(decays,'Xic2PKPi',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))


    def _makeXib2LcDsK(self):
        '''Makes RS + WS Xib -> Lc+ Ds - K- + c.c.'''
        #pions = self.pions
        kaons = self.kaons
        decays = {'Xib2LcDsK': ["[Xi_b- -> Lambda_c+ D- K-]cc"]}
        inputs = {'Xib2LcDsK': self.lc_pid+self.ds+kaons}
        rs = makeB2XSels(decays,'Lc2PKPiDs2KKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'Xib2LcDsKWS': ["[Xi_b- -> Lambda_c+ D+ K-]cc"]}
        inputs = {'Xib2LcDsKWS': self.lc_pid+self.ds+kaons}
        ws = makeB2XSels(decays,'Lc2PKPiDs2KKPi',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeXib02LcDsKPi(self):
        '''Makes RS + WS Xib -> Lc+ Ds- K- Pi+ + c.c.'''
        pions = self.pions
        kaons = self.kaons
        decays = {'Xib02LcDsKPi': ["[Xi_b0 -> Lambda_c+ D- K- pi+]cc"]}
        inputs = {'Xib02LcDsKPi': self.lc_pid+self.ds+kaons+pions}
        rs = makeB2XSels(decays,'Lc2PKPiDs2KKPi',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'Xib02LcDsKPiWS': ["[Xi_b0 -> Lambda_c+ D- K+ pi+]cc"]}
        inputs = {'Xib02LcDsKPiWS': self.lc_pid+self.ds+kaons+pions}
        ws = makeB2XSels(decays,'Lc2PKPiDs2KKPi',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))

    def _makeB2LcDspbar(self):
        '''Makes RS + WS B- -> Lc+ Ds- pbar- + c.c.'''
        protons = self.protons
        config = deepcopy(self.config)
        config['AM_MIN'] = '4750.*MeV'
        decays = {'B2LcDspbar': ["[B- -> Lambda_c+ D- p~-]cc"]}
        inputs = {'B2LcDspbar': self.lc_pid+self.d.hhh_pid+protons}
        rs = makeB2XSels(decays,'Lc2PKPiDs2KKPi',inputs, config)
        self.lines.append(ProtoLine(rs,1.0))
        decays = {'B2LcDspbarWS': ["[B- -> Lambda_c+ D+ p~-]cc"]}
        inputs = {'B2LcDspbarWS': self.lc_pid+self.d.hhh_pid+protons}
        ws = makeB2XSels(decays,'Lc2PKPiDs2KKPi',inputs,self.config)
        self.lines.append(ProtoLine(ws,0.1))
        
    def _makeX02XiccH(self):
        '''Make RS and WS Xi_bcd -> Xi_cc+ H (H=pi,K) + cc.'''
        pions = self.pions
        kaons = self.kaons
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X02XiccpPi': ["[B0 -> Xi_cc+ pi-]cc"],
                  'X02XiccpK' : ["[B0 -> Xi_cc+ K-]cc"]}
        inputs = {'X02XiccpPi': self.xiccp+pions, 'X02XiccpK': self.xiccp+kaons}
        rs = makeB2XSels(decays,'Xiccp2Xic0Pi',inputs,self.config)

        decays = {'X02XiccpPiWS': ["[B0 -> Xi_cc+ pi+]cc"],
                  'X02XiccpKWS' : ["[B0 -> Xi_cc+ K+]cc"]}
        inputs = {'X02XiccpPiWS': self.xiccp+pions, 'X02XiccpKWS': self.xiccp+kaons}
        ws = makeB2XSels(decays,'Xiccp2Xic0Pi',inputs,self.config)

        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))


    def _makeX2XiccH(self):
        '''Make RS and WS Xi_bcu -> LXi_cc++ H (H=pi,K) + cc.'''
        pions = self.pions
        kaons = self.kaons
        config = deepcopy(self.config)
        config['AM_MAX'] = '9000*MeV'
        decays = {'X2XiccppPi': ["[B+ -> Xi_cc++ pi-]cc"],
                  'X2XiccppK' : ["[B+ -> Xi_cc++ K-]cc"]}
        inputs = {'X2XiccppPi': self.xiccpp+pions, 'X2XiccppK': self.xiccpp+kaons}
        rs = makeB2XSels(decays,'Xiccpp2XicPi',inputs,self.config)

        decays = {'X2XiccppPiWS': ["[B+ -> Xi_cc++ pi+]cc"],
                  'X2XiccppKWS' : ["[B+ -> Xi_cc++ K+]cc"]}
        inputs = {'X2XiccppPiWS': self.xiccpp+pions, 'X2XiccppKWS': self.xiccpp+kaons}
        ws = makeB2XSels(decays,'Xiccpp2XicPi',inputs,self.config)

        self.lines.append(ProtoLine(rs,1.0))
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
        decays = {'X2LcLc': ["B0 -> Lambda_c+ Lambda_c~-"]}
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

    def _makeLb2DpHH(self):
        '''Make RS and WS Lb ->DpHH (H=pi,K) + cc.'''
        config = deepcopy(self.config)
        config['AM_MAX'] = '7000*MeV'
        ppipi = self.hhh.ppipi_pid
        pkpi = self.hhh.pkpi_pid
        pkk = self.hhh.pkk_pid
        decays = {'Lb2DpPiPi'  : ["[Lambda_b0 -> D+ a_1(1260)-]cc"],
                  'Lb2DpKPi'   : ["[Lambda_b0 -> D+ a_1(1260)-]cc"],
                  'Lb2DpKK'    : ["[Lambda_b0 -> D+ a_1(1260)-]cc"]}
        inputs = {'Lb2DpPiPi'  : self.d.hhh_cf_pid+ppipi,
                  'Lb2DpKPi'   : self.d.hhh_cf_pid+pkpi,
                  'Lb2DpKK'    : self.d.hhh_cf_pid+pkk}
        rs = makeB2XSels(decays,'D2HHH',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        
    def _makeXib2D0pHH(self):
        '''Make RS and WS Xi_b ->DpHH (H=pi,K) + cc.'''
        config = deepcopy(self.config)
        config['AM_MAX'] = '7000*MeV'
        ppipi = self.hhh.ppipi_pid
        pkpi = self.hhh.pkpi_pid
        pkk = self.hhh.pkk_pid
        decays = {'Xib2D0pPiPi'  : ["Xi_b- -> D0 a_1(1260)-","Xi_b~+ -> D0 a_1(1260)+"],
                  'Xib2D0pKPi'   : ["Xi_b- -> D0 a_1(1260)-","Xi_b~+ -> D0 a_1(1260)+"],
                  'Xib2D0pKK'    : ["Xi_b- -> D0 a_1(1260)-","Xi_b~+ -> D0 a_1(1260)+"]}
        inputs = {'Xib2D0pPiPi'  : self.d0+ppipi,
                  'Xib2D0pKPi'   : self.d0+pkpi,
                  'Xib2D0pKK'    : self.d0+pkk}
        rs = makeB2XSels(decays,'D02HH',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#


