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

    def __init__(self,d,dst,topoPions,topoKaons,config):
        self.config = config
        self.topoPions = topoPions
        self.topoKaons = topoKaons
        self.d = d
        self.dst = dst
        self.d0h_d02hh = self._makeB2D0H('D2HH',self.d.hh)
        self.dh_d2hhh = self._makeB02DH('D2HHH',self.d.hhh)
        self.dh_d2ksh_ll = self._makeB02DH('D2KSH_LL',self.d.ksh_ll)
        self.dh_d2ksh_dd = self._makeB02DH('D2KSH_DD',self.d.ksh_dd)
        self.d0h_d02kshh_ll = self._makeB2D0H('D2KSHH_LL',self.d.kshh_ll)
        self.d0h_d02kshh_dd = self._makeB2D0H('D2KSHH_DD',self.d.kshh_dd)
        self.d0h_d02pi0hh_resolved = self._makeB2D0H('D2Pi0HH_Resolved',
                                                     self.d.pi0hh_resolved)
        self.d0h_d02pi0hh_merged = self._makeB2D0H('D2Pi0HH_Merged',
                                                   self.d.pi0hh_merged)
        self.dh_d2pi0hhh_resolved = self._makeB02DH('D2Pi0HHH_Resolved',
                                                    self.d.pi0hhh_resolved)
        self.dh_d2pi0hhh_merged = self._makeB02DH('D2Pi0HHH_Merged',
                                                  self.d.pi0hhh_merged) 
        self.d0h_d02hhhh = self._makeB2D0H('D2HHHH',self.d.hhhh)
        self.dstdk = self._makeB2DstDK()

    def lines(self):
        '''Returns all proto-lines that are to be made into lines.'''
        lines = []
        
        # B->D0H, D0->HH
        lines.append(ProtoLine(self.d0h_d02hh['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.d0h_d02hh['RS']['TIS'],1.0))
        # B->DH, D->HHH
        lines.append(ProtoLine(self.dh_d2hhh['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.dh_d2hhh['RS']['TIS'],1.0))
        lines.append(ProtoLine(self.dh_d2hhh['WS']['TOS'],0.1))
        # B->DH, D->KSH, KS->LL
        lines.append(ProtoLine(self.dh_d2ksh_ll['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.dh_d2ksh_ll['RS']['TIS'],1.0))
        lines.append(ProtoLine(self.dh_d2ksh_ll['WS']['TOS'],0.1))
        # B->DH, D->KSH, KS->DD
        lines.append(ProtoLine(self.dh_d2ksh_dd['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.dh_d2ksh_dd['RS']['TIS'],1.0))
        lines.append(ProtoLine(self.dh_d2ksh_dd['WS']['TOS'],0.1))
        # B->DH, D->KSHH, KS->LL
        lines.append(ProtoLine(self.d0h_d02kshh_ll['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.d0h_d02kshh_ll['RS']['TIS'],1.0))
        # B->DH, D->KSHH, KS->DD
        lines.append(ProtoLine(self.d0h_d02kshh_dd['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.d0h_d02kshh_dd['RS']['TIS'],1.0))
        # B->DH, D->Pi0HHH, Pi0->Resolved
        lines.append(ProtoLine(self.dh_d2pi0hhh_resolved['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.dh_d2pi0hhh_resolved['RS']['TIS'],1.0))
        lines.append(ProtoLine(self.dh_d2pi0hhh_resolved['WS']['TOS'],0.1))
        # B->DH, D->Pi0HHH, Pi0->Merged
        lines.append(ProtoLine(self.dh_d2pi0hhh_merged['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.dh_d2pi0hhh_merged['RS']['TIS'],1.0))
        lines.append(ProtoLine(self.dh_d2pi0hhh_merged['WS']['TOS'],0.1))
        # B->DH, D->Pi0HH, Pi0->Resolved
        lines.append(ProtoLine(self.d0h_d02pi0hh_resolved['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.d0h_d02pi0hh_resolved['RS']['TIS'],1.0))
        # B->DH, D->Pi0HH, Pi0->Merged
        lines.append(ProtoLine(self.d0h_d02pi0hh_merged['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.d0h_d02pi0hh_merged['RS']['TIS'],1.0))
        # B->D0H, D0-HHHH
        lines.append(ProtoLine(self.d0h_d02hhhh['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.d0h_d02hhhh['RS']['TIS'],1.0))
        # B->D*DK
        lines.append(ProtoLine(self.dstdk['RS']['TOS'],1.0))
        lines.append(ProtoLine(self.dstdk['RS']['TIS'],1.0))
        lines.append(ProtoLine(self.dstdk['WS']['TOS'],0.1))

        return lines
           
    def _makeB02DH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        b02dh = {}
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {'B02DPi': ["[B0 -> D- pi+]cc"], 'B02DK': ["[B0 -> D- K+]cc"]}
        inputs = {'B02DPi': [d2x,pions], 'B02DK': [d2x,kaons]}
        b02dh['RS'] = makeB2XMerged('B02DH',decays,dname,inputs,self.config)
        decays = {'B02DPiWS': ["[B0 -> D- pi-]cc"],
                  'B02DKWS': ["[B0 -> D- K-]cc"]}
        inputs = {'B02DPiWS': [d2x,pions], 'B02DKWS': [d2x,kaons]}
        b02dh['WS'] = makeB2XMerged('B02DHWS',decays,dname,inputs,self.config)
        return b02dh
    
    def _makeB2D0H(self,dname,d2x):
        '''Makes RS B+ -> D0 h+ (h=pi,K) + c.c.'''
        decays = {'B2D0Pi': ["[B+ -> D~0 pi+]cc","[B+ -> D0 pi+]cc"],
                  'B2D0K' : ["[B+ -> D~0 K+]cc","[B+ -> D0 K+]cc"]}
        inputs = {'B2D0Pi': [d2x,self.topoPions],'B2D0K': [d2x,self.topoKaons]}
        b2d0h = makeB2XMerged('B2D0H',decays,dname,inputs,self.config)
        return {'RS':b2d0h,'WS':None}

    def _makeB2DstDK(self):
        '''Makes the RS and WS B+ -> D*+- D-+ K+ + c.c.'''
        b2dstdk = {}
        decays = {'B2DstDK':["[B+ -> D*(2010)+ D- K+]cc",
                             "[B+ -> D*(2010)- D+ K+]cc"]}
        inputs = {'B2DstDK':[self.dst.d0pi,self.d.hhh,self.topoKaons]}
        b2dstdk['RS'] = makeB2XMerged('B2DstDK',decays,'Dst2D0PiD2HHH',inputs,
                                      self.config)
        decays = {'B2DstDKWS':["[B+ -> D*(2010)+ D+ K+]cc"]}
        inputs = {'B2DstDKWS':[self.dst.d0pi,self.d.hhh,self.topoKaons]}
        b2dstdk['WS'] = makeB2XMerged('B2DstDKWS',decays,'Dst2D0PiD2HHH',
                                      inputs,self.config)
        return b2dstdk

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
