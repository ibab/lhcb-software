#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Configurables import SubstitutePID

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class B2DXBuilder(object):
    '''Makes all B->DX decays for the Beauty2Charm module.'''

    def __init__(self,d,dst,topoPions,topoKaons,hh,df,hhh,config):
        self.config = config
        self.topoPions = [topoPions]
        self.topoKaons = [topoKaons]
        self.d = d
        self.dst = dst
        self.hh = hh
        self.hhh = hhh
        self.fhhh = df.fhhh
        self.fhh = df.fhh
        self.fdst = df.fdst
        self.d2hh_biased = df.hh_biased
        self.d2hhh_biased = df.hhh_biased
        self.dst_biased = df.dst_biased
        self.lines = []        
        
        # B -> D0(HH) X
        self._makeB2D0H('D2HH',self.d.hh)   # B+- -> D0(HH)  H+-
        self._makeB02D0HH('D2HH',self.d.hh) # B0  -> D0(HH)  H+ H-
        # B -> D+-(HHH) X
        self._makeB02DH('D2HHH',self.d.hhh) # B0  -> D+-(HHH) H-+   (+WS)
        self._makeB02DHH('D2HHH',self.d.hhh)# B0  -> D+-(HHH) (HH)-+   (+WS) 
        # B -> D(KSH) X (LL & DD)
        self._makeB02DH('D2KSH_LL',self.d.ksh_ll) # B0  -> D+-(KSLLH) H-+ (+WS)
        self._makeB02DH('D2KSH_DD',self.d.ksh_dd) # B0  -> D+-(KSDDH) H-+ (+WS)
        self._makeB2D0H('D2KSHH_LL',self.d.kshh_ll)# B+- -> D0(KSLLHH) H+-
        self._makeB2D0H('D2KSHH_DD',self.d.kshh_dd)# B+- -> D0(KSDDHH) H+-
        self._makeB02D0HH('D2KSHH_LL',self.d.kshh_ll) # B0  -> D0(KSLLHH)H+H-
        self._makeB02D0HH('D2KSHH_DD',self.d.kshh_dd) # B0  -> D0(KSDDHH)H+H- 
        # B -> D0(HHHH) X
        self._makeB2D0H('D2HHHH',self.d.hhhh) # B+- -> D0(HHHH) H+-
        self._makeB02D0HH('D2KPiPiPi',self.d.kpipipi) # B0  -> D0(K3Pi)  H+ H-
        # B -> D D X 
        self._makeB2DstDK() # B+- -> D*(D0(HH)Pi)+-D-+(HHH)K+- (+WS)
        self._makeB2DDKst() # B0 -> D0D0K*
        # B+- -> D0(Pi0HH) H+- (resolved & merged)
        self._makeB2D0H('D2Pi0HH_Resolved',self.d.pi0hh_resolved)
        self._makeB2D0H('D2Pi0HH_Merged',self.d.pi0hh_merged)
        # B+- -> D0(Pi0HH) H+H- (resolved & merged)
        self._makeB02D0HH('D2Pi0KPi_Resolved',self.d.pi0kpi_resolved)
        self._makeB02D0HH('D2Pi0KPi_Merged',self.d.pi0kpi_merged) 
        # B0 -> D+-(Pi0HHH) H-+  (resolved & merged) (+WS)
        self._makeB02DH('D2Pi0HHH_Resolved',self.d.pi0hhh_resolved)
        self._makeB02DH('D2Pi0HHH_Merged',self.d.pi0hhh_merged) 
        # B -> D0(HH) 3h
        self._makeB2D0HHH('',self.fhh)   # B+- -> D0(HH)  H+H-H+
        # B -> D(HHH) 3h
        self._makeB02DHHH('',self.fhhh)   # B+- -> D0(HH)  H+H-H+
        # B -> D*(D0pi) 3h
        self._makeB02DstHHH('',self.fdst)   # B+- -> D0(HH)  H+H-H+
        # B -> D+ D-
        self._makeB2DD('',self.fhhh)   # B0(s)- -> D(s) D(s)
        # B- -> D- D0
        self._makeB2D0D('', self.fhhh, self.fhh)   # B0(s)- -> D(s) D0
        # B -> D*+ D-
        self._makeB2DstD(self.fhhh, self.fdst)   # B0(s)- -> D*(D0pi) D(s)
        
    def _makeB02DH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'B02DPi': ["[B0 -> D- pi+]cc"], 'B02DK': ["[B0 -> D- K+]cc"]}
        inputs = {'B02DPi': d2x+pions, 'B02DK': d2x+kaons}
        b02dh_rs = makeB2XMerged('B02DH',decays,dname,inputs,self.config)
        decays = {'B02DPiWS': ["[B0 -> D- pi-]cc"],
                  'B02DKWS': ["[B0 -> D- K-]cc"]}
        inputs = {'B02DPiWS': d2x+pions, 'B02DKWS': d2x+kaons}
        b02dh_ws = makeB2XMerged('B02DHWS',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dh_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b02dh_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b02dh_ws['TOS'],0.1))
   
    def _makeB02DHH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- h0 + c.c.'''
        decays = {'B02DPiPi0': ["[B0 -> D- rho(770)+]cc"],
                  'B02DKsPi' : ["[B0 -> D- K*(892)+]cc"],
                  'B02DKPi0' : ["[B0 -> D- K*(892)+]cc"]}
        inputs = {'B02DPiPi0': d2x+self.hh.rhoplus,
                  'B02DKsPi' : d2x+self.hh.kstplus_kspi,
                  'B02DKPi0' : d2x+self.hh.kstplus_kpi0}
        b02dhh_rs = makeB2XMerged('B02DHH',decays,dname,inputs,self.config)
        decays = {'B02DPiPi0WS': ["[B0 -> D- rho(770)-]cc"],
                  'B02DKsPiWS' : ["[B0 -> D- K*(892)-]cc"],
                  'B02DKPi0WS' : ["[B0 -> D- K*(892)-]cc"]}
        inputs = {'B02DPiPi0WS': d2x+self.hh.rhoplus,
                  'B02DKsPiWS' : d2x+self.hh.kstplus_kspi,
                  'B02DKPi0WS' : d2x+self.hh.kstplus_kpi0}
        b02dhh_ws = makeB2XMerged('B02DHHWS',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dhh_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b02dhh_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b02dhh_ws['TOS'],0.1))
 
    def _makeB2D0H(self,dname,d2x):
        '''Makes RS B+ -> D0 h+ (h=pi,K) + c.c.'''
        decays = {'B2D0Pi': ["B+ -> D0 pi+","B- -> D0 pi-"],
                  'B2D0K' : ["B+ -> D0 K+","B- -> D0 K-"]}
        inputs = {'B2D0Pi': d2x+self.topoPions,'B2D0K': d2x+self.topoKaons}
        b2d0h = makeB2XMerged('B2D0H',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0h['TOS'],1.0))
        self.lines.append(ProtoLine(b2d0h['TIS'],1.0))

    def _makeB02D0HH(self,dname,d2x):
        '''Makes RS B0 -> D0 h+h- (h=pi,K) + c.c.'''
        decays = {'B02D0PiPi': ["B0 -> D0 rho(770)0"],
                  'B02D0KPi' : ["B0 -> D0 K*(892)0","B0 -> D0 K*(892)~0"],
                  'B02D0KK'  : ["B0 -> D0 phi(1020)"]}
        inputs = {'B02D0PiPi': d2x+self.hh.pipi,
                  'B02D0KPi' : d2x+self.hh.kpi,
                  'B02D0KK'  : d2x+self.hh.kk}
        b2d0hh = makeB2XMerged('B02D0HH',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0hh['TOS'],1.0))
        self.lines.append(ProtoLine(b2d0hh['TIS'],1.0))

    def _makeB02D0V0(self,dname,d2x):
        '''Makes RS B0 -> D0 V0 (V0 = rho0, K*0, phi)'''
        decays = {'B02D0PiPi': ["B0 -> D0 rho(770)0"],
                  'B02D0KPi' : ["B0 -> D0 K*(892)0","B0 -> D0 K*(892)~0"],
                  'B02D0KK'  : ["B0 -> D0 phi(1020)"]}
        inputs = {'B02D0PiPi': d2x+self.hh.rho0,
                  'B02D0KPi' : d2x+self.hh.kstar0,
                  'B02D0KK'  : d2x+self.hh.phi}
        b2d0hh = makeB2XMerged('B02D0V0',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0hh['TOS'],1.0))
        self.lines.append(ProtoLine(b2d0hh['TIS'],1.0))

    def _makeB2DstDK(self):
        '''Makes the RS and WS B+ -> D*+- D-+ K+ + c.c.'''
        decays = {'B2DstDK':["[B+ -> D*(2010)+ D- K+]cc",
                             "[B+ -> D*(2010)- D+ K+]cc"]}
        inputs = {'B2DstDK':self.dst_biased+self.d2hhh_biased+self.topoKaons}
        b2dstdk_rs = makeB2XMerged('B2DstDK',decays,'Dst2D0PiD2HHH',inputs,
                                   self.config)
        decays = {'B2DstDKWS':["[B+ -> D*(2010)+ D+ K+]cc"]}
        inputs = {'B2DstDKWS':self.dst_biased+self.d2hhh_biased+self.topoKaons}
        b2dstdk_ws = makeB2XMerged('B2DstDKWS',decays,'Dst2D0PiD2HHH',
                                   inputs,self.config)
        self.lines.append(ProtoLine(b2dstdk_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b2dstdk_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b2dstdk_ws['TOS'],0.1))

    def _makeB2DDKst(self):
        '''Makes the RS B0 -> D0 D0 K*'''
        decays = {'B2DDKst':["B0 -> D0 D0 K*(892)0",
                             "B0 -> D0 D0 K*(892)~0"]}
        inputs = {'B2DDKst':self.d2hh_biased+self.hh.kstar0}
        b2ddkst = makeB2XMerged('B2DDKst',decays,'D2HH',inputs,
                                self.config)
        self.lines.append(ProtoLine(b2ddkst['TOS'],1.0))
        self.lines.append(ProtoLine(b2ddkst['TIS'],1.0))

    # SB ------------------
    def _makeB02DHHH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        decays = {'B02DPiPiPi': ["[B0 -> D- a_1(1260)+]cc"],
                  'B02DKPiPi' : ["[B0 -> D- K_1(1270)+]cc"]}
        inputs = {'B02DPiPiPi': d2x+pipipi, 'B02DKPiPi': d2x+kpipi}
        b02dhhh_rs = makeB2XMerged('B02DHHH',decays,dname,inputs,self.config)
        decays = {'B02DPiPiPiWS': ["[B0 -> D- a_1(1260)-]cc"],
                  'B02DKPiPiWS': ["[B0 -> D- K_1(1270)-]cc"]}
        inputs = {'B02DPiPiPiWS': d2x+pipipi, 'B02DKPiPiWS': d2x+kpipi}
        b02dhhh_ws = makeB2XMerged('B02DHHHWS',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dhhh_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b02dhhh_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b02dhhh_ws['TOS'],0.1))

    def _makeB02DstHHH(self,dname,dstar):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        decays = {'B02DstPiPiPi': ["[B0 -> D*(2010)- a_1(1260)+]cc"],
                  'B02DstKPiPi' : ["[B0 -> D*(2010)- K_1(1270)+]cc"]}
        inputs = {'B02DstPiPiPi': dstar+pipipi, 'B02DstKPiPi': dstar+kpipi}
        b02dsthhh_rs = makeB2XMerged('B02DstHHH',decays,dname,inputs,
                                     self.config)
        decays = {'B02DstPiPiPiWS': ["[B0 -> D*(2010)+ a_1(1260)+]cc"],
                  'B02DstKPiPi'   : ["[B0 -> D*(2010+- K_1(1270)+]cc"]}
        inputs = {'B02DstPiPiPiWS': dstar+pipipi,
                  'B02DstKPiPiWS' : dstar+kpipi}
        b02dsthhh_ws = makeB2XMerged('B02DstHHHWS',decays,dname,inputs,
                                     self.config)
        self.lines.append(ProtoLine(b02dsthhh_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b02dsthhh_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b02dsthhh_ws['TOS'],0.1))

    def _makeB02DstHHH(self,dname,d2x):
        '''Makes RS and WS B0 -> D*+ h-h+h- + c.c.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        decays = {'B02DstPiPiPi': ["[B0 -> D*(2010)- a_1(1260)+]cc"],
                  'B02DstKPiPi' : ["[B0 -> D*(2010)- K_1(1270)+]cc"]}
        inputs = {'B02DstPiPiPi': d2x+pipipi, 'B02DstKPiPi': d2x+kpipi}
        b02dsthhh_rs = makeB2XMerged('B02DstHHH',decays,dname,inputs,
                                     self.config)
        decays = {'B02DstPiPiPiWS': ["[B0 -> D*(2010)- a_1(1260)-]cc"],
                  'B02DstKPiPiWS' : ["[B0 -> D*(2010)- K_1(1270)-]cc"]}
        inputs = {'B02DstPiPiPiWS': d2x+pipipi, 'B02DstKPiPiWS': d2x+kpipi}
        b02dsthhh_ws = makeB2XMerged('B02DstHHHWS',decays,dname,inputs,
                                     self.config)
        self.lines.append(ProtoLine(b02dsthhh_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b02dsthhh_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b02dsthhh_ws['TOS'],0.1))

    def _makeB2D0HHH(self,dname,d2x):
        '''Makes RS B+ -> D0 h+h-h+ (h=pi,K) + c.c.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        decays = {'B2D0PiPiPi': ["B+ -> D0 a_1(1260)+","B- -> D0 a_1(1260)-"],
                  'B2D0KPiPi' : ["B+ -> D0 K_1(1270)+","B- -> D0 K_1(1270)-"]}
        inputs = {'B2D0PiPiPi': d2x+pipipi,'B2D0KPiPi': d2x+kpipi}
        b2d03h = makeB2XMerged('B2D0HHH',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d03h['TOS'],1.0))
        self.lines.append(ProtoLine(b2d03h['TIS'],1.0))

    def _makeB2DD(self,dname,d2x):
        '''Makes RS and WS B0 -> D+D- + c.c.'''
        decays = {'B2DD': ["B0 -> D- D+"] }
        inputs = {'B2DD': d2x}
        b2dd_rs = makeB2XMerged('B2DD',decays,dname,inputs,self.config)
        decays = {'B2DDWS': ["[B0 -> D+ D+]cc"] }
        inputs = {'B2DDWS': d2x}
        b2dd_ws = makeB2XMerged('B2DDWS',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2dd_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b2dd_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b2dd_ws['TOS'],0.1))

    def _makeB2D0D(self,dname,d2x,d0):
        '''Makes B+ -> D+ D0'''
        decays = {'B2D0D': ["B- -> D0 D-","B+ -> D0 D+"] }
        inputs = {'B2D0D': d2x+d0}
        b2d0d = makeB2XMerged('B2D0D',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0d['TOS'],1.0))
        self.lines.append(ProtoLine(b2d0d['TIS'],1.0))

    def _makeB2DstD(self, d2x,dstar):
        '''Makes the RS and WS B+ -> D*+- D-+ + c.c.'''
        decays = {'B2DstD':["[B0 -> D*(2010)- D+]cc"]}
        inputs = {'B2DstD': d2x+dstar}
        b2dstd_rs = makeB2XMerged('B2DstD',decays,'',inputs,
                                  self.config)
        decays = {'B2DstDWS':["[B0 -> D*(2010)- D-]cc"]}
        inputs = {'B2DstDWS': d2x+dstar}
        b2dstd_ws = makeB2XMerged('B2DstDWS',decays,'',inputs,
                                  self.config)
        self.lines.append(ProtoLine(b2dstd_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b2dstd_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b2dstd_ws['TOS'],0.1))

    # SB ------------------
    
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
