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
        self.d0h_d02pi0hh_resolved = self._makeB2D0H('D2Pi0HH_Resolved',self.d.pi0hh_resolved)
        self.d0h_d02pi0hh_merged = self._makeB2D0H('D2Pi0HH_Merged',self.d.pi0hh_merged)
        self.dh_d2pi0hhh_resolved = self._makeB02DH('D2Pi0HHH_Resolved',self.d.pi0hhh_resolved)
        self.dh_d2pi0hhh_merged = self._makeB02DH('D2Pi0HHH_Merged',self.d.pi0hhh_merged) 
        self.d0h_d02hhhh = self._makeB2D0H('D2HHHH',self.d.hhhh)
        self.dstdk = self._makeB2DstDK()

    def selections(self):
        '''Returns all selections that are to be made into lines.'''
        sels = []
        # B->D0H, D0->HH
        sels.append({'Selection': self.d0h_d02hh['RS']['TOS'],
                     'PrescaleKey': 'D2HH'})
        sels.append({'Selection': self.d0h_d02hh['RS']['TIS'],
                     'PrescaleKey': 'D2HHTIS'})
        # B->DH, D->HHH
        sels.append({'Selection': self.dh_d2hhh['RS']['TOS'],
                     'PrescaleKey': 'D2HHH'})
        sels.append({'Selection': self.dh_d2hhh['RS']['TIS'],
                     'PrescaleKey': 'D2HHHTIS'})
        sels.append({'Selection': self.dh_d2hhh['WS']['TOS'],
                     'PrescaleKey': 'D2HHHWS'})
        # B->DH, D->KSH, KS->LL
        sels.append({'Selection': self.dh_d2ksh_ll['RS']['TOS'],
                     'PrescaleKey': 'D2KSHLL'})
        sels.append({'Selection': self.dh_d2ksh_ll['RS']['TIS'],
                     'PrescaleKey': 'D2KSHLLTIS'})
        sels.append({'Selection': self.dh_d2ksh_ll['WS']['TOS'],
                     'PrescaleKey': 'D2KSHLLWS'})
        # B->DH, D->KSH, KS->DD
        sels.append({'Selection': self.dh_d2ksh_dd['RS']['TOS'],
                     'PrescaleKey': 'D2KSHDD'})
        sels.append({'Selection': self.dh_d2ksh_dd['RS']['TIS'],
                     'PrescaleKey': 'D2KSHDDTIS'})
        sels.append({'Selection': self.dh_d2ksh_dd['WS']['TOS'],
                     'PrescaleKey': 'D2KSHDDWS'})
        # B->DH, D->KSHH, KS->LL
        sels.append({'Selection': self.d0h_d02kshh_ll['RS']['TOS'],
                     'PrescaleKey': 'D2KSHHLL'})
        sels.append({'Selection': self.d0h_d02kshh_ll['RS']['TIS'],
                     'PrescaleKey': 'D2KSHHLLTIS'})
        # B->DH, D->KSHH, KS->DD
        sels.append({'Selection': self.d0h_d02kshh_dd['RS']['TOS'],
                     'PrescaleKey': 'D2KSHHDD'})
        sels.append({'Selection': self.d0h_d02kshh_dd['RS']['TIS'],
                     'PrescaleKey': 'D2KSHHDDTIS'})
        # B->DH, D->Pi0HHH, Pi0->Resolved
        sels.append({'Selection': self.dh_d2pi0hhh_resolved['RS']['TOS'],
                     'PrescaleKey': 'D2Pi0HHHResolved'})
        sels.append({'Selection': self.dh_d2pi0hhh_resolved['RS']['TIS'],
                     'PrescaleKey': 'D2Pi0HHHResolvedTIS'})
        sels.append({'Selection': self.dh_d2pi0hhh_resolved['WS']['TOS'],
                     'PrescaleKey': 'D2Pi0HHHResolvedWS'})
        # B->DH, D->Pi0HHH, Pi0->Merged
        sels.append({'Selection': self.dh_d2pi0hhh_merged['RS']['TOS'],
                     'PrescaleKey': 'D2Pi0HHHMerged'})
        sels.append({'Selection': self.dh_d2pi0hhh_merged['RS']['TIS'],
                     'PrescaleKey': 'D2Pi0HHHMergedTIS'})
        sels.append({'Selection': self.dh_d2pi0hhh_merged['WS']['TOS'],
                     'PrescaleKey': 'D2Pi0HHHMergedWS'})
        # B->DH, D->Pi0HH, Pi0->Resolved
        sels.append({'Selection': self.d0h_d02pi0hh_resolved['RS']['TOS'],
                     'PrescaleKey': 'D2Pi0HHResolved'})
        sels.append({'Selection': self.d0h_d02pi0hh_resolved['RS']['TIS'],
                     'PrescaleKey': 'D2Pi0HHResolvedTIS'})
        # B->DH, D->Pi0HH, Pi0->Merged
        sels.append({'Selection': self.d0h_d02pi0hh_merged['RS']['TOS'],
                     'PrescaleKey': 'D2Pi0HHMerged'})
        sels.append({'Selection': self.d0h_d02pi0hh_merged['RS']['TIS'],
                     'PrescaleKey': 'D2Pi0HHMergedTIS'})
        # B->D0H, D0-HHHH
        sels.append({'Selection': self.d0h_d02hhhh['RS']['TOS'],
                     'PrescaleKey': 'D2HHHH'})
        sels.append({'Selection': self.d0h_d02hhhh['RS']['TIS'],
                     'PrescaleKey': 'D2HHHHTIS'})
        # B->D*DK
        sels.append({'Selection': self.dstdk['RS']['TOS'],
                     'PrescaleKey': 'D2HHH'})
        sels.append({'Selection': self.dstdk['RS']['TIS'],
                     'PrescaleKey': 'D2HHHTIS'})
        sels.append({'Selection': self.dstdk['WS']['TOS'],
                     'PrescaleKey': 'D2HHHWS'})

        return sels
           
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
