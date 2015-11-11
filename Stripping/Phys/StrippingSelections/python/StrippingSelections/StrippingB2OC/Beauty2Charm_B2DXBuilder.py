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

    def __init__(self,d,dst,topoPions,topoPionsLoose,topoKaons,muons,ks,pi0,hh,hhh,config):
        self.config = config
        self.topoPions = [topoPions]
        self.topoPionsLoose = [topoPionsLoose]
        self.topoKaons = [topoKaons]
        self.muons = muons
        self.d = d
        self.dst = dst
        self.hh = hh
        self.hhh = hhh
        self.ks = ks
        self.pi0 = pi0
        self.lines = []        
        
        # B -> D0(HH) X
        self._makeB2D0H('D2HH',self.d.hh)   # B+- -> D0(HH)  H+-
        self._makeB2D0H('D2HHWS',self.d.hh_ws)
        self.lines[-1].pre = 0.1 # last line is WS D line
        self._makeB2D0H('D2HHUP',self.d.hh_up)   # B+- -> D0(HH)  H+-
        self._makeB2D0H('D2KPIPID',d.kpi_pid,False) # No IP line!
        self._makeB02D0HH('D2HH',self.d.hh) # B0  -> D0(HH)  H+ H-
        self._makeB02D0PPbar('D2HH',self.d.hh) # B0  -> D0(HH)  H+ H-

        #Tighter selection Full DST lines
        # self._makeB02D0HH_FULLDST('D2HHFULLDST',self.d.hh_pid_tight)
        #self._makeB02D0PPbar_FULLDST('D2HHFULLDST',self.d.hh_pid_tight) 

        self._makeB02D0HH('D2HHWS',self.d.hh_ws) # B0  -> D0(HH)WS  H+ H-
        self.lines[-2].pre = 0.1 # WS D line
        self._makeB02D0KS('D2HH',self.d.hh,'DD') # B0  -> D0(HH)  KS
        self._makeB02D0KS('D2HH',self.d.hh,'LL') # B0  -> D0(HH)  KS
        self._makeB2D0HH('D2HH',self.d.hh) # B+- -> D0(HH) H+- H0
        self._makeB2D0HH('D2KSHHLL',self.d.kshh_ll) # B+- -> D0(Kshh,LL) H+- H0
        self._makeB2D0HH('D2KSHHDD',self.d.kshh_dd) # B+- -> D0(Kshh,DD) H+- H0
        self._makeB2D0HH('D2HHHH',self.d.hhhh) # B+- -> D0(HHHH) H+- H0
        self._makeB2D0HH('D2HHHHWS',self.d.hhhh_ws) # B+- -> D0(HHHH,WS) H+- H0
        self._makeB02DHD2PhiMu('D2PhiMuNu',self.d.phimu) # B0 -> D( phi mu nu) H+-
        # B -> D+-(HHH) X
        self._makeB02DH('D2HHH',self.d.hhh) # B0  -> D+-(HHH) H-+   (+WS)
        self._makeB02DH('D2HHHUP',self.d.hhh_up) # B0  -> D+-(HHH) H-+   (+WS)
        self._makeB02DH('D2HHHCFPID',self.d.hhh_cf_pid,False) # No IP line!
        self._makeB02DH('Ds2HHHPID',self.d.ds_hhh_pid_tight,False) # No IP line!
        self._makeB02DHH('D2HHHCFPID',self.d.hhh_cf_pid)# B0 -> D+- H-+ H0 (+WS)
        self._makeB2DHH('D2HHHCFPID',self.d.hhh_cf_pid)#B+- ->D+-(HHH)H+H-(+WS)
        self._makeB2DV('D2HHHPID',self.d.hhh_pid)#B+- ->D+-(HHH) V(HH)
        self._makeB2DKS('D2HHH',self.d.hhh,'DD') # B+-  -> D+-(HHH)  KS
        self._makeB2DKS('D2HHH',self.d.hhh,'LL') # B+-  -> D+-(HHH)  KS
        self._makeB02DV('D2HHHCFPID',self.d.hhh_cf_pid)
        # B -> D(KSH[H]) X (LL & DD)
        self._makeB02DH('D2KSHLL',self.d.ksh_ll) # B0  -> D+-(KSLLH) H-+ (+WS)
        self._makeB02DH('D2KSHDD',self.d.ksh_dd) # B0  -> D+-(KSDDH) H-+ (+WS)
        self._makeB2D0H('D2KSHHLL',self.d.kshh_ll)# B+- -> D0(KSLLHH) H+-
        self._makeB2D0H('D2KSHHDD',self.d.kshh_dd)# B+- -> D0(KSDDHH) H+-
        self._makeB2D0H('D2KSHHLLold',self.d.kshh_ll_old)# B+- -> D0(KSLLHH) H+-
        self._makeB2D0H('D2KSHHDDold',self.d.kshh_dd_old)# B+- -> D0(KSDDHH) H+- 
        self._makeB2D0MuNu('D2KSHHLL',self.d.kshh_ll)# B+- -> D0(KSLLHH) Mu+- Nu
        self._makeB2D0MuNu('D2KSHHDD',self.d.kshh_dd)# B+- -> D0(KSDDHH) Mu+- Nu

        #self._makeB02DH('D2KSHLLUP',self.d.ksh_ll_up) # B0  -> D+-(KSLLH) H-+ (+WS)
        #self._makeB02DH('D2KSHDDUP',self.d.ksh_dd_up) # B0  -> D+-(KSDDH) H-+ (+WS)
        #self._makeB2D0H('D2KSHHLLUP',self.d.kshh_ll_up)# B+- -> D0(KSLLHH) H+-
        #self._makeB2D0H('D2KSHHDDUP',self.d.kshh_dd_up)# B+- -> D0(KSDDHH) H+-
        self._makeB02D0HH('D2KSHHLL',self.d.kshh_ll) # B0  -> D0(KSLLHH)H+H-
        self._makeB02D0HH('D2KSHHDD',self.d.kshh_dd) # B0  -> D0(KSDDHH)H+H-
        self._makeB02D0HH('D2KSHHLLWS',self.d.kshh_ll_ws, 0.1) # B0  -> D0(KSLLHH)H+H- (WS D daughters)
        self._makeB02D0HH('D2KSHHDDWS',self.d.kshh_dd_ws, 0.1) # B0  -> D0(KSDDHH)H+H- (WS D daughters)
        
        self._makeB2D0H('D2KSHHLLWS',self.d.kshh_ll_ws)
        self.lines[-1].pre = 0.1 # last line is WS D line
        self._makeB2D0H('D2KSHHDDWS',self.d.kshh_dd_ws)
        self.lines[-1].pre = 0.1 # last line is WS D line
        self._makeB02DstH('Dst2D0Pi_D2KSHHLL',self.dst.d0pi_kshh_ll)
        self._makeB02DstH('Dst2D0Pi_D2KSHHDD',self.dst.d0pi_kshh_dd)
        self._makeB02DstMuNu('Dst2D0Pi_D2KSHHLL',self.dst.d0pi_kshh_ll)
        self._makeB02DstMuNu('Dst2D0Pi_D2KSHHDD',self.dst.d0pi_kshh_dd)
        self._makeB02DstMuNu('Dst2D0Pi_D2HHHH',self.dst.d0pi_hhhh)
        self._makeB02DstMuNu('Dst2D0Pi_D2HHHHWS',self.dst.d0pi_hhhh_ws)

        self._makeB02DstKsH('Dst2D0Pi', self.dst.d0pi)
        # B -> D0(HHHH) X
        self._makeB2D0H('D2HHHH',self.d.hhhh) # B+- -> D0(HHHH) H+-
        self._makeB2D0H('D2HHHHold',self.d.hhhh_old) # B+- -> D0(HHHH) H+-
        self._makeB2D0H('D2HHHHWS',self.d.hhhh_ws)
        self.lines[-1].pre = 0.1 # last line is WS D line
        self._makeB2D0H('D2HHHHUP',self.d.hhhh_up) # B+- -> D0(HHHH) H+-
        self._makeB02D0HH('D2HHHH',self.d.hhhh) # B0  -> D0(K3Pi)  H+ H-
        self._makeB02D0HH('D2HHHHWS',self.d.hhhh_ws) # B0  -> D0(HHHH)WS  H+ H-
        self.lines[-2].pre = 0.1 # WS D line
        self._makeB02D0KS('D2K3Pi',self.d.k3pi,'DD') # B0  -> D0(K3Pi)  KS
        self._makeB02D0KS('D2K3Pi',self.d.k3pi,'LL') # B0  -> D0(K3Pi)  KS
        # B+- -> D0(Pi0HH) X (resolved & merged)
        self._makeB2D0H('D2Pi0HHResolved',self.d.pi0hh_resolved)
        self._makeB2D0H('D2Pi0HHResolvedold',self.d.pi0hh_resolved_old)
        self._makeB2D0H('D2Pi0HHWSResolved',self.d.pi0hh_resolved_ws)
        self.lines[-1].pre = 0.1 # last line is WS D line
        #self._makeB2D0H('D2Pi0HHResolvedUP',self.d.pi0hh_resolved_up)
        self._makeB2D0H('D2Pi0HHMerged',self.d.pi0hh_merged)
        self._makeB2D0H('D2Pi0HHMergedold',self.d.pi0hh_merged_old)
        self._makeB2D0H('D2Pi0HHWSMerged',self.d.pi0hh_merged_ws)
        self.lines[-1].pre = 0.1 # last line is WS D line
        #self._makeB2D0H('D2Pi0HHMergedUP',self.d.pi0hh_merged_up)

        # B+- -> D0(KSPi0HH) X (resolved & merged & LL & DD)
        self._makeB2D0H('D2KSPi0HHLLResolved',self.d.kspi0hh_ll_resolved)
        self._makeB2D0H('D2KSPi0HHLLMerged',self.d.kspi0hh_ll_merged)
        self._makeB2D0H('D2KSPi0HHDDResolved',self.d.kspi0hh_dd_resolved)
        self._makeB2D0H('D2KSPi0HHDDMerged',self.d.kspi0hh_dd_merged)
        
        self._makeB02D0HH('D2Pi0HHResolved',self.d.pi0hh_resolved)
        self._makeB02D0HH('D2Pi0HHMerged',self.d.pi0hh_merged)
        #self._makeB02D0KS('D2Pi0KPiResolved',self.d.pi0kpi_resolved,'DD')
        #self._makeB02D0KS('D2Pi0KPiMerged',self.d.pi0kpi_merged,'LL') 
        # B0 -> D+-(Pi0HHH) H-+  (resolved & merged) (+WS)
        self._makeB02DH('D2Pi0HHHResolved',self.d.pi0hhh_resolved)
        self._makeB02DH('D2Pi0HHHMerged',self.d.pi0hhh_merged)
        #self._makeB02DH('D2Pi0HHHResolvedUP',self.d.pi0hhh_resolved_up)
        #self._makeB02DH('D2Pi0HHHMergedUP',self.d.pi0hhh_merged_up) 
        # B -> D0(HH) 3h
        self._makeB2D0HHH('D2HHPID',self.d.hh_pid)   # B+- -> D0(HH)  H+H-H+
        self._makeB2D0HHH('D2KSHHDD',self.d.kshh_dd)
        self._makeB2D0HHH('D2KSHHLL',self.d.kshh_ll)
        #lines going to full dst: B+- -> D0(Kpi)3pi and B+- -> D0(K3pi)pi
        self._makeB2D0HHH_FULLDST('D2HHTIGHT',self.d.kpi_pid_tighter1_narrow)
        self._makeB2D0Pi('D2HHHHTIGHT',self.d.k3pi_pid_tighter1_narrow)
        
        # B -> D(HHH) 3h
        self._makeB02DHHH('D2HHHPID',self.d.hhh_pid)   # B+- -> D(HHH)  H+H-H+
        #Tighter selection Full DST lines
        #self._makeB02DHHH_FULLDST('D2HHHFULLDST',self.d.ds_hhh_pid_custom)

        self._makeB2D0ppbarH('D2HH', self.d.hh)
        self._makeB02DppbarH('D2HHH', self.d.hhh)
        
        # B+- -> D0(HH)  H+H-H+
        self._makeB02DstHHH('Dstar2D0PiPID',self.dst.d0pi_pid)
        # B -> D D 
        self._makeB02DD()
        #Tighter selection Full DST lines
        #self._makeB02DD_FULLDST()

        self._makeB2D0D()   
        self._makeB02DstD()
        self._makeB02DstDD02K3Pi()
        self._makeB02D0D0()
        #Tighter selection Full DST lines
        #self._makeB02D0D0_FULLDST()

        # Jordi: make [ B -> D*0 ( D0 pi0 ) h ] and [ B -> D*0 ( D0 gamma ) h ] lines for LL and DD.
        self._makeB2Dst0H( 'Dst02D0Pi0D2KSHHLL'  , self.dst.d0pi0_kshh_ll   )
        self._makeB2Dst0H( 'Dst02D0Pi0D2KSHHDD'  , self.dst.d0pi0_kshh_dd   )
        self._makeB2Dst0H( 'Dst02D0GammaD2KSHHLL', self.dst.d0gamma_kshh_ll )
        self._makeB2Dst0H( 'Dst02D0GammaD2KSHHDD', self.dst.d0gamma_kshh_dd )

        self._makeB2DstD0()
        self._makeB02DstDst()
        # B -> D D K
        self._makeB2DDK()   
        self._makeB02D0DK() 
        self._makeB2DstDK() 
        self._makeB2D0D0K()
        self._makeB02DstD0K()
        self._makeB2DstDstK()
        self._makeB2Dst2460DK()
        self._makeB2D0st2460D0K()
        # B -> D D K*
        self._makeB02D0D0Kst()
        self._makeB02DDKst()
        self._makeB02DstDKst()
        self._makeB02DstDstKst()
        self._makeB2DD0Kst()
        self._makeB2DstD0Kst()
        # B -> D D KS
        self._makeB02DDKS('DD')   
        self._makeB2D0DKS('DD') 
        self._makeB02DstDKS('DD') 
        self._makeB02D0D0KS('DD')
        self._makeB2DstD0KS('DD')
        self._makeB02DstDstKS('DD')
        self._makeB02DDKS('LL')   
        self._makeB2D0DKS('LL') 
        self._makeB02DstDKS('LL') 
        self._makeB02D0D0KS('LL')
        self._makeB2DstD0KS('LL')
        self._makeB02DstDstKS('LL')
        # B->DDKK
        self._makeB2D0DKK()
        self._makeB2D0DKPi()
        self._makeB02DDKK()        
        self._makeB02DstDKK()
        self._makeB02DstDstKK()
 
    def _makeB02DH(self,dname,d2x,useIP=True):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        tag = 'B02D%s'
        if not useIP: tag += 'NoIP'
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {tag%'Pi': ["[B0 -> D- pi+]cc"]}
        inputs = {tag%'Pi': d2x+pions}
        if useIP:
            decays[tag%'K'] = ["[B0 -> D- K+]cc"]
            inputs[tag%'K'] = d2x+kaons
        b02dh_rs = makeB2XSels(decays,dname,inputs,self.config,useIP)        
        decays = {tag%'PiWS': ["[B0 -> D- pi-]cc"]}
        inputs = {tag%'PiWS': d2x+pions}
        if useIP:
            decays[tag%'KWS'] = ["[B0 -> D- K-]cc"]
            inputs[tag%'KWS'] = d2x+kaons
        b02dh_ws = makeB2XSels(decays,dname,inputs,self.config,useIP)
        self.lines.append(ProtoLine(b02dh_rs,1.0))
        self.lines.append(ProtoLine(b02dh_ws,0.1))

    def _makeB02DHH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- h0 + c.c.'''
        config = deepcopy(self.config)
        config['AM_MAX' ] = '5800*MeV'
        decays = {'B02DPiPi0Merged': ["[B0 -> D- pi+ pi0]cc"],
                  'B02DPiPi0Resolved': ["[B0 -> D- pi+ pi0]cc"],
                  'B02DKsPiDD' : ["[B0 -> D- pi+ KS0]cc"],
                  'B02DKsPiLL' : ["[B0 -> D- pi+ KS0]cc"],
                  'B02DKsKDD' : ["[B0 -> D- K+ KS0]cc"],
                  'B02DKsKLL' : ["[B0 -> D- K+ KS0]cc"],
                  'B02DKPi0Merged' : ["[B0 -> D- K+ pi0]cc"],
                  'B02DKPi0Resolved' : ["[B0 -> D- K+ pi0]cc"]}
        inputs = {'B02DPiPi0Merged': d2x+self.topoPions+self.pi0['Merged'],
                  'B02DPiPi0Resolved': d2x+self.topoPions+self.pi0['Resolved'],
                  'B02DKsPiDD' : d2x+self.topoPions+self.ks['DD'],
                  'B02DKsPiLL' : d2x+self.topoPions+self.ks['LL'],
                  'B02DKsKDD' : d2x+self.topoKaons+self.ks['DD'],
                  'B02DKsKLL' : d2x+self.topoKaons+self.ks['LL'],
                  'B02DKPi0Merged' : d2x+self.topoKaons+self.pi0['Merged'],
                  'B02DKPi0Resolved' : d2x+self.topoKaons+self.pi0['Resolved']}
        b02dhh_rs = makeB2XSels(decays,dname,inputs,config)
        decays = {'B02DPiPi0MergedWS': ["[B0 -> D- pi- pi0]cc"],
                  'B02DPiPi0ResolvedWS': ["[B0 -> D- pi- pi0]cc"],
                  'B02DKsPiDDWS' : ["[B0 -> D- pi- KS0]cc"],
                  'B02DKsPiLLWS' : ["[B0 -> D- pi- KS0]cc"],
                  'B02DKsKDDWS' : ["[B0 -> D- K- KS0]cc"],
                  'B02DKsKLLWS' : ["[B0 -> D- K- KS0]cc"],
                  'B02DKPi0MergedWS' : ["[B0 -> D- K- pi0]cc"],
                  'B02DKPi0ResolvedWS' : ["[B0 -> D- K- pi0]cc"]}
        inputs = {'B02DPiPi0MergedWS': d2x+self.topoPions+self.pi0['Merged'],
                  'B02DPiPi0ResolvedWS':d2x+self.topoPions\
                  +self.pi0['Resolved'],
                  'B02DKsPiDDWS' : d2x+self.topoPions+self.ks['DD'],
                  'B02DKsPiLLWS' : d2x+self.topoPions+self.ks['LL'],
                  'B02DKsKDDWS' : d2x+self.topoKaons+self.ks['DD'],
                  'B02DKsKLLWS' : d2x+self.topoKaons+self.ks['LL'],
                  'B02DKPi0MergedWS' : d2x+self.topoKaons+self.pi0['Merged'],
                  'B02DKPi0ResolvedWS' : d2x+self.topoKaons\
                  +self.pi0['Resolved']}
        b02dhh_ws = makeB2XSels(decays,dname,inputs,config)
        self.lines.append(ProtoLine(b02dhh_rs,1.0))
        self.lines.append(ProtoLine(b02dhh_ws,0.1))

    def _makeB2D0HH(self,dname,d2x):
        '''Makes B+- -> D0 h+- h0'''
        config = deepcopy(self.config)
        config['AM_MAX' ] = '5800*MeV'
        decays = {'B2D0PiPi0Merged': ["B+ -> D0 pi+ pi0","B- -> D0 pi- pi0"],
                  'B2D0PiPi0Resolved': ["B+ -> D0 pi+ pi0","B- -> D0 pi- pi0"],
                  'B2D0KsPiDD' : ["B+ -> D0 pi+ KS0","B- -> D0 pi- KS0"],
                  'B2D0KsPiLL' : ["B+ -> D0 pi+ KS0","B- -> D0 pi- KS0"],
                  'B2D0KPi0Merged' : ["B+ -> D0 K+ pi0","B- -> D0 K- pi0"],
                  'B2D0KPi0Resolved' : ["B+ -> D0 K+ pi0","B- -> D0 K- pi0"]}
        inputs = {'B2D0PiPi0Merged': d2x+self.topoPions+self.pi0['Merged'],
                  'B2D0PiPi0Resolved': d2x+self.topoPions+self.pi0['Resolved'],
                  'B2D0KsPiDD' : d2x+self.topoPionsLoose+self.ks['DD'],
                  'B2D0KsPiLL' : d2x+self.topoPionsLoose+self.ks['LL'],
                  'B2D0KPi0Merged' : d2x+self.topoKaons+self.pi0['Merged'],
                  'B2D0KPi0Resolved' : d2x+self.topoKaons+self.pi0['Resolved']}
        b2d0hh = makeB2XSels(decays,dname,inputs,config)
        self.lines.append(ProtoLine(b2d0hh,1.0))

    #def _makeB02DHH(self,dname,d2x):
    #    '''Makes RS and WS B0 -> D + h- h0 + c.c.'''
    #    config = deepcopy(self.config)
    #    config['AM_MAX' ] = '5800*MeV'
    #    decays = {'B02DPiPi0': ["[B0 -> D- rho(770)+]cc"],
    #              'B02DKsPi' : ["[B0 -> D- K*(892)+]cc"],
    #              'B02DKPi0' : ["[B0 -> D- K*(892)+]cc"]}
    #    inputs = {'B02DPiPi0': d2x+self.hh.pipi0,
    #              'B02DKsPi' : d2x+self.hh.kspi,
    #              'B02DKPi0' : d2x+self.hh.kpi0}
    #    b02dhh_rs = makeB2XSels(decays,dname,inputs,config,True,False)
    #    decays = {'B02DPiPi0WS': ["[B0 -> D- rho(770)-]cc"],
    #              'B02DKsPiWS' : ["[B0 -> D- K*(892)-]cc"],
    #              'B02DKPi0WS' : ["[B0 -> D- K*(892)-]cc"]}
    #    inputs = {'B02DPiPi0WS': d2x+self.hh.pipi0,
    #              'B02DKsPiWS' : d2x+self.hh.kspi,
    #              'B02DKPi0WS' : d2x+self.hh.kpi0}
    #    b02dhh_ws = makeB2XSels(decays,dname,inputs,config,True,False)
    #    self.lines.append(ProtoLine(b02dhh_rs,1.0))
    #    self.lines.append(ProtoLine(b02dhh_ws,0.1))

    #def _makeB2D0HH(self,dname,d2x):
    #    '''Makes B+- -> D0 h+- h0'''
    #    config = deepcopy(self.config)
    #    config['AM_MAX' ] = '5800*MeV'
    #    decays = {'B2D0PiPi0': ["B+ -> D0 rho(770)+","B- -> D0 rho(770)-"],
    #              'B2D0KsPi' : ["B+ -> D0 K*(892)+","B- -> D0 K*(892)-"],
    #              'B2D0KPi0' : ["B+ -> D0 K*(892)+","B- -> D0 K*(892)-"]}
    #    inputs = {'B2D0PiPi0': d2x+self.hh.pipi0,
    #              'B2D0KsPi' : d2x+self.hh.kspi,
    #              'B2D0KPi0' : d2x+self.hh.kpi0}
    #    b2d0hh = makeB2XSels(decays,dname,inputs,config,True,False)
    #    self.lines.append(ProtoLine(b2d0hh,1.0))

    def _makeB2DHH(self,dname,d2x):
        '''Makes RS and WS B+ -> D+ h+ h- + c.c.'''
        decays = {'B2DPiPi': ["B+ -> D+ rho(770)0","B- -> D- rho(770)0"],
                  'B2DKPi' : ["[B- -> D- K*(892)0]cc","[B+ -> D+ K*(892)0]cc"],
                  'B2DKK'  : ["B+ -> D+ phi(1020)","B- -> D- phi(1020)"]}
        inputs = {'B2DPiPi': d2x+self.hh.pipi_pid,
                  'B2DKPi' : d2x+self.hh.kpi_pid,
                  'B2DKK'  : d2x+self.hh.kk_pid}
        b2dhh = makeB2XSels(decays,dname,inputs,self.config)
        decays = {'B2DHHOS': ["B- -> D+ rho(770)-","B+ -> D- rho(770)+"]}
        inputs = {'B2DHHOS': d2x+self.hh.hh_ws_pid}
        b2dhh_os = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2dhh,1.0))
        self.lines.append(ProtoLine(b2dhh_os,1.0))

    def _makeB2DV(self,dname,d2x):
        '''Makes B+ -> D+ V(h+h-) + c.c.'''
        decays = {'B2DRho0'   : ["B+ -> D+ rho(770)0","B- -> D- rho(770)0"],
                  'B2DKstar0' : ["[B- -> D- K*(892)0]cc","[B+ -> D+ K*(892)0]cc"],
                  'B2DPhi'    : ["B+ -> D+ phi(1020)","B- -> D- phi(1020)"]}
        inputs = {'B2DRho0':    d2x+self.hh.rho0_pid,
                  'B2DKstar0' : d2x+self.hh.kstar0_pid,
                  'B2DPhi'  :   d2x+self.hh.phi_pid}
        b2dv = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2dv,1.0))

    def _makeB02DV(self,dname,d2x):
        '''Makes B+ -> D+ V(h0h-) + c.c.'''
        decays = {'B02DRhoPM'   : ["[B0 -> D+ rho(770)-]cc"],
                  'B02DKstarPM' : ["[B0 -> D- K*(892)+]cc"]}
        inputs = {'B02DRhoPM': d2x+self.hh.rhopm,
                  'B02DKstarPM' : d2x+self.hh.kstarpm}
        b2dv = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2dv,1.0))
                                    
    def _makeB2D0H(self,dname,d2x,useIP=True):
        '''Makes RS B+ -> D0 h+ (h=pi,K) + c.c.'''
        tag = 'B2D0%s'
        if not useIP: tag += 'NoIP'
        decays = {tag%'Pi': ["B+ -> D0 pi+","B- -> D0 pi-"]}
        inputs = {tag%'Pi': d2x+self.topoPions}
        if useIP:
            decays[tag%'K'] = ["B+ -> D0 K+","B- -> D0 K-"]
            inputs[tag%'K'] = d2x+self.topoKaons
        b2d0h = makeB2XSels(decays,dname,inputs,self.config,useIP)
        self.lines.append(ProtoLine(b2d0h,1.0))

    def _makeB2D0MuNu(self,dname,d2x):
        '''Makes RS B+ -> D0 mu nu + c.c.'''
        decays = {'B2D0MuNu': ["B+ -> D0 mu+","B- -> D0 mu-"]}
        inputs = {'B2D0MuNu': d2x+ [ self.muons ] }
#        config = deepcopy(self.config)
        config = {}
        config['B2CBBDT_MIN'] = self.config['B2CBBDT_MIN']
        config['AM_MIN'] = '2500*MeV'
        config['AM_MAX'] = '5200*MeV'
        config['VCHI2DOF_MAX'] = '6'
        config['BPVDIRA_MIN'] = '0.999'
        config['BPVCORRM_MIN'] = '4000*MeV'
        config['BPVCORRM_MAX'] = '6000*MeV'
        b2d0munu = makeB2XSels(decays,dname,inputs,config,True)
        self.lines.append(ProtoLine(b2d0munu,1.0))

    def _makeB2D0Pi(self,dname,d2x,useIP=True):
        '''Makes RS B+ -> D0 h+ (h=pi,K) + c.c.'''
        tag = 'B2D0%s'
        if not useIP: tag += 'NoIP'
        decays = {tag%'Pi': ["B+ -> D0 pi+","B- -> D0 pi-"]}
        inputs = {tag%'Pi': d2x+self.topoPions}
        b2d0h = makeB2XSels(decays,dname,inputs,self.config,useIP)
        self.lines.append(ProtoLine(b2d0h,1.0))
        
    def _makeB02D0KS(self,dname,d2x,ks):
        '''Makes B0 -> D0 KS'''
        decays = {'B02D0KS'+ks: ["B0 -> D0 KS0"]}
        inputs = {'B02D0KS'+ks: d2x+self.ks[ks]}
        b2d0ks = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0ks,1.0))

    def _makeB2DKS(self,dname,d2x,ks):
        '''Makes B+- -> D+- KS'''
        decays = {'B2DKS'+ks: ["[B+ -> D+ KS0]cc"]}
        inputs = {'B2DKS'+ks: d2x+self.ks[ks]}
        b2dks = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2dks,1.0))

    def _makeB2Dst0H(self,dname,d2x):
        '''Makes B+- -> D*0 H+-'''
        decays = {'B2Dstar0Pi': ["B+ -> D*(2007)0 pi+","B- -> D*(2007)0 pi-"],
                  'B2Dstar0K' : ["B+ -> D*(2007)0 K+","B- -> D*(2007)0 K-"]}
        inputs = {'B2Dstar0Pi': d2x+self.topoPions,
                  'B2Dstar0K': d2x+self.topoKaons}
        b2d0h = makeB2XSels(decays,dname,inputs,self.config,True,False)
        self.lines.append(ProtoLine(b2d0h,1.0))

    def _makeB02DstH(self,dname,d2x):
        '''Makes B0 -> D*+- H-+'''
        decays = {'B02DstarPi': ["B0 -> D*(2010)- pi+","B0 -> D*(2010)+ pi-"],
                  'B02DstarK' : ["B0 -> D*(2010)- K+","B0 -> D*(2010)+ K-"]}
        inputs = {'B02DstarPi': d2x+self.topoPions,
                  'B02DstarK': d2x+self.topoKaons}
        b2dsth = makeB2XSels(decays,dname,inputs,self.config,True,False)
        self.lines.append(ProtoLine(b2dsth,1.0))

    def _makeB02DstMuNu(self,dname,d2x):
        '''Makes B0 -> D*+- H-+'''
        decays = {'B02DstarMuNu'   : ["B0 -> D*(2010)- mu+","B0 -> D*(2010)+ mu-"]}
        inputs = {'B02DstarMuNu'   : d2x + [ self.muons ]}
#        config = deepcopy(self.config)
        config = {}
        config['B2CBBDT_MIN'] = self.config['B2CBBDT_MIN']
        config['AM_MIN'] = '2500*MeV'
        config['AM_MAX'] = '5200*MeV'
        config['VCHI2DOF_MAX'] = '6'
        config['BPVDIRA_MIN'] = '0.999'
        config['BPVCORRM_MIN'] = '4000*MeV'
        config['BPVCORRM_MAX'] = '6000*MeV'
        b2dstmunu = makeB2XSels(decays,dname,inputs,config,True,False)
        self.lines.append(ProtoLine(b2dstmunu,1.0))

        decays = {'B02DstarMuNuWS' : ["B0 -> D*(2010)- mu-","B0 -> D*(2010)+ mu+"]}
        inputs = {'B02DstarMuNuWS' : d2x + [ self.muons ] }
        b2dstmunu = makeB2XSels(decays,dname,inputs,config,True,False)
        self.lines.append(ProtoLine(b2dstmunu,0.1))

    def _makeB02DstKsH(self,dname,d2x):
        '''Makes B0 -> D*+- H-+ Ks0'''
        decays = {
                  'B02DstarKsPiDD': ["B0 -> D*(2010)- KS0 pi+","B0 -> D*(2010)+ KS0 pi-"],
                  'B02DstarKsKDD' : ["B0 -> D*(2010)- KS0 K+","B0 -> D*(2010)+ KS0 K-"],
                  'B02DstarKsPiLL': ["B0 -> D*(2010)- KS0 pi+","B0 -> D*(2010)+ KS0 pi-"],
                  'B02DstarKsKLL' : ["B0 -> D*(2010)- KS0 K+","B0 -> D*(2010)+ KS0 K-"]
                 }
        inputs = {
                  'B02DstarKsPiDD': d2x+self.topoPions + self.ks['DD'],
                  'B02DstarKsKDD' : d2x+self.topoKaons + self.ks['DD'],
                  'B02DstarKsPiLL': d2x+self.topoPions + self.ks['LL'],
                  'B02DstarKsKLL' : d2x+self.topoKaons + self.ks['LL'] 
                 }
        b2dstksh_rs = makeB2XSels(decays,dname,inputs,self.config,True,False)
        self.lines.append(ProtoLine(b2dstksh_rs,1.0))

        decays = {
                  'B02DstarKsPiDDWS': ["B0 -> D*(2010)- KS0 pi-","B0 -> D*(2010)+ KS0 pi+"],
                  'B02DstarKsKDDWS' : ["B0 -> D*(2010)- KS0 K-","B0 -> D*(2010)+ KS0 K+"],
                  'B02DstarKsPiLLWS': ["B0 -> D*(2010)- KS0 pi-","B0 -> D*(2010)+ KS0 pi+"],
                  'B02DstarKsKLLWS' : ["B0 -> D*(2010)- KS0 K-","B0 -> D*(2010)+ KS0 K+"]
                 }
        inputs = {
                  'B02DstarKsPiDDWS': d2x+self.topoPions + self.ks['DD'],
                  'B02DstarKsKDDWS' : d2x+self.topoKaons + self.ks['DD'],
                  'B02DstarKsPiLLWS': d2x+self.topoPions + self.ks['LL'],
                  'B02DstarKsKLLWS' : d2x+self.topoKaons + self.ks['LL'] 
                 }
        b2dstksh_ws = makeB2XSels(decays,dname,inputs,self.config,True,False)
        self.lines.append(ProtoLine(b2dstksh_ws,0.1))

    def _makeB02D0HH(self,dname,d2x,presc=1.0):
        '''Makes RS B0 -> D0 h+h- (h=pi,K) + c.c.'''
        decays = {'B02D0PiPi' : ["B0 -> D0 rho(770)0"],
                  'B02D0KPi'  : ["B0 -> D0 K*(892)0","B0 -> D0 K*(892)~0"],
                  'B02D0KK'   : ["B0 -> D0 phi(1020)"]}
        inputs = {'B02D0PiPi' : d2x+self.hh.pipi,
                  'B02D0KPi'  : d2x+self.hh.kpi,
                  'B02D0KK'   : d2x+self.hh.kk}
        b2d0hh = makeB2XSels(decays,dname,inputs,self.config)
        decays = {'B02DHHWS': ["B0 -> D0 rho(770)-","B0 -> D0 rho(770)+"]}
        inputs = {'B02DHHWS': d2x+self.hh.hh_ws}
        b2d0hh_ws = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0hh,presc))
        self.lines.append(ProtoLine(b2d0hh_ws,0.1))        

    def _makeB02D0PPbar(self,dname,d2x):
        '''Makes RS B0 -> D0 p+p~- + c.c.'''
        decays = { 'B02D0PPbar': ["B0 -> D0 rho(770)0"]}
        inputs = { 'B02D0PPbar': d2x+self.hh.ppbar_pid}
        b2d0ppbar = makeB2XSels(decays,dname,inputs,self.config)
        decays = {'B02D0PPbarWS': ["B0 -> D0 rho(770)-","B0 -> D0 rho(770)+"]}
        inputs = {'B02D0PPbarWS': d2x+self.hh.ppbar_ws_pid}
        b2d0ppbar_ws = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0ppbar,1.0))
        self.lines.append(ProtoLine(b2d0ppbar_ws,0.1))        


    def _makeB02D0HH_FULLDST(self,dname,d2x):
        '''Makes RS B0 -> D0 h+h- (h=pi,K) + c.c. (Tighter selection for Full DST)'''
        config = deepcopy(self.config)
        config['AM_MIN'] = '4950*MeV'
        config['AM_MAX'] = '6000*MeV'
        config['B2CBBDT_MIN'] = '0.3'
        config['DZ1_MIN'] = '-1.5*mm'
        decays = {'B02D0PiPi': ["B0 -> D0 rho(770)0"],
                  'B02D0KPi' : ["B0 -> D0 K*(892)0","B0 -> D0 K*(892)~0"]}
        inputs = {'B02D0PiPi' : d2x+self.hh.pipi_pid_tightpi,
                  'B02D0KPi'  : d2x+self.hh.kpi_pid_tighter}
        b2d0hh = makeB2XSels(decays,dname,inputs,config)
        self.lines.append(ProtoLine(b2d0hh,1.0))

    def _makeB02D0PPbar_FULLDST(self,dname,d2x):
        '''Makes RS B0 -> D0 p+p~- + c.c.'''
        config = deepcopy(self.config)
        config['AM_MIN'] = '4950*MeV'
        config['AM_MAX'] = '5800*MeV'
        config['B2CBBDT_MIN'] = '0.3'
        config['DZ1_MIN'] = '-1.5*mm'
        decays = { 'B02D0PPbar': ["B0 -> D0 rho(770)0"]}
        inputs = { 'B02D0PPbar': d2x+self.hh.ppbar_Mcut_pid_tighter2}
        b2d0ppbar = makeB2XSels(decays,dname,inputs,self.config)
        #decays = {'B02D0PPbarWS': ["B0 -> D0 rho(770)-","B0 -> D0 rho(770)+"]}
        #inputs = {'B02D0PPbarWS': d2x+self.hh.ppbar_ws_pid_tighter}
        #b2d0ppbar_ws = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0ppbar,1.0))
        #self.lines.append(ProtoLine(b2d0ppbar_ws,0.1))        

    def _makeB02DHHH(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        pipipi   = self.hhh.pipipi
        kpipi    = self.hhh.kpipi
        kpipi_ss = self.hhh.kpipiSS
        kkpi     = self.hhh.kkpi
        decays = {'B02DPiPiPi'  : ["[B0 -> D- a_1(1260)+]cc"],
                  'B02DKPiPi'   : ["[B0 -> D- K_1(1270)+]cc"],
                  'B02DKPiPiSS' : ["[B0 -> D- K_1(1270)+]cc"],
                  'B02DKKPi'    : ["[B0 -> D- a_1(1260)+]cc"]}
        inputs = {'B02DPiPiPi': d2x+pipipi, 'B02DKPiPi': d2x+kpipi, 'B02DKPiPiSS' : d2x+kpipi_ss, 'B02DKKPi': d2x+kkpi}
        b02dhhh_rs = makeB2XSels(decays,dname,inputs,self.config)
        decays = {'B02DPiPiPiWS'  : ["[B0 -> D- a_1(1260)-]cc"],
                  'B02DKPiPiWS'   : ["[B0 -> D- K_1(1270)-]cc"],
                  'B02DKPiPiSSWS' : ["[B0 -> D- K_1(1270)-]cc"],
                  'B02DKKPiWS'    : ["[B0 -> D- a_1(1260)-]cc"]}
        inputs = {'B02DPiPiPiWS': d2x+pipipi, 'B02DKPiPiSSWS': d2x + kpipi_ss, 'B02DKPiPiWS': d2x+kpipi, 'B02DKKPiWS': d2x+kkpi}
        b02dhhh_ws = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dhhh_rs,1.0))
        self.lines.append(ProtoLine(b02dhhh_ws,0.1))

    def _makeB02DHHH_FULLDST(self,dname,d2x):
        '''Makes RS and WS B0 -> D + h- + c.c.  (Tighter selection for Full DST)'''
        config = deepcopy(self.config)
        config['AM_MIN'] = '4950*MeV'
        config['AM_MAX'] = '6000*MeV'
        config['B2CBBDT_MIN'] = '0.3'
        config['DZ1_MIN'] = '-1.5*mm'
        
        kpipi_realtightk = self.hhh.kpipi_realtightk
        pipipi_tighterpi = self.hhh.pipipi_tighterpi
        decays = {'B02DKPiPi' : ["[B0 -> D- K_1(1270)+]cc"],
                  'B02DPiPiPi': ["[B0 -> D- a_1(1260)+]cc"]}
        inputs = {'B02DKPiPi' : d2x+kpipi_realtightk,
                  'B02DPiPiPi': d2x+pipipi_tighterpi}
        b02dhhh_rs = makeB2XSels(decays,dname,inputs,config)
        self.lines.append(ProtoLine(b02dhhh_rs,1.0))

    def _makeB02DstHHH(self,dname,dstar):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        kkpi = self.hhh.kkpi
        decays = {'B02DstPiPiPi': ["[B0 -> D*(2010)- a_1(1260)+]cc"],
                  'B02DstKPiPi' : ["[B0 -> D*(2010)- K_1(1270)+]cc"],
                  'B02DstKKPi'  : ["[B0 -> D*(2010)- a_1(1260)+]cc"]}
        inputs = {'B02DstPiPiPi': dstar+pipipi, 'B02DstKPiPi': dstar+kpipi, 'B02DstKKPi': dstar+kkpi}
        b02dsthhh_rs = makeB2XSels(decays,dname,inputs,self.config)
        decays = {'B02DstPiPiPiWS'  : ["[B0 -> D*(2010)+ a_1(1260)+]cc"],
                  'B02DstKPiPiWS'   : ["[B0 -> D*(2010)+ K_1(1270)+]cc"],
                  'B02DstKKPiWS'    : ["[B0 -> D*(2010)+ a_1(1260)+]cc"]}
        inputs = {'B02DstPiPiPiWS': dstar+pipipi,
                  'B02DstKPiPiWS' : dstar+kpipi,
                  'B02DstKKPiWS' : dstar+kkpi}
        b02dsthhh_ws = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dsthhh_rs,1.0))
        self.lines.append(ProtoLine(b02dsthhh_ws,0.1))

#
#    def _makeB02DstHHH(self,dname,d2x):
#        '''Makes RS and WS B0 -> D*+ h-h+h- + c.c.'''
#        pipipi = self.hhh.pipipi
#        kpipi = self.hhh.kpipi
#        kkpi = self.hhh.kkpi
#        decays = {'B02DstPiPiPi': ["[B0 -> D*(2010)- a_1(1260)+]cc"],
#                  'B02DstKPiPi' : ["[B0 -> D*(2010)- K_1(1270)+]cc"],
#                  'B02DstKKPi':   ["[B0 -> D*(2010)- a_1(1260)+]cc"]}
#        inputs = {'B02DstPiPiPi': d2x+pipipi, 'B02DstKPiPi': d2x+kpipi, 'B02DstKKPi': d2x+kkpi}
#        b02dsthhh_rs = makeB2XSels(decays,dname,inputs,self.config)
#        decays = {'B02DstPiPiPiWS': ["[B0 -> D*(2010)- a_1(1260)-]cc"],
#                  'B02DstKPiPiWS' : ["[B0 -> D*(2010)- K_1(1270)-]cc"],
#                  'B02DstKKPi'    : ["[B0 -> D*(2010)- a_1(1260)-]cc"]}
#        inputs = {'B02DstPiPiPiWS': d2x+pipipi, 'B02DstKPiPiWS': d2x+kpipi, 'B02DstKKPiWS': d2x+kkpi}
#        b02dsthhh_ws = makeB2XSels(decays,dname,inputs,self.config)
#        self.lines.append(ProtoLine(b02dsthhh_rs,1.0))
#        self.lines.append(ProtoLine(b02dsthhh_ws,0.1))
#

    def _makeB2D0HHH(self,dname,d2x):
        '''Makes RS B+ -> D0 h+h-h+ (h=pi,K) + c.c.'''
        pipipi = self.hhh.pipipi
        kpipi = self.hhh.kpipi
        kkpi = self.hhh.kkpi
        decays = {'B2D0PiPiPi': ["B+ -> D0 a_1(1260)+","B- -> D0 a_1(1260)-"],
                  'B2D0KPiPi' : ["B+ -> D0 K_1(1270)+","B- -> D0 K_1(1270)-"],
                  'B2D0KKPi':   ["B+ -> D0 a_1(1260)+","B- -> D0 a_1(1260)-"]}
        inputs = {'B2D0PiPiPi': d2x+pipipi,'B2D0KPiPi': d2x+kpipi, 'B2D0KKPi': d2x+kkpi}
        b2d03h = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d03h,1.0))

    def _makeB2D0ppbarH(self, dname, d2x) : 
        '''Makes B+ -> D0 p pbar h+ (h=pi,K) + c.c.'''
        ppbarpi = self.hhh.ppbarpi
        ppbark = self.hhh.ppbark
        decays = {'B2D0ppbarPi': ["B+ -> D0 a_1(1260)+","B- -> D0 a_1(1260)-"],
                  'B2D0ppbarK' : ["B+ -> D0 a_1(1260)+","B- -> D0 a_1(1260)-"] }
        inputs = {'B2D0ppbarPi': d2x+ppbarpi,'B2D0ppbarK': d2x+ppbark }
        b2d0ppbarh = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d0ppbarh,1.0))

    def _makeB02DppbarH(self, dname, d2x) : 
        '''Makes B0 -> D- p pbar h+ (h=pi,K) + c.c.'''
        ppbarpi = self.hhh.ppbarpi
        ppbark = self.hhh.ppbark
        decays = {'B02DppbarPi': ["[B0 -> D- a_1(1260)+]cc"],
                  'B02DppbarK' : ["[B0 -> D- a_1(1260)+]cc"] }
        inputs = {'B02DppbarPi': d2x+ppbarpi,'B02DppbarK': d2x+ppbark }
        b02dppbarh = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b02dppbarh,1.0))

    def _makeB2D0HHH_FULLDST(self,dname,d2x):
        '''Makes RS B+ -> D0 h+h-h+ (h=pi,K) + c.c.'''
        config = deepcopy(self.config)
        config['AM_MAX'] = '5500*MeV'
        config['AM_MIN'] = '5000*MeV'
        pipipi = self.hhh.pipipi_tighterpi
        decays = {'B2D0PiPiPi': ["B+ -> D0 a_1(1260)+","B- -> D0 a_1(1260)-"]}
        inputs = {'B2D0PiPiPi': d2x+pipipi}
        b2d03h = makeB2XSels(decays,dname,inputs,self.config)
        self.lines.append(ProtoLine(b2d03h,1.0))

    def _makeB02DD(self):
        '''Makes RS and WS B0 -> D+D- + c.c.'''
        decays = {'B02DD': ["B0 -> D- D+"] }
        inputs = {'B02DD': self.d.hhh_pid}
        b2dd_rs = makeB2XSels(decays,'',inputs,self.config)
        decays = {'B02DDWS': ["[B0 -> D+ D+]cc"] }
        inputs = {'B02DDWS': self.d.hhh_pid}
        b2dd_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dd_rs,1.0))
        self.lines.append(ProtoLine(b2dd_ws,0.1))

    def _makeB02DD_FULLDST(self):
        '''Makes RS and WS B0 -> D+D- + c.c.  (Tighter selection for Full DST)'''
        config = deepcopy(self.config)
        config['B2CBBDT_MIN'] = '0.3'
        config['AM_MAX'] = '6000*MeV'
        config['DZ1_MIN'] = '-1.5*mm'
        config['DZ2_MIN'] = '-1.5*mm'
        decays = {'B02DDFULLDST': ["B0 -> D- D+"],
                  'B02DDUPFULLDST': ["B0 -> D- D+"]}
        inputs = {'B02DDFULLDST': self.d.d_hhh_4_B2DD_custom,
                  'B02DDUPFULLDST': self.d.d_cf_hhh_pid_tightpi_up}
        b2dd_rs = makeB2XSels(decays,'',inputs,config)
        self.lines.append(ProtoLine(b2dd_rs,1.0))

    def _makeB2D0D(self):
        '''Makes B+ -> D+ D0'''
        decays = {'B2D0D': ["B- -> D0 D-","B+ -> D0 D+"] }
        inputs = {'B2D0D': self.d.hhh_pid+self.d.hh_pid}
        b2d0d = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0d,1.0))
        inputs = {'B2D0D': self.d.hhh_pid+self.d.k3pi_pid}
        b2d0d = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2d0d,1.0))

    def _makeB02D0D0(self):
        '''Makes B0 -> D0 D0'''
        dec = ["B0 -> D0 D0"]
 #       decays = {'B02D0D0' : dec}
  #      inputs = {'B02D0D0' : self.d.d0_cf_pid}
        decays = {'B02D0D0D02HHD02HH'    : dec,
                  'B02D0D0D02HHD02K3Pi'  : dec,
                  'B02D0D0D02K3PiD02K3Pi': dec}
        inputs = {'B02D0D0D02HHD02HH'  : self.d.hh_pid,
                  'B02D0D0D02HHD02K3Pi':self.d.hh_pid+self.d.k3pi_pid,
                  'B02D0D0D02K3PiD02K3Pi':self.d.k3pi_pid}
        b2d0d0 = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0d0,1.0))

    def _makeB02D0D0_FULLDST(self):
        '''Makes B0 -> D0 D0  (Tighter selection for Full DST)'''
        config = deepcopy(self.config)
        config['B2CBBDT_MIN'] = '0.3'
        config['AM_MAX'] = '6000*MeV'
        config['DZ1_MIN'] = '-1.5*mm'
        config['DZ2_MIN'] = '-1.5*mm'
        dec = ["B0 -> D0 D0"]
        decays = {'B02D0D0FULLDST' : dec,
                  'B02D0D0D02K3PiUPFULLDST' : dec}
        inputs = {'B02D0D0FULLDST' : self.d.d0_cf_pid_tight,
                  'B02D0D0D02K3PiUPFULLDST' : self.d.k3pi_pid_tight_up}
        b2d0d0 = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0d0,1.0))
   
    def _makeB02DstD(self):
        '''Makes the RS and WS B+ -> D*+- D-+ + c.c.'''
        decays = {'B02DstD':["[B0 -> D*(2010)- D+]cc"]}
        inputs = {'B02DstD': self.d.hhh_pid+self.dst.d0pi_pid}
        b2dstd_rs = makeB2XSels(decays,'',inputs,self.config)
        decays = {'B02DstDWS':["[B0 -> D*(2010)- D-]cc"]}
        inputs = {'B02DstDWS': self.d.hhh_pid+self.dst.d0pi_pid}
        b2dstd_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd_rs,1.0))
        self.lines.append(ProtoLine(b2dstd_ws,0.1))

    def _makeB02DstDD02K3Pi(self):
        '''Makes the B0 -> D*+- D-+ '''
        decays = {'B02DstDD02K3Pi':["[B0 -> D*(2010)- D+]cc"]}
        inputs = {'B02DstDD02K3Pi': self.dst.d0pi_k3pi + self.d.hhh_pid }
        b2dstdd02k3pi = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdd02k3pi,1.0))
        
    def _makeB2DstD0(self):
        '''Makes B+- -> D*+- D0'''
        decays = {'B2DstD0':["B+ -> D*(2010)+ D0","B- -> D*(2010)- D0"]}
        inputs = {'B2DstD0': self.dst.d0pi_pid+self.d.hh_pid}
        b2dstd0 = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0,1.0))
        inputs = {'B2DstD0': self.dst.d0pi_pid+self.d.k3pi_pid}
        b2dstd0 = makeB2XSels(decays,'D02K3Pi',inputs,self.config)  
        self.lines.append(ProtoLine(b2dstd0,1.0))

    def _makeB02DstDst(self):
        '''Makes the RS and WS B+ -> D*+- D*-+ + c.c.'''
        decays = {'B02DstDst':["B0 -> D*(2010)- D*(2010)+"]}
        inputs = {'B02DstDst': self.dst.d0pi_pid}
        b2dstdst_rs = makeB2XSels(decays,'',inputs,self.config)
        decays = {'B02DstDstWS':["[B0 -> D*(2010)- D*(2010)-]cc"]}
        inputs = {'B02DstDstWS': self.dst.d0pi_pid}
        b2dstdst_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdst_rs,1.0))
        self.lines.append(ProtoLine(b2dstdst_ws,0.1))

    def _makeB2DDK(self):
        '''Makes RS and WS B+ -> D+D-K+ + c.c.'''
        decays = {'B2DDK': ["[B+ -> D- D+ K+]cc","[B+ -> D+ D+ K-]cc"]}
        inputs = {'B2DDK': self.d.hhh_pid+self.topoKaons}
        b2ddk_rs = makeB2XSels(decays,'',inputs,self.config)
        decays = {'B2DDKWS': ["[B+ -> D+ D+ K+]cc"] }
        inputs = {'B2DDKWS': self.d.hhh_pid+self.topoKaons}
        b2ddk_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2ddk_rs,1.0))
        self.lines.append(ProtoLine(b2ddk_ws,0.1))

    def _makeB02D0DK(self):
        '''Makes RS and WS B0 -> D+ D0 K-'''
        decays = {'B02D0DK': ["B0 -> D0 D- K+","B0 -> D0 D+ K-"]}
        inputs = {'B02D0DK': self.d.hhh_pid+self.d.hh_pid+self.topoKaons}
        b2d0dk_rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dk_rs,1.0))
        inputs = {'B02D0DK': self.d.hhh_pid+self.d.k3pi_pid+self.topoKaons}
        b2d0dk_rs = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dk_rs,1.0))
        decays = {'B02D0DKWS': ["B0 -> D0 D- K-","B0 -> D0 D+ K+"]}
        inputs = {'B02D0DKWS': self.d.hhh_pid+self.d.hh_pid+self.topoKaons}
        b2d0dk_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dk_ws,0.1))
        inputs = {'B02D0DKWS': self.d.hhh_pid+self.d.k3pi_pid+self.topoKaons}
        b2d0dk_ws = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dk_ws,0.1))
    

    def _makeB2D0D0K(self):
        '''Makes B+- -> D0 D0 K+-'''
        dec = ["B+ -> D0 D0 K+","B- -> D0 D0 K-"]
        decays = {'B2D0D0KD02HHD02HH': dec, 'B2D0D0KD02HHD02K3Pi': dec, 'B2D0D0KD02K3PiD02K3Pi': dec}
        inputs = {'B2D0D0KD02HHD02HH': self.d.hh_pid+self.topoKaons, 'B2D0D0KD02HHD02K3Pi': self.d.hh_pid+self.d.k3pi_pid+self.topoKaons,  'B2D0D0KD02K3PiD02K3Pi':self.d.k3pi_pid+self.topoKaons}
        b2d0d0k = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0d0k,1.0))

    def _makeB2DstDK(self):
        '''Makes the RS and WS B+- -> D*+- D-+ K+- + c.c.'''
        decays = {'B2DstDK':["[B+ -> D*(2010)- D+ K+]cc",
                             "[B+ -> D*(2010)+ D- K+]cc",
                             "[B+ -> D*(2010)+ D+ K-]cc"]}
        inputs = {'B2DstDK': self.d.hhh_pid+self.dst.d0pi_pid+self.topoKaons}
        b2dstd_rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd_rs,1.0))
        inputs = {'B2DstDK': self.d.hhh_pid+self.dst.d0pi_k3pi+self.topoKaons}
        b2dstd_rs = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd_rs,1.0))
        decays = {'B2DstDKWS':["[B- -> D*(2010)- D- K-]cc"]}
        inputs = {'B2DstDKWS':self.d.hhh_pid+self.dst.d0pi_pid+self.topoKaons}
        b2dstd_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd_ws,0.1))
        inputs = {'B2DstDKWS':self.d.hhh_pid+self.dst.d0pi_k3pi+self.topoKaons}
        b2dstd_ws = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd_ws,0.1))

    def _makeB02DstD0K(self):
        '''Makes RS and WS B0 -> D*+- D0 K-+'''
        decays = {'B02DstD0K':["B0 -> D*(2010)+ D0 K-","B0 -> D*(2010)- D0 K+"]}
        inputs ={'B02DstD0K':self.dst.d0pi_pid+self.d.hh_pid+self.topoKaons}
        b2dstd0k_rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_rs,1.0))
        inputs ={'B02DstD0K':self.dst.d0pi_pid+self.d.k3pi_pid+self.topoKaons}
        b2dstd0k_rs = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_rs,1.0))
        inputs ={'B02DstD0K':self.dst.d0pi_k3pi+self.d.hh_pid+self.topoKaons}
        b2dstd0k_rs = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_rs,1.0))
        inputs ={'B02DstD0K':self.dst.d0pi_k3pi+self.d.k3pi_pid+self.topoKaons}
        b2dstd0k_rs = makeB2XSels(decays,'DstarD02K3PiD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_rs,1.0))
        decays = {'B02DstD0KWS':["B0 -> D*(2010)+ D0 K+",
                                "B0 -> D*(2010)- D0 K-"]}
        inputs ={'B02DstD0KWS': self.dst.d0pi_pid+self.d.hh_pid+self.topoKaons}
        b2dstd0k_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_ws,0.1))
        inputs ={'B02DstD0KWS': self.dst.d0pi_pid+self.d.k3pi_pid+self.topoKaons}
        b2dstd0k_ws = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_ws,0.1))
        inputs ={'B02DstD0KWS': self.dst.d0pi_k3pi+self.d.hh_pid+self.topoKaons}
        b2dstd0k_ws = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_ws,0.1))
        inputs ={'B02DstD0KWS': self.dst.d0pi_k3pi+self.d.k3pi_pid+self.topoKaons}
        b2dstd0k_ws = makeB2XSels(decays,'DstarD02K3PiD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0k_ws,0.1))

    def _makeB2DstDstK(self):
        '''Makes the RS and WS B+ -> D*+- D*-+ K+ + c.c.'''
        decays = {'B2DstDstK':["[B+ -> D*(2010)- D*(2010)+ K+]cc",
                               "[B+ -> D*(2010)+ D*(2010)+ K-]cc"]}
        inputs = {'B2DstDstK': self.dst.d0pi_pid+self.topoKaons}
        b2dstdstk_rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstk_rs,1.0))
        inputs = {'B2DstDstK': self.dst.d0pi_pid+self.dst.d0pi_k3pi+self.topoKaons}
        b2dstdstk_rs = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstk_rs,1.0))
        inputs = {'B2DstDstK': self.dst.d0pi_k3pi+self.topoKaons}
        b2dstdstk_rs = makeB2XSels(decays,'DstarD02K3PiDstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstk_rs,1.0))
        decays = {'B2DstDstKWS':["[B- -> D*(2010)- D*(2010)- K-]cc"]}
        inputs = {'B2DstDstKWS': self.dst.d0pi_pid+self.topoKaons}
        b2dstdstk_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstk_ws,0.1))
        inputs = {'B2DstDstKWS': self.dst.d0pi_pid+self.dst.d0pi_k3pi+self.topoKaons}
        b2dstdstk_ws = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstk_ws,1.0))
        inputs = {'B2DstDstKWS': self.dst.d0pi_k3pi+self.topoKaons}
        b2dstdstk_ws = makeB2XSels(decays,'DstarD02K3PiDstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstk_ws,1.0))

    def _makeB2Dst2460DK(self):
        '''Makes RS and WS B -> D*2(2460)+- D-+ K+-'''
        input = self.dst.d0pi_2460_pid+self.d.hhh_cf_pid+self.topoKaons
        decays = {'B2Dst2460DK':["[B+ -> D*_2(2460)+ D- K+]cc",
                                 "[B+ -> D*_2(2460)- D+ K+]cc"]}
        inputs ={'B2Dst2460DK': input}
        rs = makeB2XSels(decays,'',inputs,self.config)
        decays = {'B2Dst2460DKWS':["[B+ -> D*_2(2460)+ D+ K+]cc"]}
        inputs ={'B2Dst2460DKWS': input}
        ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))
        self.lines.append(ProtoLine(ws,0.1))

    def _makeB2D0st2460D0K(self):
        '''Makes B -> D*2(2460)0 D0 K+-'''
        input = self.dst.dpi_2460_pid+self.d.kpi_pid+self.topoKaons
        decays = {'B2D0st2460D0K':["B+ -> D*_2(2460)0 D0 K+",
                                   "B- -> D*_2(2460)0 D0 K-"]}
        inputs ={'B2D0st2460D0K': input}
        rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeB02D0D0Kst(self):
        '''Makes the RS B0 -> D0 D0 K*'''
        dec = ["B0 -> D0 D0 K*(892)0","B0 -> D0 D0 K*(892)~0"]
        decays = {'B02D0D0KstD02HHD02HH':dec,  'B02D0D0KstD02HHD02K3Pi':dec, 'B02D0D0KstD02K3PiD02K3Pi':dec}
        inputs = {'B02D0D0KstD02HHD02HH':self.d.hh_pid+self.hh.kpi, 'B02D0D0KstD02HHD02K3Pi':self.d.hh_pid+self.d.k3pi_pid+self.hh.kpi,  'B02D0D0KstD02K3PiD02K3Pi':self.d.k3pi_pid+self.hh.kpi}
        b2ddkst = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2ddkst,1.0))
        

    def _makeB02DDKst(self):
        '''Makes the RS B0 -> D+ D- K*'''
        decays = {'B02DDKst':["B0 -> D+ D- K*(892)0",
                              "B0 -> D+ D- K*(892)~0"]}
        inputs = {'B02DDKst':self.d.hhh_cf_pid+self.hh.kpi}
        b2ddkst = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2ddkst,1.0))

    def _makeB02DstDKst(self):
        '''Makes the RS B0 -> D*+ D- K*'''   
        decays = {'B02DstDKst':["B0 -> D*(2010)+ D- K*(892)0",
                              "B0 -> D+ D*(2010)- K*(892)~0",
                              "B0 -> D- D*(2010)+ K*(892)~0",
                              "B0 -> D*(2010)- D+ K*(892)0"]}
        inputs = {'B02DstDKst':self.dst.d0pi_pid+self.d.hhh_cf_pid+self.hh.kpi}
        b2dstdkst = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdkst,1.0))
        inputs = {'B02DstDKst':self.dst.d0pi_k3pi+self.d.hhh_cf_pid+self.hh.kpi}
        b2dstdkst = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdkst,1.0))

    def _makeB02DstDstKst(self):       
        '''Makes the RS B0 -> D*+ D*- K*'''
        decays = {'B02DstDstKst':["B0 -> D*(2010)+ D*(2010)- K*(892)0",
                                "B0 -> D*(2010)- D*(2010)+ K*(892)~0"]}
        inputs = {'B02DstDstKst':self.dst.d0pi_pid+self.hh.kpi}
        b2dstdstkst = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstkst,1.0))
        inputs = {'B02DstDstKst':self.dst.d0pi_pid+self.dst.d0pi_k3pi+self.hh.kpi}
        b2dstdstkst = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstkst,1.0))
        inputs = {'B02DstDstKst':self.dst.d0pi_k3pi+self.hh.kpi}
        b2dstdstkst = makeB2XSels(decays,'DstarD02K3PiDstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstkst,1.0))

 
    def _makeB2DD0Kst(self):
        '''Makes the RS B+ -> D+ D~0 K*'''
        decays = {'B2DD0Kst':["B- -> D- D0 K*(892)~0","B+ -> D+ D0 K*(892)0"]}
        inputs = {'B2DD0Kst':self.d.hhh_pid+self.d.hh_pid+self.hh.kpi}
        b2dd0kst = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dd0kst,1.0))
        inputs = {'B2DD0Kst':self.d.hhh_pid+self.d.k3pi_pid+self.hh.kpi}
        b2dd0kst = makeB2XSels(decays,'D02K3Pi',inputs,self.config)      
        self.lines.append(ProtoLine(b2dd0kst,1.0))

    def _makeB2DstD0Kst(self):
        '''Makes the RS B+ -> D*(2010)+ D~0 K*'''
        decays = {'B2DstD0Kst':["B- -> D*(2010)- D0 K*(892)~0","B+ -> D*(2010)+ D0 K*(892)0"]}     
        inputs = {'B2DstD0Kst':self.dst.d0pi_pid+self.d.hh_pid+self.hh.kpi}
        b2dstd0kst = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0kst,1.0))
        inputs = {'B2DstD0Kst':self.dst.d0pi_k3pi+self.d.hh_pid+self.hh.kpi}
        b2dstd0kst = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0kst,1.0)) 
        inputs = {'B2DstD0Kst':self.dst.d0pi_pid+self.d.k3pi_pid+self.hh.kpi}
        b2dstd0kst = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0kst,1.0))
        inputs = {'B2DstD0Kst':self.dst.d0pi_k3pi+self.d.k3pi_pid+self.hh.kpi}
        b2dstd0kst = makeB2XSels(decays,'DstarD02K3PiD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0kst,1.0))

    def _makeB02DDKS(self,ks):
        '''Makes RS and WS B0 -> D+D-KS'''
        decays = {'B02DDKS'+ks: ["B0 -> D- D+ KS0"]}
        inputs = {'B02DDKS'+ks: self.d.hhh_pid+self.ks[ks]}
        b2ddks_rs = makeB2XSels(decays,'',inputs,self.config)
        decays = {'B02DDKSWS'+ks: ["[B+ -> D+ D+ KS0]cc"]}
        inputs = {'B02DDKSWS'+ks: self.d.hhh_pid+self.ks[ks]}
        b2ddks_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2ddks_rs,1.0))
        self.lines.append(ProtoLine(b2ddks_ws,0.1))

    def _makeB2D0DKS(self,ks):
        '''Makes B+ -> D+ D0 KS + c.c.'''
        decays = {'B2D0DKS'+ks: ["B+ -> D0 D+ KS0","B- -> D0 D- KS0"]}
        inputs = {'B2D0DKS'+ks: self.d.hhh_pid+self.d.hh_pid+self.ks[ks]}
        b2d0dks = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dks,1.0))
        inputs = {'B2D0DKS'+ks: self.d.hhh_pid+self.d.k3pi_pid+self.ks[ks]}
        b2d0dks = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dks,1.0))

    def _makeB02D0D0KS(self,ks):
        '''Makes B0 -> D0 D0 KS'''
        dec = ["B0 -> D0 D0 KS0"]
        decays = {'B02D0D0KSD02HHD02HH'+ks: dec,  'B02D0D0KSD02HHD02K3Pi'+ks: dec,  'B02D0D0KSD02K3PiD02K3Pi'+ks: dec}
        inputs = {'B02D0D0KSD02HHD02HH'+ks: self.d.hh_pid+self.ks[ks], 'B02D0D0KSD02HHD02K3Pi'+ks: self.d.hh_pid+self.d.k3pi_pid+self.ks[ks], 'B02D0D0KSD02K3PiD02K3Pi'+ks: self.d.k3pi_pid+self.ks[ks]}
        b2d0d0ks = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0d0ks,1.0))

    def _makeB02DstDKS(self,ks):
        '''Makes the RS and WS B0 -> D*+- D-+ KS + c.c.'''
        decays = {'B02DstDKS'+ks:["[B0 -> D*(2010)- D+ KS0]cc"]}
        inputs = {'B02DstDKS'+ks: self.d.hhh_pid+self.dst.d0pi_pid+self.ks[ks]}
        b2dstdks_rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdks_rs,1.0))
        inputs = {'B02DstDKS'+ks: self.d.hhh_pid+self.dst.d0pi_k3pi+self.ks[ks]}
        b2dstdks_rs = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdks_rs,1.0))
        decays = {'B02DstDKSWS'+ks:["[B0 -> D*(2010)- D- KS0]cc"]}
        inputs ={'B02DstDKSWS'+ks:self.d.hhh_pid+self.dst.d0pi_pid+self.ks[ks]}
        b2dstdks_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdks_ws,0.1))
        inputs ={'B02DstDKSWS'+ks:self.d.hhh_pid+self.dst.d0pi_k3pi+self.ks[ks]}
        b2dstdks_ws = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdks_ws,0.1))

    def _makeB2DstD0KS(self,ks):
        '''Makes B+- -> D*+- D0 KS'''
        decays = {'B2DstD0KS'+ks:["B+ -> D*(2010)+ D0 KS0",
                                  "B- -> D*(2010)- D0 KS0"]}
        inputs={'B2DstD0KS'+ks:self.dst.d0pi_pid+self.d.hh_pid+self.ks[ks]}
        b2dstd0ks = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0ks,1.0))
        inputs={'B2DstD0KS'+ks:self.dst.d0pi_pid+self.d.k3pi_pid+self.ks[ks]}
        b2dstd0ks = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0ks,1.0))
        inputs={'B2DstD0KS'+ks:self.dst.d0pi_k3pi+self.d.hh_pid+self.ks[ks]}
        b2dstd0ks = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)  
        self.lines.append(ProtoLine(b2dstd0ks,1.0))  
        inputs={'B2DstD0KS'+ks:self.dst.d0pi_k3pi+self.d.k3pi_pid+self.ks[ks]}
        b2dstd0ks = makeB2XSels(decays,'DstarD02K3PiD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstd0ks,1.0))

    def _makeB02DstDstKS(self,ks):
        '''Makes the RS and WS B0 -> D*+- D*-+ KS'''
        decays = {'B02DstDstKS'+ks:["B0 -> D*(2010)- D*(2010)+ KS0"]}
        inputs = {'B02DstDstKS'+ks: self.dst.d0pi_pid+self.ks[ks]}
        b2dstdstks_rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstks_rs,1.0))
        inputs = {'B02DstDstKS'+ks: self.dst.d0pi_pid+self.dst.d0pi_k3pi+self.ks[ks]}
        b2dstdstks_rs = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstks_rs,1.0))
        inputs = {'B02DstDstKS'+ks: self.dst.d0pi_k3pi+self.ks[ks]}
        b2dstdstks_rs = makeB2XSels(decays,'DstarD02K3PiDstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstks_rs,1.0))
        decays = {'B02DstDstKSWS'+ks:["[B0 -> D*(2010)- D*(2010)- KS0]cc"]}
        inputs = {'B02DstDstKSWS'+ks: self.dst.d0pi_pid+self.ks[ks]}
        b2dstdstks_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2dstdstks_ws,0.1))
        inputs = {'B02DstDstKSWS'+ks: self.dst.d0pi_pid+self.dst.d0pi_k3pi+self.ks[ks]}
        b2dstdstks_ws = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)   
        self.lines.append(ProtoLine(b2dstdstks_ws,0.1))
        inputs = {'B02DstDstKSWS'+ks: self.dst.d0pi_k3pi+self.ks[ks]}
        b2dstdstks_ws = makeB2XSels(decays,'DstarD02K3PiDstarD02K3Pi',inputs,self.config)   
        self.lines.append(ProtoLine(b2dstdstks_ws,0.1))

    def _makeB2D0DKK(self):
        '''Makes RS B- -> D0D-K+K- + c.c. B- ->Ds- D0 K+ K+ + cc'''
        decays = {'B2D0DKK': ["B- -> D0 D- K+ K-","B+ -> D- D0 K+ K+","B+ -> D0 D+ K+ K-","B- -> D+ D0 K- K-"]}
        inputs = {'B2D0DKK': self.d.hhh_pid+self.d.hh_pid+self.topoKaons}
        b2d0dkk = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dkk,1.0))
        inputs = {'B2D0DKK': self.d.hhh_pid+self.d.k3pi_pid+self.topoKaons}
        b2d0dkk = makeB2XSels(decays,'D2K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dkk,1.0))

    def _makeB2D0DKPi(self):
        '''Makes RS B+ -> D0D-K+pi+ + c.c.'''
        decays = {'B2D0DKPi': ["B+ -> D- D0 K+ pi+","B- -> D+ D0 K- pi-"]}  
        inputs = {'B2D0DKPi': self.d.hhh_pid+self.d.hh_pid+self.topoPions}
        b2d0dkpi = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dkpi,1.0))
        inputs = {'B2D0DKPi': self.d.hhh_pid+self.d.k3pi_pid+self.topoPions}
        b2d0dkpi = makeB2XSels(decays,'D02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b2d0dkpi,1.0))


    def _makeB02DDKK(self):
        '''Makes RS B0 -> D+D-K+K- + c.c.'''  
        decays = {'B02DDKK': ["B0 -> D+ D- K+ K-"]}                  
        inputs = {'B02DDKK': self.d.hhh_pid+self.topoKaons}
        b02ddkk = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b02ddkk,1.0))   

    def _makeB02DstDKK(self):
        '''Makes RS B0 -> Dst+D-K+K- + c.c.'''
        decays = {'B02DstDKK': ["[B0 -> D*(2010)+ D- K+ K-]cc"]}
        inputs = {'B02DstDKK': self.d.hhh_pid+self.dst.d0pi_pid+self.topoKaons}
        b02dstdkk = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b02dstdkk,1.0))
        inputs = {'B02DstDKK': self.d.hhh_pid+self.dst.d0pi_k3pi+self.topoKaons}
        b02dstdkk = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)
        self.lines.append(ProtoLine(b02dstdkk,1.0))

    def _makeB02DstDstKK(self):
        '''Makes RS B0 -> Dst+Dst-K+K- + c.c.'''
        decays = {'B02DstDstKK': ["B0 -> D*(2010)+ D*(2010)- K+ K-"]}                                 
        inputs = {'B02DstDstKK': self.dst.d0pi_pid+self.topoKaons}
        b02dstdstkk = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b02dstdstkk,1.0))
        inputs = {'B02DstDstKK': self.dst.d0pi_pid+self.dst.d0pi_k3pi+self.topoKaons}
        b02dstdstkk = makeB2XSels(decays,'DstarD02K3Pi',inputs,self.config)   
        self.lines.append(ProtoLine(b02dstdstkk,1.0))
        inputs = {'B02DstDstKK': self.dst.d0pi_k3pi+self.topoKaons}
        b02dstdstkk = makeB2XSels(decays,'DstarD02K3PiDstarD02K3Pi',inputs,self.config)   
        self.lines.append(ProtoLine(b02dstdstkk,1.0))


    def _makeB02D0D0KK(self):
        '''Makes RS B0 -> D0D~0K+K- + c.c.'''
        dec = ["B0 -> D0 D0 K+ K-"]
        decays = {'B02D0D0KKD02HHD02HH': dec, 'B02D0D0KKD02HHD02K3Pi': dec, 'B02D0D0KKD02K3PiD02K3Pi': dec }
        inputs = {'B02D0D0KKD02HHD02HH': self.d.hh_pid+self.topoKaons, 'B02D0D0KKD02HHD02K3Pi': self.d.hh_pid+self.d.k3pi_pid+self.topoKaons, 'B02D0D0KKD02K3PiD02K3Pi': self.d.k3pi_pid+self.topoKaons}
        b02d0d0kk_rs = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b02d0d0kk_rs,1.0))
        dec = ["B0 -> D0 D0 K+ K+","B0 -> D0 D0 K- K-"]
        decays = {'B02D0D0KKWSD02HHD02HH': dec,'B02D0D0KKWSD02HHD02K3Pi': dec, 'B02D0D0KKWSD02K3PiD02K3Pi': dec}
        inputs = {'B02D0D0KKWSD02HHD02HH': self.d.hh_pid+self.topoKaons,
                  'B02D0D0KKWSD02HHD02K3Pi': self.d.hh_pid+self.d.k3pi_pid+self.topoKaons, 
                  'B02D0D0KKWSD02K3PiD02K3Pi': self.d.k3pi_pid+self.topoKaons}	
        b02d0d0kk_ws = makeB2XSels(decays,'',inputs,self.config)
        self.lines.append(ProtoLine(b02d0d0kk_ws,1.0))

    def _makeB02DHD2PhiMu(self,dname,d2x,useIP=True):
        '''Makes RS and WS B0 -> D + h- + c.c.'''
        config = deepcopy(self.config)
        #config{ # Cuts made on all B's and Lb's used in all lines
        config['SUMPT_MIN'    ] = '4000*MeV'
        config['AM_MIN'       ] = '4000*MeV' 
        config.pop('BPVIPCHI2_MAX')
        
        tag = 'B02D%s'
        if not useIP: tag += 'NoIP'
        pions = self.topoPions
        kaons = self.topoKaons
        decays = {tag%'Pi': ["[B0 -> D- pi+]cc"]}
        inputs = {tag%'Pi': d2x+pions}
        if useIP:
            decays[tag%'K'] = ["[B0 -> D- K+]cc"]
            inputs[tag%'K'] = d2x+kaons
        b02dh_rs = makeB2XSels(decays,dname,inputs,config,useIP)        
        decays = {tag%'PiWS': ["[B0 -> D- pi-]cc"]}
        inputs = {tag%'PiWS': d2x+pions}
        if useIP:
            decays[tag%'KWS'] = ["[B0 -> D- K-]cc"]
            inputs[tag%'KWS'] = d2x+kaons
        b02dh_ws = makeB2XSels(decays,dname,inputs,config,useIP)
        self.lines.append(ProtoLine(b02dh_rs,1.0))
        self.lines.append(ProtoLine(b02dh_ws,0.1))

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
