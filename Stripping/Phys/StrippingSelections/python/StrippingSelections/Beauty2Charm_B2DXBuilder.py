#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class B2DXBuilder(object):
    '''Makes all B->DX decays for the Beauty2Charm module.'''

    def __init__(self,d,dst,topoPions,topoKaons,hh,config):
        self.config = config
        self.topoPions = topoPions
        self.topoKaons = topoKaons
        self.d = d
        self.dst = dst
        self.hh = hh
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
        # B -> D0(HHHH) X
        self._makeB2D0H('D2HHHH',self.d.hhhh) # B+- -> D0(HHHH) H+- 
        # B -> D D X 
        self._makeB2DstDK() # B+- -> D*(D0(HH)Pi)+-D-+(HHH)K+- (+WS)
        # B+- -> D0(Pi0HH) H+- (resolved & merged)
        self._makeB2D0H('D2Pi0HH_Resolved',self.d.pi0hh_resolved)
        self._makeB2D0H('D2Pi0HH_Merged',self.d.pi0hh_merged)
        # B0 -> D+-(Pi0HHH) H-+  (resolved & merged) (+WS)
        self._makeB02DH('D2Pi0HHH_Resolved',self.d.pi0hhh_resolved)
        self._makeB02DH('D2Pi0HHH_Merged',self.d.pi0hhh_merged) 
         
    def _makeB02DH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'B02DPi': ["[B0 -> D- pi+]cc"], 'B02DK': ["[B0 -> D- K+]cc"]}
        inputs = {'B02DPi': [d2x,pions], 'B02DK': [d2x,kaons]}
        b02dh_rs = makeB2XMerged('B02DH',decays,dname,inputs,self.config)
        decays = {'B02DPiWS': ["[B0 -> D- pi-]cc"],
                  'B02DKWS': ["[B0 -> D- K-]cc"]}
        inputs = {'B02DPiWS': [d2x,pions], 'B02DKWS': [d2x,kaons]}
        b02dh_ws = makeB2XMerged('B02DHWS',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dh_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b02dh_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b02dh_ws['TOS'],0.1))
   
    def _makeB02DHH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- h0 + c.c.'''
        decays = {'B02DPiPi0': ["[B0 -> D- rho(770)+]cc"],
                  'B02DKsPi' : ["[B0 -> D- K*(892)+]cc"],
                  'B02DKPi0' : ["[B0 -> D- K*(892)+]cc"]}
        inputs = {'B02DPiPi0': [d2x,self.hh.pipi0],
                  'B02DKsPi' : [d2x,self.hh.kspi],
                  'B02DKPi0'  : [d2x,self.hh.kpi0]}
        b02dhh_rs = makeB2XMerged('B02DHH',decays,dname,inputs,self.config)
        decays = {'B02DPiPi0WS': ["[B0 -> D- rho(770)-]cc"],
                  'B02DKsPiWS' : ["[B0 -> D- K*(892)-]cc"],
                  'B02DKPi0WS' : ["[B0 -> D- K*(892)-]cc"]}
        inputs = {'B02DPiPi0WS': [d2x,self.hh.pipi0],
                  'B02DKsPiWS' : [d2x,self.hh.kspi],
                  'B02DKPi0WS' : [d2x,self.hh.kpi0]}
        b02dhh_ws = makeB2XMerged('B02DHHWS',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dhh_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b02dhh_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b02dhh_ws['TOS'],0.1))
 
    def _makeB2D0H(self,dname,d2x):
        '''Makes RS B+ -> D0 h+ (h=pi,K) + c.c.'''
        decays = {'B2D0Pi': ["B+ -> D0 pi+","B- -> D0 pi-"],
                  'B2D0K' : ["B+ -> D0 K+","B- -> D0 K-"]}
        inputs = {'B2D0Pi': [d2x,self.topoPions],'B2D0K': [d2x,self.topoKaons]}
        b2d0h = makeB2XMerged('B2D0H',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0h['TOS'],1.0))
        self.lines.append(ProtoLine(b2d0h['TIS'],1.0))

    def _makeB02D0HH(self,dname,d2x):
        '''Makes RS B0 -> D0 h+h- (h=pi,K) + c.c.'''
        decays = {'B02D0PiPi': ["B0 -> D0 rho(770)0"],
                  'B02D0KPi' : ["B0 -> D0 K*(892)0","B0 -> D0 K*(892)~0"],
                  'B02D0KK'  : ["B0 -> D0 phi(1020)"]}
        inputs = {'B02D0PiPi': [d2x,self.hh.pipi],
                  'B02D0KPi' : [d2x,self.hh.kpi],
                  'B02D0KK'  : [d2x,self.hh.kk]}
        b2d0hh = makeB2XMerged('B02D0HH',decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0hh['TOS'],1.0))
        self.lines.append(ProtoLine(b2d0hh['TIS'],1.0))

    def _makeB2DstDK(self):
        '''Makes the RS and WS B+ -> D*+- D-+ K+ + c.c.'''
        decays = {'B2DstDK':["[B+ -> D*(2010)+ D- K+]cc",
                             "[B+ -> D*(2010)- D+ K+]cc"]}
        inputs = {'B2DstDK':[self.dst.d0pi,self.d.hhh,self.topoKaons]}
        b2dstdk_rs = makeB2XMerged('B2DstDK',decays,'Dst2D0PiD2HHH',inputs,
                                   self.config)
        decays = {'B2DstDKWS':["[B+ -> D*(2010)+ D+ K+]cc"]}
        inputs = {'B2DstDKWS':[self.dst.d0pi,self.d.hhh,self.topoKaons]}
        b2dstdk_ws = makeB2XMerged('B2DstDKWS',decays,'Dst2D0PiD2HHH',
                                   inputs,self.config)
        self.lines.append(ProtoLine(b2dstdk_rs['TOS'],1.0))
        self.lines.append(ProtoLine(b2dstdk_rs['TIS'],1.0))
        self.lines.append(ProtoLine(b2dstdk_ws['TOS'],0.1))

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
