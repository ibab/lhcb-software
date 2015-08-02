#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from Configurables import ( DaVinci__N3BodyDecays, DaVinci__N4BodyDecays,
                            DaVinci__N5BodyDecays )
from PhysSelPython.Wrappers import Selection, MergedSelection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Configurables import SubPIDMMFilter

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def awmFunctor(decays,min,max):
     wm = []
     for daughters in decays:
          pids = ','.join(["'%s'"%d for d in daughters])
          wm.append("in_range(%s,AWM(%s),%s)" % (min,pids,max))
     return '('+('|'.join(wm))+')'

def getCC(decay):
     cc = []
     for d in decay:
          if d.find('+') >=0: cc.append(d.replace('+','-'))
          elif d.find('-') >=0: cc.append(d.replace('-','+'))
          else: cc.append(d)
     return cc

def getCCs(decays): return [getCC(d) for d in decays]

def subPIDSels(decays,prefix,suffix,min,max,inputs):
     min = float(min.split('*')[0])
     max = float(max.split('*')[0])
     filter = SubPIDMMFilter(prefix+'SubPIDBeauty2Charm'+suffix,Code='ALL',
                             MinMM=min,MaxMM=max,PIDs=decays)
     presel = Selection(prefix+'SubPIDSelBeauty2Charm'+suffix,
                        Algorithm=filter,
                        RequiredSelections=inputs)
     mm =  "in_range(%s,MM,%s)" % (min,max)
     return filterSelection(prefix+suffix,mm,[presel])

def makeSelName(pre,dec,post=''):
     name = pre+'2'
     for d in dec :
          if   d == 'K*(892)+' : name += 'Kst+'
          elif d == 'K*(892)-' : name += 'Kst-'
          else                 : name += d
     name += post
     #print name
     return name

def makeSel(parent,dec):
     sel = parent + ' ->'
     for d in dec : sel += ' ' + d
     #print sel
     return sel

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DBuilder(object):
     '''Produces all D mesons for the Beauty2Charm module.'''

     def __init__(self,pions,kaons,ks,pi0,uppions,upkaons,muons,hh,config,config_pid):

          self.pions = pions
          self.kaons = kaons
          self.ks    = ks
          self.pi0   = pi0
          self.uppions = uppions
          self.upkaons = upkaons
          self.muons = muons
          self.hhBuilder = hh
          self.config = config
          self.config_pid = config_pid
          self.hh   = self._makeD2hh()
          self.hhh  = self._makeD2hhh()
          self.hhhh = self._makeD2hhhh()

          self.ksh_ll    = self._makeD2KSh("LL")
          self.ksh_dd    = self._makeD2KSh("DD")
          self.kshh_ll   = self._makeD2KShh("LL")
          self.kshh_dd   = self._makeD2KShh("DD")
          self.kshhh_ll  = self._makeD2KShhh("LL")
          self.kshhh_dd  = self._makeD2KShhh("DD")

          self.pi0hh_merged = self._makeD2Pi0hh("Merged")
          self.pi0hh_resolved = self._makeD2Pi0hh("Resolved")
          self.pi0hhh_merged = self._makeD2Pi0hhh("Merged")
          self.pi0hhh_resolved = self._makeD2Pi0hhh("Resolved")
          self.pi0hhhh_merged = self._makeD2Pi0hhhh("Merged")
          self.pi0hhhh_resolved = self._makeD2Pi0hhhh("Resolved")

          self.kspi0h_ll_merged = self._makeD2KSPi0h("LL","Merged")
          self.kspi0h_ll_resolved = self._makeD2KSPi0h("LL","Resolved")
          self.kspi0h_dd_merged = self._makeD2KSPi0h("DD","Merged")
          self.kspi0h_dd_resolved = self._makeD2KSPi0h("DD","Resolved")

          self.kspi0hh_ll_merged   = self._makeD2KSPi0hh("LL","Merged")
          self.kspi0hh_ll_resolved = self._makeD2KSPi0hh("LL","Resolved")
          self.kspi0hh_dd_merged   = self._makeD2KSPi0hh("DD","Merged")
          self.kspi0hh_dd_resolved = self._makeD2KSPi0hh("DD","Resolved")

          self.kspi0hh_ll_merged_pid   = [filterPID('D2KsLLMergedPi0HHPID',self.kspi0hh_ll_merged,config_pid)]
          self.kspi0hh_ll_resolved_pid = [filterPID('D2KsLLResolvedPi0HHPID',self.kspi0hh_ll_resolved,config_pid)]
          self.kspi0hh_dd_merged_pid   = [filterPID('D2KsDDMergedPi0HHPID',self.kspi0hh_dd_merged,config_pid)]
          self.kspi0hh_dd_resolved_pid = [filterPID('D2KsDDResolvedPi0HHPID',self.kspi0hh_dd_resolved,config_pid)]

          self.kstarhh = self._makeD2Kstarhh()
          self.kstarkstar0 = self._makeD2KstarKstar0()
          self.kstarks_ll = self._makeD2KstarKs("LL")
          self.kstarks_dd = self._makeD2KstarKs("DD")

          # UP tracks in D
          self.hh_up = self._makeD2hh(True)
          self.hhh_up = self._makeD2hhh(True)
          self.hhhh_up = self._makeD2hhhh(True)
          self.ksh_ll_up  = self._makeD2KSh("LL",True)
          self.ksh_dd_up  = self._makeD2KSh("DD",True)
          self.kshh_ll_up  = self._makeD2KShh("LL",True)
          self.kshh_dd_up  = self._makeD2KShh("DD",True)
          self.kshhh_ll_up  = self._makeD2KShhh("LL",True)
          self.kshhh_dd_up  = self._makeD2KShhh("DD",True)
          self.pi0hh_merged_up = self._makeD2Pi0hh("Merged",True)
          self.pi0hh_resolved_up = self._makeD2Pi0hh("Resolved",True)
          self.pi0hhh_merged_up = self._makeD2Pi0hhh("Merged",True)
          self.pi0hhh_resolved_up = self._makeD2Pi0hhh("Resolved",True)
          self.pi0hhhh_merged_up = self._makeD2Pi0hhhh("Merged",True)
          self.pi0hhhh_resolved_up = self._makeD2Pi0hhhh("Resolved",True)

          self.kspi0h_ll_merged_up = self._makeD2KSPi0h("LL","Merged",True)
          self.kspi0h_ll_resolved_up = self._makeD2KSPi0h("LL","Resolved",True)
          self.kspi0h_dd_merged_up = self._makeD2KSPi0h("DD","Merged",True)
          self.kspi0h_dd_resolved_up = self._makeD2KSPi0h("DD","Resolved",True)

          self.kspi0hh_ll_merged_up = self._makeD2KSPi0hh("LL","Merged",True)
          self.kspi0hh_ll_resolved_up = self._makeD2KSPi0hh("LL","Resolved",True)
          self.kspi0hh_dd_merged_up = self._makeD2KSPi0hh("DD","Merged",True)
          self.kspi0hh_dd_resolved_up = self._makeD2KSPi0hh("DD","Resolved",True)

          # PID filtered selections
          self.hh_pid = [filterPID('D2HHPID',self.hh,config_pid)]
          self.hh_pid_tight = [filterPID('D2HHTIGHTPID',self.hh_pid,
                                         config_pid['TIGHT'])]
          self.hhh_pid = [filterPID('D2HHHPID',self.hhh,config_pid)]
          self.hhh_pid_tight = [filterPID('D2HHHPIDTIGHT',self.hhh_pid,
                                          config_pid['TIGHT'])]
          self.hhhh_pid = [filterPID('D2HHHHPID',self.hhhh,config_pid)]
          self.hhhh_pid_tight = [filterPID('D2HHHHPIDTIGHT',self.hhhh_pid,
                                           config_pid['TIGHT'])]
          self.hh_pid_tight_up = [filterPID('D2HHPIDTIGHTUP',self.hh_up,config_pid['TIGHT'])]

          self.ksh_ll_pid    = [filterPID('D2KsHLLPID',self.ksh_ll,config_pid)]
          self.ksh_dd_pid    = [filterPID('D2KsHDDPID',self.ksh_dd,config_pid)]
          self.kshh_ll_pid   = [filterPID('D2KsHHLLPID',self.kshh_ll,config_pid)]
          self.kshh_dd_pid   = [filterPID('D2KsHHDDPID',self.kshh_dd,config_pid)]
          self.kshhh_ll_pid  = [filterPID('D2KsHHHLLPID',self.kshhh_ll,config_pid)]
          self.kshhh_dd_pid  = [filterPID('D2KsHHHDDPID',self.kshhh_dd,config_pid)]

          self.pi0hh_merged_pid     = [filterPID('D2MergedPi0HHPID',self.pi0hh_merged,config_pid)]
          self.pi0hh_resolved_pid   = [filterPID('D2ResolvedPi0HHPID',self.pi0hh_resolved,config_pid)]
          self.pi0hhh_merged_pid    = [filterPID('D2MergedPi0HHHPID',self.pi0hhh_merged,config_pid)]
          self.pi0hhh_resolved_pid  = [filterPID('D2ResolvedPi0HHHPID',self.pi0hhh_resolved,config_pid)]
          self.pi0hhhh_merged_pid   = [filterPID('D2MergedPi0HHHHPID',self.pi0hhhh_merged,config_pid)]
          self.pi0hhhh_resolved_pid = [filterPID('D2ResolvedPi0HHHHPID',self.pi0hhhh_resolved,config_pid)]

          # subset decays
          oneK = "NINTREE(ABSID=='K+') == 1"
          self.kpi = [filterSelection('D2KPi',oneK,self.hh)]
          d_cf = "(((ID=='D+') & (NINTREE(ID=='K-')==1) & (%s)) | "\
                 "((ID=='D-') & (NINTREE(ID=='K+')==1) & (%s)))" % (oneK,oneK)
          ds_cf = "((NINTREE(ID=='K-')==1) & (NINTREE(ID=='K+')==1))"
          self.kpi_pid = [filterSelection('D2KPIPID',oneK,self.hh_pid)]
          self.kpi_pid_tight = [filterSelection('D2KPITIGHTPID',oneK,self.hh_pid_tight)]
          self.kpi_pid_tighter1 = [filterPID('D2KPITIGHTER1PID',self.kpi_pid,config_pid['TIGHTER1'])]

          atLeastOneK = "NINTREE(ABSID=='K+') > 0"
          self.k2h = [filterSelection('D2K2H',atLeastOneK,self.hhh)]
          self.k3h = [filterSelection('D2K3H',atLeastOneK,self.hhhh)]
          self.k2h_pid = [filterPID('D2K2HPID',self.k2h,config_pid)]
          self.k3h_pid = [filterPID('D2K3HPID',self.k3h,config_pid)]
          self.kskhh_ll = [filterSelection('D2KsKHHLL',atLeastOneK,self.kshhh_ll)]
          self.kskhh_dd = [filterSelection('D2KsKHHDD',atLeastOneK,self.kshhh_dd)]
          self.kskhh_ll_pid = [filterPID('D2KsKHHLLPID',self.kskhh_ll,config_pid)]
          self.kskhh_dd_pid = [filterPID('D2KsKHHDDPID',self.kskhh_dd,config_pid)]
          self.pi0khh_merged   = [filterSelection('D2MergedPi0KHH',atLeastOneK,self.pi0hhh_merged)]
          self.pi0khh_resolved = [filterSelection('D2ResolvedPi0KHH',atLeastOneK,self.pi0hhh_resolved)]
          self.pi0khh_merged_pid   = [filterPID('D2MergedPi0KHHPID',self.pi0khh_merged,config_pid)]
          self.pi0khh_resolved_pid = [filterPID('D2ResolvedPi0KHHPID',self.pi0khh_resolved,config_pid)]

          #d0 narrow mass window +-40MeV
          d0_narrow_min,d0_narrow_max = self._massWindow('D0narrow')

          self.kpi_pid_tighter1_narrow = [filterSelection('D2KPITIGHTER1PIDNARROWMW','in_range(%s,MM,%s)'%(d0_narrow_min,d0_narrow_max),self.kpi_pid_tighter1)]
          self.kpi_pid_tight_up = [filterSelection('D2KPITIGHTPIDUP',oneK,self.hh_pid_tight_up)]
          self.k3pi = [filterSelection('D2K3PI',oneK,self.hhhh)]
          self.k3pi_up = [filterSelection('D2K3PIUP',oneK,self.hhhh_up)]
          self.k3pi_pid = [filterPID('D2K3PIPID',self.k3pi,config_pid)]
          self.k3pi_pid_tight = [filterPID('D2K3PIPIDTIGHT',self.k3pi,config_pid['TIGHT'])]
          self.k3pi_pid_tighter1 = [filterPID('D2K3PIPIDTIGHTER1',self.k3pi,config_pid['TIGHTER1'])]
          self.k3pi_pid_tighter1_narrow = [filterSelection('D2K3PIPIDTIGHTER1NARROWMW','in_range(%s,MM,%s)'%(d0_narrow_min,d0_narrow_max),self.k3pi_pid_tighter1)]
          self.k3pi_pid_tight_up = [filterPID('D2K3PIPIDTIGHTUP',self.k3pi_up,config_pid['TIGHT'])]
          self.d0_cf_pid = [MergedSelection('D0CFPIDBeauty2Charm',
                                            RequiredSelections=self.kpi_pid+self.k3pi_pid)]
          self.pi0kpi_merged = [filterSelection('D2Pi0KPi_Merged',oneK,
                                                self.pi0hh_merged)]
          self.pi0kpi_resolved = [filterSelection('D2Pi0KPi_Resolved',oneK,
                                                  self.pi0hh_resolved)]
          d_min,d_max,ds_min,ds_max = self._massWindowD2HHHCF()

          #use this below
          d_cf_noMassWin = d_cf
          d_cf = LoKiCuts.combine([d_cf,"in_range(%s,MM,%s)"%(d_min,d_max)])
          ds_cf = LoKiCuts.combine([ds_cf,"in_range(%s,MM,%s)"%(ds_min,ds_max)])
          cf = '('+d_cf+') | (' + ds_cf + ')'
          self.hhh_cf_pid = [filterSelection('D2HHHCFPID',cf,self.hhh_pid)]
          ds = "(((NINTREE(ID=='K-')==1) & (NINTREE(ID=='K+')==1)) |"
          ds += "(NINTREE(ABSID=='K+')==0) | "
          ds += "((ID=='D+') & (NINTREE(ID=='K+')==1) & (%s)) | "\
                "((ID=='D-') & (NINTREE(ID=='K-')==1) & (%s)))" % (oneK,oneK)
          ds = LoKiCuts.combine([ds,"in_range(%s,MM,%s)"%(ds_min,ds_max)])
          #print 'ds =', ds
          self.ds_hhh_pid_tight = [filterSelection('Ds2HHHPIDTIGHT',ds,
                                                   self.hhh_pid_tight)]
          # WS decays
          self.kshh_ll_ws = self._makeD2KShhWS("LL")
          self.kshh_dd_ws = self._makeD2KShhWS("DD")
          self.hh_ws = self._makeD2hhWS()
          self.pi0hh_merged_ws = self._makeD2Pi0hhWS('Merged')
          self.pi0hh_resolved_ws = self._makeD2Pi0hhWS('Resolved')
          self.hhhh_ws = self._makeD2hhhhWS()

          # phi mu nu
          self.phimu = self._makeD2PhiMuNu()

          #Different subsets or selections for FULL DST lines
          #Different PID filtering
          self.hhh_pid_tightpi = [filterPID('D2HHHPIDTIGHTPI',self.hhh_pid,config_pid['TIGHTPI'])]
          self.hhh_pid_tightpi_up = [filterPID('D2HHHPIDTIGHTPIUP',self.hhh_up,config_pid['TIGHTPI'])]
          self.hhh_pid_tighter = [filterPID('D2HHHPIDTIGHTER',self.hhh_pid,config_pid['TIGHTER'])]
          self.hhh_pid_special = [filterPID('D2HHHPIDSPECIAL',self.hhh,config_pid['SPECIAL'])]
          self.hhh_pid_specialpi = [filterPID('D2HHHPIDSPECIALPI',self.hhh,config_pid['SPECIALPI'])]

          self.hh_pid_tight = [filterPID('D2HHPIDTIGHT',self.hh_pid,config_pid['TIGHT'])]

          kkpi = "((NINTREE(ID=='K-')==1) & (NINTREE(ID=='K+')==1))"
          self.kkpi_pid_tightpi = [filterSelection('D2KKPiPIDTIGHTPI',kkpi,self.hhh_pid_tightpi)]
          self.kkpi_pid_specialpi = [filterSelection('D2KKPiPIDSPECIALPI',kkpi,self.hhh_pid_specialpi)]

          #filter fo d2kkpi
          myPreAmble = [ "qPion = CHILD(Q,'pi+' == ABSID)",
                         "qKaonP = CHILD(Q, 'K+' == ID)",
                         "qKaonM = CHILD(Q, 'K-' == ID)",
                         "px_pi = CHILD(PX, 'pi+' == ABSID ) " ,
                         "py_pi = CHILD(PY, 'pi+' == ABSID ) " ,
                         "pz_pi = CHILD(PZ, 'pi+' == ABSID ) " ,
                         "e_pi = CHILD(E, 'pi+' == ABSID  ) " ,
                         "px_kp = CHILD(PX, 'K+' == ID ) " ,
                         "py_kp = CHILD(PY, 'K+' == ID ) " ,
                         "pz_kp = CHILD(PZ, 'K+' == ID ) " ,
                         "e_kp = CHILD(E, 'K+' == ID ) " ,
                         "pid_kp = CHILD(PIDK, 'K+' == ID ) " ,
                         "px_km = CHILD(PX, 'K-' == ID ) " ,
                         "py_km = CHILD(PY, 'K-' == ID ) " ,
                         "pz_km = CHILD(PZ, 'K-' == ID ) " ,
                         "e_km = CHILD(E, 'K-' == ID ) " ,
                         "pid_km = CHILD(PIDK, 'K-' == ID ) " ,
                         "px_KK = px_kp + px_km ",
                         "py_KK = py_kp + py_km ",
                         "pz_KK = pz_kp + pz_km ",
                         "e_KK = e_kp + e_km" ,
                         "m_KK = sqrt ( e_KK**2 - px_KK**2 - py_KK**2 - pz_KK**2 ) ",
                         "px_Kpi1 = px_kp + px_pi " ,
                         "py_Kpi1 = py_kp + py_pi " ,
                         "pz_Kpi1 = pz_kp + pz_pi " ,
                         "e_Kpi1 = e_kp + e_pi " ,
                         "m_KpPim = sqrt ( e_Kpi1**2 - px_Kpi1**2 - py_Kpi1**2 - pz_Kpi1**2 ) ",
                         "px_Kpi2 = px_km + px_pi " ,
                         "py_Kpi2 = py_km + py_pi " ,
                         "pz_Kpi2 = pz_km + pz_pi " ,
                         "e_Kpi2 = e_km + e_pi " ,
                         "m_KmPip = sqrt ( e_Kpi2**2 - px_Kpi2**2 - py_Kpi2**2 - pz_Kpi2**2 ) "
                         ]

          D1isDs2KKPi = "( ((pid_km>0) & (pid_kp>0)) | (m_KK<1040) | ((qPion*qKaonP<0) & (abs(m_KpPim-892)<100)) | ((qPion*qKaonM<0) & (abs(m_KmPip-892)<100) )  )"

          #self.kkpi_custom = [filterSelection('D2KKPiCUSTOM',D1isDs2KKPi,self.kkpi_pid_tightpi,myPreAmble)]
          self.kkpi_custom = [filterSelection('D2KKPiCUSTOM',D1isDs2KKPi,self.kkpi_pid_specialpi,myPreAmble)]

          #for use with the FULL DST B --> Ds 3H lines

          #ds non CF selection = (Ds+ --> K+pi+pi-) | (Ds+ --> 3pi)
          ds_pipipi = "(NINTREE(ABSID=='K+')==0)"
          ds_kpipi = "((ID=='D+') & (NINTREE(ID=='K+')==1) & (%s)) | "\
                     "((ID=='D-') & (NINTREE(ID=='K-')==1) & (%s))" % (oneK,oneK)

          self.ds_hhh_cf_custom = [filterSelection('Ds2HHHCFCUSTOM',"in_range(%s,MM,%s)"%(ds_min,ds_max),self.kkpi_custom)]

          self.ds_pipipi_pid_tightpi = [filterSelection('Ds2PiPiPiPIDTIGHTPI',ds_pipipi,self.hhh_pid_tightpi)]
          self.ds_kpipi_pid_special = [filterSelection('Ds2KPiPiPIDSPECIAL',ds_kpipi,self.hhh_pid_special)]


          self.ds_hhh_pid_custom = [MergedSelection('Ds2HHHPIDCUSTOMBeauty2Charm',
                                                    RequiredSelections=self.ds_pipipi_pid_tightpi+self.ds_kpipi_pid_special+self.ds_hhh_cf_custom)]

          #this spans the whole mass range covered by D+ and Ds (comes from self.hhh)
          #either d(s) --> pipipi kpipi or kkpi
          self.hhh_pid_custom = [MergedSelection('D2HHHPIDCUSTOMBeauty2Charm',
                                                 RequiredSelections=self.ds_pipipi_pid_tightpi+self.ds_kpipi_pid_special+self.kkpi_custom)]

          #for use with the FULL DST B --> DD line
          #add D+ --> K+K-pi+ to the d_cf selection
          #remove trailing bracket to add this charge combo in
          #d_cf_plus = d_cf_noMassWin[:-1] + " | ((NINTREE(ID=='K-')==1) & (NINTREE(ID=='K+')==1)))"
          #d_cf_plus = LoKiCuts.combine([d_cf_plus,"in_range(%s,MM,%s)"%(d_min,d_max)])

          #d_kkpi_custom = [filterSelection('Dplus2KKPiCUSTOM',"in_range(%s,MM,%s)"%(d_min,d_max),self.kkpi_custom)]
          self.d_cf_hhh_pid_tightpi = [filterSelection('Dplus2HHHCF',d_cf,self.hhh_pid_tightpi)]
          #self.d_hhh_custom = [MergedSelection('Dplus2HHHCUSTOMBeauty2Charm',RequiredSelections=d_kkpi_custom+self.d_cf_hhh_pid_tightpi)]
          self.d_cf_hhh_pid_tightpi_up = [filterSelection('Dplus2HHHCFUP',d_cf,self.hhh_pid_tightpi_up)]
          #self.d_hhh_4_B2DD_custom = [MergedSelection('D2HHH_4_B2DD_CUSTOM_Beauty2Charm',
          #                                            RequiredSelections=self.ds_hhh_pid_custom+self.d_hhh_custom)]
          self.d_hhh_4_B2DD_custom = [MergedSelection('D2HHH_4_B2DD_CUSTOM_Beauty2Charm',
                                                      RequiredSelections=self.hhh_pid_custom+self.d_cf_hhh_pid_tightpi)]
          #for use with FULL DST D0D0 line
          self.d0_cf_pid_tight = [filterPID('D0CFPIDTIGHT',
                                            self.d0_cf_pid,config_pid['TIGHT'])]
          self.d0_cf_pid_tight_up = [MergedSelection('D0CFPIDTIGHTUPBeauty2Charm',
                                                     RequiredSelections=self.kpi_pid_tight_up+self.k3pi_pid_tight_up)]

     def _makeD2TwoBody(self,name,decays,wm,up,config,extrainputs):
          ''' Makes all D -> HH selections.'''
          if up:
               wm += '& (ANUM(ISUP)==1)'
               name += 'UP'
          comboCuts = [LoKiCuts(['ASUMPT'],config).code(),wm,hasTopoChild()]
          comboCuts.append(LoKiCuts(['ADOCA12'],config).code())
          comboCuts = LoKiCuts.combine(comboCuts)
          momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
          cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                                DecayDescriptors=decays)
          return  Selection('Proto'+name+'Beauty2Charm',Algorithm=cp,
                            RequiredSelections=[self.pions]+extrainputs)

     def _makeD2ThreeBody(self,name,decays,wm,up,config,extrainputs):
          ''' Makes all D -> HHH selections.'''
          if up:
               wm += '& (ANUM(ISUP)==1)'
               name += 'UP'
          comboCuts = [LoKiCuts(['ASUMPT'],config).code(),wm,hasTopoChild()]
          comboCuts12 = [LoKiCuts(['ADOCA12'],config).code()]
          comboCuts.append(LoKiCuts(['ADOCA13'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA23'],config).code())
          comboCuts = LoKiCuts.combine(comboCuts)
          comboCuts12 = LoKiCuts.combine(comboCuts12)
          momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
          cp = DaVinci__N3BodyDecays(Combination12Cut=comboCuts12,
                                     CombinationCut=comboCuts,MotherCut=momCuts,
                                     DecayDescriptors=decays)
          return  Selection('Proto'+name+'Beauty2Charm',Algorithm=cp,
                            RequiredSelections=[self.pions]+extrainputs)

     def _makeD2FourBody(self,name,decays,wm,up,config,extrainputs):
          ''' Makes all D -> HHHH selections.'''
          if up:
               wm += '& (ANUM(ISUP)==1)'
               name += 'UP'
          comboCuts = [LoKiCuts(['ASUMPT'],config).code(),wm,hasTopoChild()]
          comboCuts12 = [LoKiCuts(['ADOCA12'],config).code()]
          comboCuts123 = ["AALL"]
          comboCuts123.append(LoKiCuts(['ADOCA13'],config).code())
          comboCuts123.append(LoKiCuts(['ADOCA23'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA14'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA24'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA34'],config).code())
          comboCuts = LoKiCuts.combine(comboCuts)
          comboCuts12 = LoKiCuts.combine(comboCuts12)
          comboCuts123 = LoKiCuts.combine(comboCuts123)
          momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
          cp = DaVinci__N4BodyDecays(Combination12Cut=comboCuts12,
                                     Combination123Cut=comboCuts123,
                                     CombinationCut=comboCuts,MotherCut=momCuts,
                                     DecayDescriptors=decays)
          return  Selection('Proto'+name+'Beauty2Charm',Algorithm=cp,
                            RequiredSelections=[self.pions]+extrainputs)

     def _makeD2FiveBody(self,name,decays,wm,up,config,extrainputs):
          ''' Makes all D -> HHHHH selections.'''
          if up:
               wm += '& (ANUM(ISUP)==1)'
               name += 'UP'
          comboCuts = [LoKiCuts(['ASUMPT'],config).code(),wm,hasTopoChild()]
          comboCuts12 = [LoKiCuts(['ADOCA12'],config).code()]
          comboCuts123 = ["AALL"]
          comboCuts123.append(LoKiCuts(['ADOCA13'],config).code())
          comboCuts123.append(LoKiCuts(['ADOCA23'],config).code())
          comboCuts1234 = ["AALL"]
          comboCuts1234.append(LoKiCuts(['ADOCA14'],config).code())
          comboCuts1234.append(LoKiCuts(['ADOCA24'],config).code())
          comboCuts1234.append(LoKiCuts(['ADOCA34'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA15'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA25'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA35'],config).code())
          comboCuts.append(LoKiCuts(['ADOCA45'],config).code())
          comboCuts     = LoKiCuts.combine(comboCuts)
          comboCuts12   = LoKiCuts.combine(comboCuts12)
          comboCuts123  = LoKiCuts.combine(comboCuts123)
          comboCuts1234 = LoKiCuts.combine(comboCuts1234)
          momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
          cp = DaVinci__N5BodyDecays(Combination12Cut=comboCuts12,
                                     Combination123Cut=comboCuts123,
                                     Combination1234Cut=comboCuts1234,
                                     CombinationCut=comboCuts,MotherCut=momCuts,
                                     DecayDescriptors=decays)
          return  Selection('Proto'+name+'Beauty2Charm',Algorithm=cp,
                            RequiredSelections=[self.pions]+extrainputs)

     def _massWindow(self,which):
          dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
          if which == 'D0':
               min = 1864.84 - dm # D0 - dm
               max = 1864.84 + dm # D0 + dm
          elif which == 'D0wide' :
               min = 1864.84 - dm - 150.
               max = 1864.84 + dm + 150.
          elif which == 'D0narrow' :
               min = 1864.84 - dm + 60.
               max = 1864.84 + dm - 60.
          else:
               min = 1869.62 - dm # D+ - dm
               max = 1968.49 + dm # Ds+ + dm
          return ('%s*%s'%(min,units),'%s*%s'%(max,units))

     def _massWindowD2HHHCF(self):
          dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
          d_min = 1869.62 - dm
          d_max = 1869.62 + dm
          ds_min = 1968.49 - dm
          ds_max = 1968.49 + dm
          return ('%s*%s'%(d_min,units),'%s*%s'%(d_max,units),
                  '%s*%s'%(ds_min,units),'%s*%s'%(ds_max,units))

     def _makeD2hh(self,up=False):
          '''Makes D->hh'''
          min,max  = self._massWindow('D0')
          decays = [['pi+','pi-'],['pi+','K-'],['K+','pi-'],['K+','K-']]
          wm = awmFunctor(decays,min,max)
          protoD2hh = self._makeD2TwoBody('D2HH',['D0 -> pi+ pi-'],wm,up,self.config,
                                    [self.uppions] if up else [])
          name = 'D2HH'
          if up: name += 'UP'
          return [subPIDSels(decays,name,'',min,max,[protoD2hh])]

     def _makeD2hhWS(self,up=False):
          '''Makes D->hh WS'''
          tag = ''
          if up: tag = 'UP'
          min,max  = self._massWindow('D0')
          decays = [['pi+','pi+'],['pi+','K+'],['K+','pi+'],['K+','K+']]
          wm = awmFunctor(decays,min,max)
          psel = self._makeD2TwoBody('D2HHWSPlus',['D0 -> pi+ pi+'],wm,up,self.config,
                                     [self.uppions] if up else [])
          psel = subPIDSels(decays,'D2HHWSPlus'+tag,'',min,max,[psel])
          msel = self._makeD2TwoBody('D2HHWSMinus',['D0 -> pi- pi-'],wm,up,self.config,
                                     [self.uppions] if up else [])
          msel = subPIDSels(getCCs(decays),'D2HHWSMinus'+tag,'',min,max,[msel])
          return [MergedSelection('D2HHWSBeauty2Charm'+tag,
                                  RequiredSelections=[psel,msel])]

     ## # Implementation using one instance of SubPID for all subsitutions
     ## def _makeD2hhh(self,up=False):
     ##     '''Makes D->hhh'''
     ##     tag = ''
     ##     if up: tag = 'UP'
     ##     min,max = self._massWindow('D+')
     ##     decays = [['pi+','pi+','pi-'],['pi+','pi+','K-'],
     ##               ['K+', 'pi+','pi-'],['K+', 'pi+','K-'],
     ##               ['pi+','K+', 'pi-'],['pi+','K+', 'K-'],
     ##               ['K+', 'K+', 'pi-'],['K+', 'K+', 'K-']]
     ##     wm = awmFunctor(decays,min,max)
     ##     protoDp2hhh = self._makeD2ThreeBody('D+2HHH',['D+ -> pi+ pi+ pi-'],wm,up,
     ##                                         self.config, [self.uppions] if up else [])
     ##     psels = subPIDSels(decays,'D+2HHH'+tag,'',min,max,[protoDp2hhh])
     ##     protoDm2hhh = self._makeD2ThreeBody('D-2HHH',['D- -> pi- pi- pi+'],wm,up,
     ##                                         self.config, [self.uppions] if up else [])
     ##     msels = subPIDSels(getCCs(decays),'D-2HHH'+tag,'',min,max,[protoDm2hhh])
     ##     return [MergedSelection('D2HHHBeauty2Charm'+tag,RequiredSelections=[psels,msels])]

     # Implementation using one instance of SubPID per substitution
     def _makeD2hhh(self,up=False):
          '''Makes D->hhh'''

          tag = ''
          if up: tag = 'UP'

          min,max = self._massWindow('D+')

          extrainputs = [self.uppions] if up else []

          decaysp = [['pi+','pi+','pi-'],['pi+','pi+','K-'],
                     ['K+', 'pi+','pi-'],['K+', 'pi+','K-'],
                     ['pi+','K+', 'pi-'],['pi+','K+', 'K-'],
                     ['K+', 'K+', 'pi-'],['K+', 'K+', 'K-']]
          decaysm = getCCs(decaysp)

          sp = self._makeD2ThreeBody('D+2HHH',['D+ -> pi+ pi+ pi-'],
                                     awmFunctor(decaysp,min,max),
                                     up, self.config, extrainputs)
          sm = self._makeD2ThreeBody('D-2HHH',['D- -> pi- pi- pi+'],
                                     awmFunctor(decaysm,min,max),
                                     up, self.config, extrainputs)

          sels = []

          for dec in decaysp :
               name = makeSelName('D+',dec)
               sels += [ subPIDSels([dec],name,tag,min,max,[sp]) ]
          for dec in decaysm :
               name = makeSelName('D-',dec)
               sels += [ subPIDSels([dec],name,tag,min,max,[sm]) ]

          return [MergedSelection('D2HHHBeauty2Charm'+tag,RequiredSelections=sels)]

     def _makeD2KSh(self,which,up=False):
          '''Makes D->Ksh'''
          tag = ''
          if up: tag = 'UP'
          min,max  = self._massWindow('D+')
          decays = [['KS0','pi+'],['KS0','K+']]
          wm = awmFunctor(decays,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA12_MAX')
          if up : extrainputs = self.ks[which] + [self.uppions]
          else: extrainputs = self.ks[which]
          protoDp2Ksh = self._makeD2TwoBody('D+2KsH_'+which,['D+ -> KS0 pi+'],wm,up,config,extrainputs)
          psels = subPIDSels(decays,'D+2KsH'+tag,which,min,max,[protoDp2Ksh])
          protoDm2Ksh = self._makeD2TwoBody('D-2KsH_'+which,['D- -> KS0 pi-'],wm,up,config,extrainputs)
          msels = subPIDSels(getCCs(decays),'D-2KsH'+tag,which,min,max,[protoDm2Ksh])
          return [MergedSelection('D2KsHBeauty2Charm_%s%s'%(which,tag),
                                  RequiredSelections=[psels,msels])]

     def _makeD2KstarKs(self,which):
          min,max = self._massWindow('D+')
          decaysp = [["K*(892)+","KS0"]]
          wmp = awmFunctor(decaysp,min,max)
          decaysm = [["K*(892)-","KS0"]]
          wmm = awmFunctor(decaysm,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA12_MAX')
          inputs = self.hhBuilder.kspi + self.hhBuilder.kstarpm + self.ks[which]
          comboCutsp = [LoKiCuts(['ASUMPT'],config).code(),wmp]
          comboCutsp = LoKiCuts.combine(comboCutsp)
          comboCutsm = [LoKiCuts(['ASUMPT'],config).code(),wmm]
          comboCutsm = LoKiCuts.combine(comboCutsm)
          momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
          cpp = CombineParticles(CombinationCut=comboCutsp,MotherCut=momCuts,
                                 DecayDescriptors=['D+ -> K*(892)+ KS0'])
          cpm = CombineParticles(CombinationCut=comboCutsm,MotherCut=momCuts,
                                 DecayDescriptors=['D- -> K*(892)- KS0'])
          selp = Selection('ProtoD+2KstKs'+which+'Beauty2Charm',Algorithm=cpp,RequiredSelections=inputs)
          selm = Selection('ProtoD-2KstKs'+which+'Beauty2Charm',Algorithm=cpm,RequiredSelections=inputs)
          return [ MergedSelection('D2KstKsBeauty2Charm'+which,RequiredSelections=[selp,selm]) ]

     def _makeD2KstarKstar0(self):
          min,max = self._massWindow('D+')
          decaysp = [["K*(892)+","K*(892)~0"]]
          wmp = awmFunctor(decaysp,min,max)
          decaysm = [["K*(892)-","K*(892)0"]]
          wmm = awmFunctor(decaysm,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA12_MAX')
          inputs = self.hhBuilder.kspi + self.hhBuilder.kstarpm + self.hhBuilder.kstar0
          comboCutsp = [LoKiCuts(['ASUMPT'],config).code(),wmp]
          comboCutsp = LoKiCuts.combine(comboCutsp)
          comboCutsm = [LoKiCuts(['ASUMPT'],config).code(),wmm]
          comboCutsm = LoKiCuts.combine(comboCutsm)
          momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
          #dauCuts = { "K*(892)+"  : hasTopoChild(),
          #            "K*(892)-"  : hasTopoChild(),
          #            "K*(892)~0" : hasTopoChild(),
          #            "K*(892)0"  : hasTopoChild() }
          cpp = CombineParticles(CombinationCut=comboCutsp,MotherCut=momCuts,
                                 #DaughtersCuts=dauCuts,
                                 DecayDescriptors=['D+ -> K*(892)+ K*(892)~0'])
          cpm = CombineParticles(CombinationCut=comboCutsm,MotherCut=momCuts,
                                 #DaughtersCuts=dauCuts,
                                 DecayDescriptors=['D- -> K*(892)- K*(892)0'])
          selp = Selection('ProtoD+2KstKst0Beauty2Charm',Algorithm=cpp,RequiredSelections=inputs)
          selm = Selection('ProtoD-2KstKst0Beauty2Charm',Algorithm=cpm,RequiredSelections=inputs)
          return [ MergedSelection('D2KstKst0Beauty2Charm',RequiredSelections=[selp,selm]) ]

     ## # Implementation using one instance of SubPID for all substitutions
     ## def _makeD2Kstarhh(self,up=False):
     ##      '''Makes D-> K*+(->Ksh,Kpi0) hh'''
     ##      tag = ''
     ##      if up: tag = 'UP'
     ##      min,max = self._massWindow('D+')
     ##      decaysp = [['pi+','pi-','K*(892)+'],['pi+','K-','K*(892)+'],
     ##                 ['K+', 'pi-','K*(892)+'],['K+', 'K-','K*(892)+']]
     ##      wmp = awmFunctor(decaysp,min,max)
     ##      decaysm = [['pi+','pi-','K*(892)-'],['pi+','K-','K*(892)-'],
     ##                 ['K+', 'pi-','K*(892)-'],['K+', 'K-','K*(892)-']]
     ##      wmm = awmFunctor(decaysm,min,max)
     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA13_MAX')
     ##      config.pop('ADOCA23_MAX')
     ##      if up : extrainputs = self.hhBuilder.kspi + self.hhBuilder.kstarpm + [self.uppions]
     ##      else:   extrainputs = self.hhBuilder.kspi + self.hhBuilder.kstarpm
     ##      protoDp2Ksthh = self._makeD2ThreeBody('D+2KstHH',['D+ -> pi+ pi- K*(892)+'],
     ##                                            wmp,up,config,extrainputs)
     ##      psels = subPIDSels(decaysp,'D+2KstHH',tag,min,max,[protoDp2Ksthh])
     ##      protoDm2Ksthh = self._makeD2ThreeBody('D-2KstHH',['D- -> pi+ pi- K*(892)-'],
     ##                                            wmm,up,config,extrainputs)
     ##      msels = subPIDSels(decaysm,'D-2KstHH',tag,min,max,[protoDm2Ksthh])
     ##      return [MergedSelection('D2KstHHBeauty2Charm_%s'%tag,
     ##                              RequiredSelections=[psels,msels])]

     # Implementation using one instance of SubPID per substitution
     def _makeD2Kstarhh(self,up=False):
          '''Makes D-> K*+(->Ksh,Kpi0) hh'''

          tag = ''
          if up: tag = 'UP'

          min,max = self._massWindow('D+')

          config = deepcopy(self.config)
          config.pop('ADOCA13_MAX')
          config.pop('ADOCA23_MAX')

          if up : extrainputs = self.hhBuilder.kspi + self.hhBuilder.kstarpm + [self.uppions]
          else:   extrainputs = self.hhBuilder.kspi + self.hhBuilder.kstarpm

          decaysp = [['pi+','pi-','K*(892)+'],['pi+','K-','K*(892)+'],
                     ['K+', 'pi-','K*(892)+'],['K+', 'K-','K*(892)+']]

          decaysm = [['pi+','pi-','K*(892)-'],['pi+','K-','K*(892)-'],
                     ['K+', 'pi-','K*(892)-'],['K+', 'K-','K*(892)-']]

          ps = self._makeD2ThreeBody('D+2HHKst',['D+ -> pi+ pi- K*(892)+'],
                                     awmFunctor(decaysp,min,max),
                                     up,config,extrainputs)

          ms = self._makeD2ThreeBody('D-2HHKst',['D- -> pi+ pi- K*(892)-'],
                                     awmFunctor(decaysm,min,max),
                                     up,config,extrainputs)

          sels = [ ]

          for dec in decaysp :
               name = makeSelName('D+',dec)
               sels += [ subPIDSels([dec],name,tag,min,max,[ps]) ]

          for dec in decaysm :
               name = makeSelName('D-',dec)
               sels += [ subPIDSels([dec],name,tag,min,max,[ms]) ]

          return [MergedSelection('D2KstHHBeauty2Charm_%s'%tag,RequiredSelections=sels)]

     def _makeD2KShh(self,which,up=False):
          '''Makes D->Kshh'''
          min,max = self._massWindow('D0')
          decays = [['pi+','pi-','KS0'],['pi+','K-','KS0'],
                    ['K+', 'pi-','KS0'],['K+', 'K-','KS0']]
          wm = awmFunctor(decays,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA13_MAX')
          config.pop('ADOCA23_MAX')
          if up : extrainputs = self.ks[which] +  [self.uppions]
          else: extrainputs = self.ks[which]
          protoD2Kshh = self._makeD2ThreeBody('D2KSHH'+which,['D0 -> pi+ pi- KS0'],
                                              wm,up,config,extrainputs)
          name = 'D2KsHH'
          if up: name += 'UP'
          return [subPIDSels(decays,name,which,min,max,[protoD2Kshh])]

     ## # Implementation using one instance of SubPID for all substitutions
     ## def _makeD2KShhh(self,which,up=False):
     ##      '''Makes D->Kshhh'''
     ##      tag = ''
     ##      if up: tag = 'UP'
     ##      min,max  = self._massWindow('D+')
     ##      decays = [['pi+','pi-','pi+','KS0'],
     ##                ['K+', 'pi-','pi+','KS0'],['pi+','K-', 'pi+','KS0'],['pi+','pi-','K+','KS0'],
     ##                ['K+', 'K-', 'pi+','KS0'],['K+', 'pi-','K+', 'KS0'],['pi+','K-', 'K+','KS0'],
     ##                ['K+', 'K-', 'K+', 'KS0']]
     ##      wm = awmFunctor(decays,min,max)
     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA14_MAX')
     ##      config.pop('ADOCA24_MAX')
     ##      config.pop('ADOCA34_MAX')
     ##      if up : extrainputs = self.ks[which] + [self.uppions]
     ##      else: extrainputs = self.ks[which]
     ##      protoDp2Kshhh = self._makeD2FourBody('D+2KsHHH_'+which,['D+ -> pi+ pi- pi+ KS0'],
     ##                                           wm,up,config,extrainputs)
     ##      psels = subPIDSels(decays,'D+2KsHHH'+tag,which,min,max,[protoDp2Kshhh])
     ##      protoDm2Kshhh = self._makeD2FourBody('D-2KsHHH_'+which,['D- -> pi- pi+ pi- KS0'],
     ##                                           wm,up,config,extrainputs)
     ##      msels = subPIDSels(getCCs(decays),'D-2KsHHH'+tag,which,min,max,[protoDm2Kshhh])
     ##      return [MergedSelection('D2KsHHHBeauty2Charm_%s%s'%(which,tag),
     ##                              RequiredSelections=[psels,msels])]

     # Implementation using one instance of SubPID per substitution
     def _makeD2KShhh(self,which,up=False):
          '''Makes D->Kshhh'''

          tag = ''
          if up: tag = 'UP'

          min,max  = self._massWindow('D+')

          decaysp = [['pi+','pi-','pi+','KS0'],
                     ['K+', 'pi-','pi+','KS0'],['pi+','K-', 'pi+','KS0'],['pi+','pi-','K+','KS0'],
                     ['K+', 'K-', 'pi+','KS0'],['K+', 'pi-','K+', 'KS0'],['pi+','K-', 'K+','KS0'],
                     ['K+', 'K-', 'K+', 'KS0']]
          decaysm = getCCs(decaysp)

          config = deepcopy(self.config)
          config.pop('ADOCA14_MAX')
          config.pop('ADOCA24_MAX')
          config.pop('ADOCA34_MAX')

          if up : extrainputs = self.ks[which] + [self.uppions]
          else:   extrainputs = self.ks[which]

          sp = self._makeD2FourBody('D+2KsHHH_'+which,['D+ -> pi+ pi- pi+ KS0'],
                                    awmFunctor(decaysp,min,max),
                                    up,config,extrainputs)
          sm = self._makeD2FourBody('D-2KsHHH_'+which,['D- -> pi- pi+ pi- KS0'],
                                    awmFunctor(decaysm,min,max),
                                    up,config,extrainputs)

          sels = []

          for dec in decaysp :
               name = makeSelName('D+',dec,tag)
               sels += [ subPIDSels([dec],name,which,min,max,[sp]) ]
          for dec in decaysm :
               name = makeSelName('D-',dec,tag)
               sels += [ subPIDSels([dec],name,which,min,max,[sm]) ]

          return [MergedSelection('D2KsHHHBeauty2Charm_%s%s'%(which,tag),RequiredSelections=sels)]

     def _makeD2KShhWS(self,which,up=False):
          '''Makes D->Kshh WS'''
          tag = ''
          if up: tag = 'UP'
          min,max = self._massWindow('D0')
          decays = [['pi+','pi+','KS0'],['pi+','K+','KS0'],
                    ['K+','pi+','KS0'],['K+','K+','KS0']]
          wm = awmFunctor(decays,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA13_MAX')
          config.pop('ADOCA23_MAX')
          if up : extrainputs = self.ks[which] + [self.uppions]
          else : extrainputs = self.ks[which]
          protoPlus = self._makeD2ThreeBody('D2KSHHWSPlus'+which,['D0 -> pi+ pi+ KS0'],
                                    wm,up,config,extrainputs)
          psels = subPIDSels(decays,'D2KSHHWSPlus'+tag,which,min,max,[protoPlus])
          protoMinus = self._makeD2ThreeBody('D2KSHHWSMinus'+which,['D0 -> pi- pi- KS0'],
                                     wm,up,config,extrainputs)
          msels = subPIDSels(getCCs(decays),'D2KSHHWSMinus'+tag,which,min,max,
                             [protoMinus])
          return [MergedSelection('D2KsHHWSBeauty2Charm_%s%s'%(which,tag),
                                  RequiredSelections=[psels,msels])]

     # CRJ : To be improved. Will have hideous combinatorics if used ...
     def _makeD2Pi0hhhh(self,which,up=False):
          '''Makes D->Pi0hhhh'''
          min,max = self._massWindow('D0wide')
          decays = [
              ['pi+','pi-','pi+','pi-','pi0'],
              ['pi+','pi-','pi+','K-','pi0'],['pi+','pi-','K+','pi-','pi0'],
              ['pi+','K-','pi+','pi-','pi0'],['K+','pi-','pi+','pi-','pi0'],
              ['pi+','pi-','K+','K-','pi0'],['pi+','K-','pi+','K-','pi0'],
              ['K+','pi-','pi+','K-','pi0'],
              ['pi+','K-','K+','K-','pi0'],['K+','pi-','K+','K-','pi0'],
              ['K+','K-','K+','pi-','pi0'],
              ['K+','K-','K+','K-','pi0']
              ]
          wm = awmFunctor(decays,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA15_MAX')
          config.pop('ADOCA25_MAX')
          config.pop('ADOCA35_MAX')
          config.pop('ADOCA45_MAX')
          if up : extrainputs = self.pi0[which]+ [self.uppions]
          else : extrainputs = self.pi0[which]
          protoD2pi0hh = self._makeD2FiveBody('D2Pi0HHHH_'+which,['D0 -> pi+ pi- pi+ pi- pi0'],
                                              wm,up,config,extrainputs)
          name = 'D2Pi0HHHH'
          if up: name += 'UP'
          return [subPIDSels(decays,name,which,min,max,[protoD2pi0hh])]

     ## Method not using SubPID st all
     def _makeD2Pi0hhh(self,which,up=False):
          '''Makes D->Pi0hhh'''
          tag = ''
          if up: tag = 'UP'

          min,max = self._massWindow('D+')

          config = deepcopy(self.config)
          config.pop('ADOCA14_MAX')
          config.pop('ADOCA24_MAX')
          config.pop('ADOCA34_MAX')

          if up : extrainputs = self.pi0[which] + [self.kaons] + [self.uppions] + [self.upkaons]
          else  : extrainputs = self.pi0[which] + [self.kaons]

          decaysp = [['pi+','pi+','pi-','pi0'],
                     ['K+', 'pi+','pi-','pi0'],
                     ['K+', 'K+', 'pi-','pi0'],
                     ['K+', 'pi+','K-', 'pi0'],
                     ['pi+','pi+','K-', 'pi0'],
                     ['K+', 'K+', 'K-', 'pi0']]
          decaysm = getCCs(decaysp)
          
          sels = [ ]

          for dec in decaysp :
               name = makeSelName('D+',dec,which)
               sel  = makeSel('D+',dec)
               sels += [ self._makeD2FourBody( name, [sel], awmFunctor([dec],min,max),
                                               up,config,extrainputs ) ]

          for dec in decaysm:
               name = makeSelName('D-',dec,which)
               sel  = makeSel('D-',dec)
               sels += [ self._makeD2FourBody( name, [sel], awmFunctor([dec],min,max),
                                               up,config,extrainputs ) ]

          return [MergedSelection('D2Pi0HHHBeauty2Charm_%s%s'%(which,tag),RequiredSelections=sels)]

     ## # Implementation using one instance of Sub PID for all substitutions
     ## def _makeD2Pi0hhh(self,which,up=False):
     ##      '''Makes D->Pi0hhh'''
     ##      tag = ''
     ##      if up: tag = 'UP'

     ##      min,max = self._massWindow('D+')

     ##      decaysp = [['pi+','pi+','pi-','pi0'],['K+', 'pi+','pi-','pi0'],
     ##                 ['pi+','K+', 'pi-','pi0'],['K+', 'K+', 'pi-','pi0'],
     ##                 ['K+', 'pi+','K-', 'pi0'],['pi+','pi+','K-', 'pi0'],
     ##                 ['K+', 'K+', 'K-', 'pi0'],['pi+','K+', 'K-', 'pi0']]
     ##      decaysm = getCCs(decaysp)

     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA14_MAX')
     ##      config.pop('ADOCA24_MAX')
     ##      config.pop('ADOCA34_MAX')

     ##      if up : extrainputs = self.pi0[which] + [self.uppions]
     ##      else  : extrainputs = self.pi0[which]

     ##      sels = [ ]

     ##      name = 'D+2Pi0HHH'
     ##      sels += [ subPIDSels(decaysp,name+tag,which,min,max,
     ##                           [self._makeD2FourBody(name+'_'+which,
     ##                                                 ['D+ -> pi+ pi+ pi- pi0'],
     ##                                                 awmFunctor(decaysp,min,max),
     ##                                                 up,config,extrainputs)]) ]

     ##      name = 'D-2Pi0HHH'
     ##      sels += [ subPIDSels(decaysm,name+tag,which,min,max,
     ##                           [self._makeD2FourBody(name+'_'+which,
     ##                                                 ['D- -> pi- pi- pi+ pi0'],
     ##                                                 awmFunctor(decaysm,min,max),
     ##                                                 up,config,extrainputs)]) ]

     ##      return [MergedSelection('D2Pi0HHHBeauty2Charm_%s%s'%(which,tag),
     ##                              RequiredSelections=sels)]

     ## ## Implementation using one instance of SubPID per substitution
     ## def _makeD2Pi0hhh(self,which,up=False):
     ##      '''Makes D->Pi0hhh'''

     ##      tag = ''
     ##      if up: tag = 'UP'

     ##      min,max = self._massWindow('D+')

     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA14_MAX')
     ##      config.pop('ADOCA24_MAX')
     ##      config.pop('ADOCA34_MAX')

     ##      if up : extrainputs = self.pi0[which] + [self.uppions]
     ##      else  : extrainputs = self.pi0[which]

     ##      decaysp = [['pi+','pi+','pi-','pi0'],['K+', 'pi+','pi-','pi0'],
     ##                 ['pi+','K+', 'pi-','pi0'],['K+', 'K+', 'pi-','pi0'],
     ##                 ['K+', 'pi+','K-', 'pi0'],['pi+','pi+','K-', 'pi0'],
     ##                 ['K+', 'K+', 'K-', 'pi0'],['pi+','K+', 'K-', 'pi0']]
     ##      decaysm = getCCs(decaysp)

     ##      ps = self._makeD2FourBody( 'D+2HHHPi0_'+which, ['D+ -> pi+ pi+ pi- pi0'],
     ##                                 awmFunctor(decaysp,min,max),
     ##                                 up,config,extrainputs )

     ##      ms = self._makeD2FourBody( 'D-2HHHPi0_'+which, ['D- -> pi- pi- pi+ pi0'],
     ##                                 awmFunctor(decaysm,min,max),
     ##                                 up,config,extrainputs )

     ##      sels = []

     ##      for dec in decaysp :
     ##           name = makeSelName('D+',dec,tag)
     ##           sels += [ subPIDSels([dec],name,which,min,max,[ps]) ]

     ##      for dec in decaysm :
     ##           name = makeSelName('D-',dec,tag)
     ##           sels += [ subPIDSels([dec],name,which,min,max,[ms]) ]

     ##      return [MergedSelection('D2Pi0HHHBeauty2Charm_%s%s'%(which,tag),RequiredSelections=sels)]

     ## ## Implementation with one SubPID instance per substitution
     ## def _makeD2Pi0hh(self,which,up=False):
     ##      '''Makes D->Pi0hh'''

     ##      min,max = self._massWindow('D0wide')

     ##      tag = ''
     ##      if up: tag = 'UP'

     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA13_MAX')
     ##      config.pop('ADOCA23_MAX')

     ##      if up : extrainputs = self.pi0[which] + [self.uppions]
     ##      else  : extrainputs = self.pi0[which]

     ##      decays = [['pi+','pi-','pi0'],['pi+','K-','pi0'],
     ##                ['K+','pi-','pi0'],['K+','K-','pi0']]

     ##      s = self._makeD2ThreeBody( 'D2HHPi0_'+which, ['D0 -> pi+ pi- pi0'],
     ##                                 awmFunctor(decays,min,max),
     ##                                 up,config,extrainputs )

     ##      sels = [ ]

     ##      for dec in decays :
     ##           name = makeSelName('D',dec,tag)
     ##           sels += [ subPIDSels([dec],name,which,min,max,[s]) ]

     ##      return [MergedSelection('D2Pi0HHBeauty2Charm_%s%s'%(which,tag),RequiredSelections=sels)]

     ## # Implementation using one SubPID instance for all substitutions
     ## def _makeD2Pi0hh(self,which,up=False):
     ##      '''Makes D->Pi0hh'''
     ##      min,max = self._massWindow('D0wide')
     ##      decays = [['pi+','pi-','pi0'],['pi+','K-','pi0'],
     ##                ['K+','pi-','pi0'],['K+','K-','pi0']]
     ##      wm = awmFunctor(decays,min,max)
     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA13_MAX')
     ##      config.pop('ADOCA23_MAX')
     ##      if up : extrainputs = self.pi0[which]+ [self.uppions]
     ##      else : extrainputs = self.pi0[which]
     ##      protoD2pi0hh = self._makeD2ThreeBody('D2Pi0HH_'+which,['D0 -> pi+ pi- pi0'],
     ##                                           wm,up,config,extrainputs)
     ##      name = 'D2Pi0HH'
     ##      if up: name += 'UP'
     ##      return [subPIDSels(decays,name,which,min,max,[protoD2pi0hh])]

     ## Implementation not using SubPID at all
     def _makeD2Pi0hh(self,which,up=False):
          '''Makes D->Pi0hh'''

          min,max = self._massWindow('D0wide')

          tag = ''
          if up: tag = 'UP'

          config = deepcopy(self.config)
          config.pop('ADOCA13_MAX')
          config.pop('ADOCA23_MAX')

          if up : extrainputs = self.pi0[which] + [self.kaons] + [self.uppions] + [self.upkaons]
          else  : extrainputs = self.pi0[which] + [self.kaons]

          decays = [['pi+','pi-','pi0'],
                    ['pi+','K-', 'pi0'],
                    ['K+', 'pi-','pi0'],
                    ['K+', 'K-', 'pi0']]

          sels = [ ]

          for dec in decays :
               name = makeSelName('D',dec,which)
               sel  = makeSel('D0',dec)
               sels += [ self._makeD2ThreeBody(name,[sel],awmFunctor([dec],min,max),
                                               up,config,extrainputs) ]

          return [MergedSelection('D2Pi0HHBeauty2Charm_%s%s'%(which,tag),RequiredSelections=sels)]

     def _makeD2Pi0hhWS(self,which,up=False):
          '''Makes D->Pi0hh WS'''
          tag = ''
          if up: tag = 'UP'
          min,max = self._massWindow('D0')
          decays = [['pi+','pi+','pi0'],['pi+','K+','pi0'],
                    ['K+','pi+','pi0'],['K+','K+','pi0']]
          wm = awmFunctor(decays,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA13_MAX')
          config.pop('ADOCA23_MAX')
          if up : extrainputs = self.pi0[which] + [self.uppions]
          else : extrainputs = self.pi0[which]
          psel = self._makeD2ThreeBody('D2Pi0HHWSPlus'+which,['D0 -> pi+ pi+ pi0'],wm,
                               up,config,extrainputs)
          psel = subPIDSels(decays,'D2Pi0HHWSPlus'+tag,which,min,max,[psel])
          msel = self._makeD2ThreeBody('D2Pi0HHWSMinus'+which,['D0 -> pi- pi- pi0'],wm,
                               up,config,extrainputs)
          msel = subPIDSels(getCCs(decays),'D2Pi0HHWSMinus'+tag,which,min,max,[msel])
          return [MergedSelection('D2Pi0HHWSBeauty2Charm'+which+tag,
                                  RequiredSelections=[psel,msel])]

     def _makeD2KSPi0h(self,whichKs,whichPi0,up=False):
          '''Makes D->KsPi0h'''
          tag = ''
          if up: tag = 'UP'
          which = whichKs+whichPi0
          min,max = self._massWindow('D+')
          decays = [['pi+','KS0','pi0'],
                    ['K+','KS0','pi0']]
          wm = awmFunctor(decays,min,max)
          config = deepcopy(self.config)
          config.pop('ADOCA13_MAX')
          config.pop('ADOCA23_MAX')
          if up : extrainputs = self.pi0[whichPi0]+self.ks[whichKs] + [self.uppions]
          else  : extrainputs = self.pi0[whichPi0]+self.ks[whichKs]
          protoDp2Kspi0h = self._makeD2ThreeBody('D+2KSPi0H_'+whichKs+'_'+whichPi0,['D+ -> pi+ KS0 pi0'],
                                                 wm,up,config,extrainputs)
          psel = subPIDSels(decays,'D+2KSPi0H'+tag,which,min,max,[protoDp2Kspi0h])
          protoDm2Kspi0h = self._makeD2ThreeBody('D-2KSPi0H_'+whichKs+'_'+whichPi0,['D- -> pi- KS0 pi0'],
                                                 wm,up,config,extrainputs)
          msel = subPIDSels(getCCs(decays),'D-2KSPi0H'+tag,which,min,max,[protoDm2Kspi0h])
          return [MergedSelection('D2KSPi0HBeauty2Charm_'+which+tag,RequiredSelections=[psel,msel])]

     ## # Implementation using one instance of SubPID for all substitutions
     ## def _makeD2KSPi0hh(self,whichKs,whichPi0,up=False):
     ##      '''Makes D->KsPi0hh'''
     ##      min,max = self._massWindow('D0wide')
     ##      decays = [['pi+','pi-','KS0','pi0'],['pi+','K-','KS0','pi0'],
     ##                ['K+','pi-','KS0','pi0'],['K+','K-','KS0','pi0']]
     ##      wm = awmFunctor(decays,min,max)
     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA13_MAX')
     ##      config.pop('ADOCA23_MAX')
     ##      config.pop('ADOCA14_MAX')
     ##      config.pop('ADOCA24_MAX')
     ##      config.pop('ADOCA34_MAX')
     ##      if up : extrainputs = self.pi0[whichPi0]+self.ks[whichKs] + [self.uppions]
     ##      else : extrainputs = self.pi0[whichPi0]+self.ks[whichKs]
     ##      protoD2Kspi0hh = self._makeD2FourBody('D2KSPi0HH_'+whichKs+'_'+whichPi0,['D0 -> pi+ pi- KS0 pi0'],
     ##                                            wm,up,config,extrainputs)
     ##      name = 'D2KsPi0HH'
     ##      if up: name += 'UP'
     ##      return [subPIDSels(decays,name,whichKs+'_'+whichPi0,min,max,[protoD2Kspi0hh])]

     ## ## Implementation using one instance of SubPID per substitution
     ## def _makeD2KSPi0hh(self,whichKs,whichPi0,up=False):
     ##      '''Makes D->KsPi0hh'''

     ##      min,max = self._massWindow('D0wide')

     ##      tag = ''
     ##      if up: tag = 'UP'

     ##      which = whichKs+'_'+whichPi0

     ##      config = deepcopy(self.config)
     ##      config.pop('ADOCA13_MAX')
     ##      config.pop('ADOCA23_MAX')
     ##      config.pop('ADOCA14_MAX')
     ##      config.pop('ADOCA24_MAX')
     ##      config.pop('ADOCA34_MAX')

     ##      if up : extrainputs = self.pi0[whichPi0] + self.ks[whichKs] + [self.uppions]
     ##      else  : extrainputs = self.pi0[whichPi0] + self.ks[whichKs]

     ##      decays = [['pi+','pi-','KS0','pi0'],['pi+','K-','KS0','pi0'],
     ##                ['K+', 'pi-','KS0','pi0'],['K+', 'K-','KS0','pi0']]

     ##      s = self._makeD2FourBody('D2HHKsPi0_'+which,['D0 -> pi+ pi- KS0 pi0'],
     ##                               awmFunctor(decays,min,max),up,config,extrainputs)

     ##      sels = [ ]

     ##      for dec in decays :
     ##           name = makeSelName('D',dec,tag)
     ##           sels += [ subPIDSels([dec],name,which,min,max,[s]) ]

     ##      return [ MergedSelection( 'D2KSPi0HHBeauty2Charm'+whichPi0+whichKs+tag,
     ##                                RequiredSelections=sels ) ]

     ## Implementation not using SubPID at all
     def _makeD2KSPi0hh(self,whichKs,whichPi0,up=False):
          '''Makes D->KsPi0hh'''

          min,max = self._massWindow('D0wide')

          tag = ''
          if up: tag = 'UP'

          which = whichKs+'_'+whichPi0

          config = deepcopy(self.config)
          config.pop('ADOCA13_MAX')
          config.pop('ADOCA23_MAX')
          config.pop('ADOCA14_MAX')
          config.pop('ADOCA24_MAX')
          config.pop('ADOCA34_MAX')

          if up : extrainputs = [self.kaons] + self.pi0[whichPi0] + self.ks[whichKs] + [self.uppions] + [self.upkaons]
          else  : extrainputs = [self.kaons] + self.pi0[whichPi0] + self.ks[whichKs]

          decays = [['pi+','pi-','KS0','pi0'],['pi+','K-','KS0','pi0'],
                    ['K+', 'pi-','KS0','pi0'],['K+', 'K-','KS0','pi0']]

          sels = [ ]

          for dec in decays :
               name = makeSelName('D',dec,which)
               sel  = makeSel('D0',dec)
               sels += [ self._makeD2FourBody(name,[sel],awmFunctor([dec],min,max),
                                              up,config,extrainputs) ]

          return [ MergedSelection( 'D2HHKSPi0'+whichPi0+whichKs+tag, RequiredSelections=sels ) ]

     ## # Implementation using one instance of SubPID for all substitutions
     ## def _makeD2hhhh(self,up=False):
     ##      '''Makes D->hhhh'''
     ##      min,max = self._massWindow('D0')
     ##      decays = [
     ##           # 4pi
     ##           ['pi+','pi+','pi-','pi-'],
     ##           # K3pi
     ##           ['pi+','pi+','K-','pi-'],['pi+','pi+','pi-','K-'],
     ##           ['K+','pi+','pi-','pi-'],['pi+','K+','pi-','pi-'],
     ##           # 2K2pi
     ##           ['K+','K+','pi-','pi-'],['K+','pi+','K-','pi-'],
     ##           ['K+','pi+','pi-','K-'],['pi+','K+','K-','pi-'],
     ##           ['pi+','K+','pi-','K-'],['pi+','pi+','K-','K-'],
     ##           # 3Kpi
     ##           ['K+','K+','K-','pi-'],['K+','K+','pi-','K-'],
     ##           ['K+','pi+','K-','K-'],['pi+','K+','K-','K-'],
     ##           # 4K
     ##           ['K+','K+','K-','K-']
     ##           ]
     ##      wm = awmFunctor(decays,min,max)
     ##      protoD2hhhh = self._makeD2FourBody('D2HHHH',['D0 -> pi+ pi+ pi- pi-'],wm,
     ##                                  up,self.config,[self.uppions] if up else [])
     ##      name = 'D2HHHH'
     ##      if up: name += 'UP'
     ##      return [subPIDSels(decays,name,'',min,max,[protoD2hhhh])]

     ## # Implementation using one instance of SubPID per substitution
     ## def _makeD2hhhh(self,up=False):
     ##      '''Makes D->hhhh'''

     ##      tag = ''
     ##      if up: tag = 'UP'

     ##      min,max = self._massWindow('D0')

     ##      decays = [
     ##           # 4pi
     ##           ['pi+','pi+','pi-','pi-'],
     ##           # K3pi
     ##           ['pi+','pi+','K-','pi-'],['pi+','pi+','pi-','K-'],
     ##           ['K+','pi+','pi-','pi-'],['pi+','K+','pi-','pi-'],
     ##           # 2K2pi
     ##           ['K+','K+','pi-','pi-'],['K+','pi+','K-','pi-'],
     ##           ['K+','pi+','pi-','K-'],['pi+','K+','K-','pi-'],
     ##           ['pi+','K+','pi-','K-'],['pi+','pi+','K-','K-'],
     ##           # 3Kpi
     ##           ['K+','K+','K-','pi-'],['K+','K+','pi-','K-'],
     ##           ['K+','pi+','K-','K-'],['pi+','K+','K-','K-'],
     ##           # 4K
     ##           ['K+','K+','K-','K-']
     ##           ]

     ##      extrainputs = [self.uppions] if up else []

     ##      s = self._makeD2FourBody('D2HHHH',['D0 -> pi+ pi+ pi- pi-'],
     ##                               awmFunctor(decays,min,max),
     ##                               up,self.config,extrainputs)

     ##      sels = []

     ##      for dec in decays :
     ##           name = makeSelName('D',dec)
     ##           sels += [ subPIDSels([dec],name,tag,min,max,[s]) ]

     ##      return [ MergedSelection( 'D2HHHH'+tag+'Beauty2Charm', RequiredSelections = sels)]

     # Implementation not using SubPID at all
     def _makeD2hhhh(self,up=False):
          '''Makes D->hhhh'''

          tag = ''
          if up: tag = 'UP'

          min,max = self._massWindow('D0')

          decays = [
               # 4pi
               ['pi+','pi+','pi-','pi-'],
               # K3pi
               ['pi+','pi+','K-','pi-'],
               ['K+','pi+','pi-','pi-'],
               # 2K2pi
               ['K+','K+','pi-','pi-'],
               ['pi+','K+','K-','pi-'],
               ['pi+','pi+','K-','K-'],
               # 3Kpi
               ['K+','K+','K-','pi-'],
               ['K+','pi+','K-','K-'],
               # 4K
               ['K+','K+','K-','K-']
               ]

          extrainputs = [self.uppions] if up else []

          if up : extrainputs = [self.kaons] + [self.uppions] + [self.upkaons]
          else  : extrainputs = [self.kaons]

          sels = []

          for dec in decays :
               name = makeSelName('D',dec)
               sel  = makeSel('D0',dec)
               sels += [ self._makeD2FourBody(name,[sel],awmFunctor([dec],min,max),
                                              up,self.config,extrainputs) ]

          return [ MergedSelection( 'D2HHHH'+tag+'Beauty2Charm', RequiredSelections = sels ) ]

     # Implementation using one instance of SubPID per substitution
     def _makeD2hhhhWS(self,up=False):
          '''Makes D->hhhh WS'''

          tag = ''
          if up: tag = 'UP'

          min,max = self._massWindow('D0')

          decaysp = [['pi+','pi+','pi+','pi+'],
                     ['pi+','pi+','K+', 'pi+'],['pi+','pi+','pi+','K+'],
                     ['K+', 'pi+','pi+','pi+'],['pi+','K+', 'pi+','pi+'],
                     ['K+', 'pi+','K+', 'pi+'],['K+', 'pi+','pi+','K+'],
                     ['pi+','K+', 'K+', 'pi+'],['pi+','K+', 'pi+','K+'],
                     ['K+', 'K+', 'pi+','pi+'],['pi+','pi+','K+', 'K+'],
                     ['pi+','K+', 'K+', 'K+'], ['K+', 'pi+','K+', 'K+'],
                     ['K+', 'K+', 'pi+','K+'], ['K+', 'K+', 'K+', 'pi+'],
                     ['K+', 'K+', 'K+', 'K+']
                     ]
          decaysm = getCCs(decaysp)

          extrainputs = [self.uppions] if up else []

          sp = self._makeD2FourBody('D2HHHHWSPlus',['D0 -> pi+ pi+ pi+ pi+'],
                                    awmFunctor(decaysp,min,max),
                                    up,self.config,extrainputs)
          sm = self._makeD2FourBody('D2HHHHWSMinus',['D0 -> pi- pi- pi- pi-'],
                                    awmFunctor(decaysm,min,max),
                                    up,self.config,extrainputs)

          sels = []

          for dec in decaysp :
               name = makeSelName('D',dec,'WSPlus')
               sels += [ subPIDSels([dec],name,tag,min,max,[sp]) ]
          for dec in decaysm :
               name = makeSelName('D',dec,'WSMinus')
               sels += [ subPIDSels([dec],name,tag,min,max,[sm]) ]

          return [MergedSelection('D2HHHH'+tag+'WSBeauty2Charm',RequiredSelections=sels)]


     def _makeD2PhiMuNu(self,up=False):
          '''makes Ds->phi mu nu'''
          decays  = [['phi(1020)','mu+']]
          min = '1125.13*MeV'
          max = '2168.49*MeV'
          cpkk = CombineParticles(    CombinationCut="ADAMASS('phi(1020)') < 150*MeV", #( hasTopoChild() )
                                      MotherCut='(VFASPF(VCHI2/VDOF) < 10)',
                                      DaughtersCuts={'K+' : '(PIDK>-10)' },
                                      DecayDescriptors=['phi(1020) -> K+ K-'] )
          phi = Selection('PHI2KK4D2PhiMuNu'+'Beauty2Charm',
                                      Algorithm=cpkk,
                                      RequiredSelections=[self.kaons])
          momCuts = [LoKiCuts(['VCHI2DOF'],self.config).code()]
          momCuts.append( '(MM < 2168.49)' )
          momCuts = LoKiCuts.combine(momCuts)

          cpp = CombineParticles(     #CombinationCut=comboCuts,
                                      MotherCut=momCuts,
                                      DecayDescriptors=['[D+ -> phi(1020) mu+]cc'])
          protoD2PhiMuNu  = Selection('ProtoD2PhiMuNuBeauty2Charm',
                                      Algorithm=cpp,
                                      RequiredSelections=[phi,self.muons])
          return [protoD2PhiMuNu]


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DstarBuilder(object):
     '''Makes D* mesons for Beauty2Charm.'''

     def __init__(self,d,pions,uppions,pi0,photons,config,config_pid):
          self.d              = d
          self.pions          = pions
          self.uppions        = uppions
          self.pi0            = pi0
          self.photons        = photons
          self.config         = config
          self.d0pi           = self._makeDstar2D0pi('',self.d.hh)
          self.d0pi           = self._makeDstar2D0pi('D2HHPID',self.d.hh_pid)
          self.d0pi_k3pi      = self._makeDstar2D0pi('D2K3Pi',self.d.k3pi)
          self.d0pi_hhhh      = self._makeDstar2D0pi('D2HHHH',self.d.hhhh)
          self.d0pi_hhhh_pid  = self._makeDstar2D0pi('D2HHHHPID',self.d.hhhh_pid)
          self.d0pi_k3h       = self._makeDstar2D0pi('D2K3H',self.d.k3h)
          self.d0pi_k3h_pid   = self._makeDstar2D0pi('D2K3HPID',self.d.k3h_pid)
          self.d0pi_hhhh_ws   = self._makeDstar2D0pi('D2HHHHWS',self.d.hhhh_ws)
          self.d0pi_kshh_ll   = self._makeDstar2D0pi('D2KSHHLL',self.d.kshh_ll)
          self.d0pi_kshh_dd   = self._makeDstar2D0pi('D2KSHHDD',self.d.kshh_dd)
          self.d0pi_kshh_ll_pid  = self._makeDstar2D0pi('D2KSHHLLPID',self.d.kshh_ll_pid)
          self.d0pi_kshh_dd_pid  = self._makeDstar2D0pi('D2KSHHDDPID',self.d.kshh_dd_pid)
          self.d0pi_pid       = [filterPID('Dstar2D0PiPID',self.d0pi,config_pid,2)]

          self.d0pi_kspi0hh_ll_merged = self._makeDstar2D0pi('D2KSLLPI0MERGEDHH',self.d.kspi0hh_ll_merged)
          self.d0pi_kspi0hh_dd_merged = self._makeDstar2D0pi('D2KSDDPI0MERGEDHH',self.d.kspi0hh_dd_merged)
          self.d0pi_kspi0hh_ll_resolved = self._makeDstar2D0pi('D2KSLLPI0RESOLVEDHH',self.d.kspi0hh_ll_resolved)
          self.d0pi_kspi0hh_dd_resolved = self._makeDstar2D0pi('D2KSDDPI0RESOLVEDHH',self.d.kspi0hh_dd_resolved)

          self.d0pi_kspi0hh_ll_merged_pid = self._makeDstar2D0pi('D2KSLLPI0MERGEDHHPID',self.d.kspi0hh_ll_merged_pid)
          self.d0pi_kspi0hh_dd_merged_pid = self._makeDstar2D0pi('D2KSDDPI0MERGEDHHPID',self.d.kspi0hh_dd_merged_pid)
          self.d0pi_kspi0hh_ll_resolved_pid = self._makeDstar2D0pi('D2KSLLPI0RESOLVEDHHPID',self.d.kspi0hh_ll_resolved_pid)
          self.d0pi_kspi0hh_dd_resolved_pid = self._makeDstar2D0pi('D2KSDDPI0RESOLVEDHHPID',self.d.kspi0hh_dd_resolved_pid)

          # With Upstream D's
          self.d0pi_kspi0hh_ll_merged_dup = self._makeDstar2D0pi('UPD2KSLLPI0MERGEDHH',self.d.kspi0hh_ll_merged_up)
          self.d0pi_kspi0hh_dd_merged_dup = self._makeDstar2D0pi('UPD2KSDDPI0MERGEDHH',self.d.kspi0hh_dd_merged_up)
          self.d0pi_kspi0hh_ll_resolved_dup = self._makeDstar2D0pi('UPD2KSLLPI0RESOLVEDHH',self.d.kspi0hh_ll_resolved_up)
          self.d0pi_kspi0hh_dd_resolved_dup = self._makeDstar2D0pi('UPD2KSDDPI0RESOLVEDHH',self.d.kspi0hh_dd_resolved_up)
          # With Upstream bachelor h
          self.d0pi_kspi0hh_ll_merged_hup = self._makeDstar2D0pi('D2KSLLPI0MERGEDHHUP',self.d.kspi0hh_ll_merged,True)
          self.d0pi_kspi0hh_dd_merged_hup = self._makeDstar2D0pi('D2KSDDPI0MERGEDHHUP',self.d.kspi0hh_dd_merged,True)
          self.d0pi_kspi0hh_ll_resolved_hup = self._makeDstar2D0pi('D2KSLLPI0RESOLVEDHHUP',self.d.kspi0hh_ll_resolved,True)
          self.d0pi_kspi0hh_dd_resolved_hup = self._makeDstar2D0pi('D2KSDDPI0RESOLVEDHHUP',self.d.kspi0hh_dd_resolved,True)

          self.d0pi0_merged        = self._makeDstar02D0Pi0( '', 'Merged'  , self.d.hh )
          self.d0pi0_resolved      = self._makeDstar02D0Pi0( '', 'Resolved', self.d.hh )
          self.d0pi0_merged_pid    = self._makeDstar02D0Pi0( 'D2HHPID', 'Merged'  , self.d.hh_pid )
          self.d0pi0_resolved_pid  = self._makeDstar02D0Pi0( 'D2HHPID', 'Resolved', self.d.hh_pid )
          self.d0pi0_hhhh_merged   = self._makeDstar02D0Pi0( 'D2HHHH', 'Merged'  , self.d.hhhh )
          self.d0pi0_hhhh_resolved = self._makeDstar02D0Pi0( 'D2HHHH', 'Resolved', self.d.hhhh )
          self.d0pi0_hhhh_merged_pid   = self._makeDstar02D0Pi0( 'D2HHHHPID', 'Merged'  , self.d.hhhh_pid )
          self.d0pi0_hhhh_resolved_pid = self._makeDstar02D0Pi0( 'D2HHHHPID', 'Resolved', self.d.hhhh_pid )

          self.d0pi0_k3h_merged   = self._makeDstar02D0Pi0( 'D2K3H', 'Merged'  , self.d.k3h )
          self.d0pi0_k3h_resolved = self._makeDstar02D0Pi0( 'D2K3H', 'Resolved', self.d.k3h )
          self.d0pi0_k3h_merged_pid   = self._makeDstar02D0Pi0( 'D2K3HPID', 'Merged'  , self.d.k3h_pid )
          self.d0pi0_k3h_resolved_pid = self._makeDstar02D0Pi0( 'D2K3HPID', 'Resolved', self.d.k3h_pid )

          # With Upstream D's
          self.d0pi0_merged_dup        = self._makeDstar02D0Pi0( 'UPD2HH', 'Merged'  , self.d.hh_up )
          self.d0pi0_resolved_dup      = self._makeDstar02D0Pi0( 'UPD2HH', 'Resolved', self.d.hh_up )
          self.d0pi0_hhhh_merged_dup   = self._makeDstar02D0Pi0( 'UPD2HHHH', 'Merged'  , self.d.hhhh_up )
          self.d0pi0_hhhh_resolved_dup = self._makeDstar02D0Pi0( 'UPD2HHHH', 'Resolved', self.d.hhhh_up )

          # CRJ : With upstream D candidates
          self.d0pi_dup           = self._makeDstar2D0pi('UPD2HH',self.d.hh_up)
          self.d0pi_hhhh_dup      = self._makeDstar2D0pi('UPD2HHHH',self.d.hhhh_up)
          self.d0pi_kshh_ll_dup   = self._makeDstar2D0pi('UPD2KSHHLL',self.d.kshh_ll_up)
          self.d0pi_kshh_dd_dup   = self._makeDstar2D0pi('UPD2KSHHDD',self.d.kshh_dd_up)

          # CRJ : With Upstream bachelor pi
          self.d0pi_hup               = self._makeDstar2D0pi('',self.d.hh,True)
          self.d0pi_hhhh_hup          = self._makeDstar2D0pi('D2HHHH',self.d.hhhh,True)
          self.d0pi_kshh_ll_hup       = self._makeDstar2D0pi('D2KSHHLL',self.d.kshh_ll,True)
          self.d0pi_kshh_dd_hup       = self._makeDstar2D0pi('D2KSHHDD',self.d.kshh_dd,True)
          self.d0pi_dup_hup           = self._makeDstar2D0pi('UPD2HH',self.d.hh_up,True)
          self.d0pi_hhhh_dup_hup      = self._makeDstar2D0pi('UPD2HHHH',self.d.hhhh_up,True)
          self.d0pi_kshh_ll_dup_hup   = self._makeDstar2D0pi('UPD2KSHHLL',self.d.kshh_ll_up,True)
          self.d0pi_kshh_dd_dup_hup   = self._makeDstar2D0pi('UPD2KSHHDD',self.d.kshh_dd_up,True)

          # CRJ : Modes for Ds*+ -> Ds+ pi0
          self.dpi0_merged              = self._makeDstar2DPi0( '', 'Merged'  ,   self.d.hhh )
          self.dpi0_resolved            = self._makeDstar2DPi0( '', 'Resolved'  , self.d.hhh )
          self.dpi0_merged_pid          = self._makeDstar2DPi0( 'D2HHHPID', 'Merged'  ,   self.d.hhh_pid )
          self.dpi0_resolved_pid        = self._makeDstar2DPi0( 'D2HHHPID', 'Resolved'  , self.d.hhh_pid )
          self.dpi0_khh_merged          = self._makeDstar2DPi0( 'D2KHH', 'Merged'  ,   self.d.k2h )
          self.dpi0_khh_resolved        = self._makeDstar2DPi0( 'D2KHH', 'Resolved'  , self.d.k2h )
          self.dpi0_khh_merged_pid      = self._makeDstar2DPi0( 'D2KHHPID', 'Merged'  ,   self.d.k2h_pid )
          self.dpi0_khh_resolved_pid    = self._makeDstar2DPi0( 'D2KHHPID', 'Resolved'  , self.d.k2h_pid )
          self.dpi0_merged_ksh_ll       = self._makeDstar2DPi0( 'D2KSHLL', 'Merged', self.d.ksh_ll )
          self.dpi0_merged_ksh_dd       = self._makeDstar2DPi0( 'D2KSHDD', 'Merged', self.d.ksh_dd )
          self.dpi0_merged_kshhh_ll     = self._makeDstar2DPi0( 'D2KSHHHLL', 'Merged', self.d.kshhh_ll )
          self.dpi0_merged_kshhh_dd     = self._makeDstar2DPi0( 'D2KSHHHDD', 'Merged', self.d.kshhh_dd )
          self.dpi0_merged_kshhh_ll_pid = self._makeDstar2DPi0( 'D2KSHHHLLPID', 'Merged', self.d.kshhh_ll_pid )
          self.dpi0_merged_kshhh_dd_pid = self._makeDstar2DPi0( 'D2KSHHHDDPID', 'Merged', self.d.kshhh_dd_pid )
          self.dpi0_merged_kskhh_ll     = self._makeDstar2DPi0( 'D2KSKHHLL', 'Merged', self.d.kskhh_ll )
          self.dpi0_merged_kskhh_dd     = self._makeDstar2DPi0( 'D2KSKHHDD', 'Merged', self.d.kskhh_dd )
          self.dpi0_merged_kskhh_ll_pid = self._makeDstar2DPi0( 'D2KSKHHLLPID', 'Merged', self.d.kskhh_ll_pid )
          self.dpi0_merged_kskhh_dd_pid = self._makeDstar2DPi0( 'D2KSKHHDDPID', 'Merged', self.d.kskhh_dd_pid )
          self.dpi0_resolved_ksh_ll     = self._makeDstar2DPi0( 'D2KSHLL', 'Resolved', self.d.ksh_ll )
          self.dpi0_resolved_ksh_dd     = self._makeDstar2DPi0( 'D2KSHDD', 'Resolved', self.d.ksh_dd )
          self.dpi0_resolved_kshhh_ll   = self._makeDstar2DPi0( 'D2KSHHHLL', 'Resolved', self.d.kshhh_ll )
          self.dpi0_resolved_kshhh_dd   = self._makeDstar2DPi0( 'D2KSHHHDD', 'Resolved', self.d.kshhh_dd )
          self.dpi0_resolved_kshhh_ll_pid = self._makeDstar2DPi0( 'D2KSHHHLLPID', 'Resolved', self.d.kshhh_ll_pid )
          self.dpi0_resolved_kshhh_dd_pid = self._makeDstar2DPi0( 'D2KSHHHDDPID', 'Resolved', self.d.kshhh_dd_pid )
          self.dpi0_resolved_kskhh_ll     = self._makeDstar2DPi0( 'D2KSKHHLL', 'Resolved', self.d.kskhh_ll )
          self.dpi0_resolved_kskhh_dd     = self._makeDstar2DPi0( 'D2KSKHHDD', 'Resolved', self.d.kskhh_dd )
          self.dpi0_resolved_kskhh_ll_pid = self._makeDstar2DPi0( 'D2KSKHHLLPID', 'Resolved', self.d.kskhh_ll_pid )
          self.dpi0_resolved_kskhh_dd_pid = self._makeDstar2DPi0( 'D2KSKHHDDPID', 'Resolved', self.d.kskhh_dd_pid )
          self.dpi0_merged_kspi0h_ll    = self._makeDstar2DPi0( 'D2KSPI0HLL', 'Merged', self.d.kspi0h_ll_resolved+self.d.kspi0h_ll_merged )
          self.dpi0_merged_kspi0h_dd    = self._makeDstar2DPi0( 'D2KSPI0HDD', 'Merged', self.d.kspi0h_dd_resolved+self.d.kspi0h_dd_merged )
          self.dpi0_resolved_kspi0h_ll  = self._makeDstar2DPi0( 'D2KSPI0HLL', 'Resolved', self.d.kspi0h_ll_resolved+self.d.kspi0h_ll_merged )
          self.dpi0_resolved_kspi0h_dd  = self._makeDstar2DPi0( 'D2KSPI0HDD', 'Resolved', self.d.kspi0h_dd_resolved+self.d.kspi0h_dd_merged )

          # With Upstream D
          self.dpi0_merged_dup   = self._makeDstar2DPi0( 'UPD2HHH', 'Merged'  , self.d.hhh_up )
          self.dpi0_resolved_dup = self._makeDstar2DPi0( 'UPD2HHH', 'Resolved', self.d.hhh_up )
          self.dpi0_merged_ksh_ll_dup     = self._makeDstar2DPi0( 'UPD2KSHLL', 'Merged', self.d.ksh_ll_up )
          self.dpi0_merged_ksh_dd_dup     = self._makeDstar2DPi0( 'UPD2KSHDD', 'Merged', self.d.ksh_dd_up )
          self.dpi0_merged_kshhh_ll_dup   = self._makeDstar2DPi0( 'UPD2KSHHHLL', 'Merged', self.d.kshhh_ll_up )
          self.dpi0_merged_kshhh_dd_dup   = self._makeDstar2DPi0( 'UPD2KSHHHDD', 'Merged', self.d.kshhh_dd_up )
          self.dpi0_resolved_ksh_ll_dup   = self._makeDstar2DPi0( 'UPD2KSHLL', 'Resolved', self.d.ksh_ll_up )
          self.dpi0_resolved_ksh_dd_dup   = self._makeDstar2DPi0( 'UPD2KSHDD', 'Resolved', self.d.ksh_dd_up )
          self.dpi0_resolved_kshhh_ll_dup = self._makeDstar2DPi0( 'UPD2KSHHHLL', 'Resolved', self.d.kshhh_ll_up )
          self.dpi0_resolved_kshhh_dd_dup = self._makeDstar2DPi0( 'UPD2KSHHHDD', 'Resolved', self.d.kshhh_dd_up )
          self.dpi0_merged_kspi0h_ll_dup    = self._makeDstar2DPi0( 'UPD2KSPI0HLL', 'Merged', self.d.kspi0h_ll_resolved_up+self.d.kspi0h_ll_merged_up )
          self.dpi0_merged_kspi0h_dd_dup    = self._makeDstar2DPi0( 'UPD2KSPI0HDD', 'Merged', self.d.kspi0h_dd_resolved_up+self.d.kspi0h_dd_merged_up )
          self.dpi0_resolved_kspi0h_ll_dup  = self._makeDstar2DPi0( 'UPD2KSPI0HLL', 'Resolved', self.d.kspi0h_ll_resolved_up+self.d.kspi0h_ll_merged_up )
          self.dpi0_resolved_kspi0h_dd_dup  = self._makeDstar2DPi0( 'UPD2KSPI0HDD', 'Resolved', self.d.kspi0h_dd_resolved_up+self.d.kspi0h_dd_merged_up )

          # CRJ : Modes for Ds*+ -> Ds+ gamma
          self.dgamma_hhh       = self._makeDstar2DGamma( 'D2HHH', self.d.hhh )
          self.dgamma_hhh_pid   = self._makeDstar2DGamma( 'D2HHHPID', self.d.hhh_pid )
          self.dgamma_khh       = self._makeDstar2DGamma( 'D2KHH', self.d.k2h )
          self.dgamma_khh_pid   = self._makeDstar2DGamma( 'D2KHHPID', self.d.k2h_pid )
          self.dgamma_ksh_ll    = self._makeDstar2DGamma( 'D2KsHLL', self.d.ksh_ll )
          self.dgamma_ksh_dd    = self._makeDstar2DGamma( 'D2KsHDD', self.d.ksh_dd )
          self.dgamma_kshhh_ll  = self._makeDstar2DGamma( 'D2KsHHHLL', self.d.kshhh_ll )
          self.dgamma_kshhh_dd  = self._makeDstar2DGamma( 'D2KsHHHDD', self.d.kshhh_dd )
          self.dgamma_kshhh_ll_pid  = self._makeDstar2DGamma( 'D2KsHHHLLPID', self.d.kshhh_ll_pid )
          self.dgamma_kshhh_dd_pid  = self._makeDstar2DGamma( 'D2KsHHHDDPID', self.d.kshhh_dd_pid )
          self.dgamma_kskhh_ll  = self._makeDstar2DGamma( 'D2KsKHHLL', self.d.kskhh_ll )
          self.dgamma_kskhh_dd  = self._makeDstar2DGamma( 'D2KsKHHDD', self.d.kskhh_dd )
          self.dgamma_kskhh_ll_pid  = self._makeDstar2DGamma( 'D2KsKHHLLPID', self.d.kskhh_ll_pid )
          self.dgamma_kskhh_dd_pid  = self._makeDstar2DGamma( 'D2KsKHHDDPID', self.d.kskhh_dd_pid )
          self.dgamma_kspi0h_ll = self._makeDstar2DGamma( 'D2KsPI0HLL', self.d.kspi0h_ll_resolved+self.d.kspi0h_ll_merged )
          self.dgamma_kspi0h_dd = self._makeDstar2DGamma( 'D2KsPI0HDD', self.d.kspi0h_dd_resolved+self.d.kspi0h_dd_merged )
          # With Upstream D
          self.dgamma_hhh_dup       = self._makeDstar2DGamma( 'UPD2HHH', self.d.hhh_up )
          self.dgamma_ksh_ll_dup    = self._makeDstar2DGamma( 'UPD2KsHLL', self.d.ksh_ll_up )
          self.dgamma_ksh_dd_dup    = self._makeDstar2DGamma( 'UPD2KsHDD', self.d.ksh_dd_up )
          self.dgamma_kshhh_ll_dup  = self._makeDstar2DGamma( 'UPD2KsHHHLL', self.d.kshhh_ll_up )
          self.dgamma_kshhh_dd_dup  = self._makeDstar2DGamma( 'UPD2KsHHHDD', self.d.kshhh_dd_up )
          self.dgamma_kspi0h_ll_dup = self._makeDstar2DGamma( 'UPD2KsPI0HLL', self.d.kspi0h_ll_resolved_up+self.d.kspi0h_ll_merged_up )
          self.dgamma_kspi0h_dd_dup = self._makeDstar2DGamma( 'UPD2KsPI0HDD', self.d.kspi0h_dd_resolved_up+self.d.kspi0h_dd_merged_up )

          # Jordi: create the lists of selections for D*0 -> D0 pi0 with merged and resolved LL and DD Ks.
          self.d0pi0_kshh_ll     = self._makeDstar02D0Pi0( 'KsHHLL', 'all', self.d.kshh_ll )
          self.d0pi0_kshh_dd     = self._makeDstar02D0Pi0( 'KsHHDD', 'all', self.d.kshh_dd )
          self.d0pi0_kshh_ll_pid = self._makeDstar02D0Pi0( 'KsHHLLPID', 'all', self.d.kshh_ll_pid )
          self.d0pi0_kshh_dd_pid = self._makeDstar02D0Pi0( 'KsHHDDPID', 'all', self.d.kshh_dd_pid )
          # With Upstream D's
          self.d0pi0_kshh_ll_dup = self._makeDstar02D0Pi0( 'UPKsHHLL', 'all', self.d.kshh_ll_up )
          self.d0pi0_kshh_dd_dup = self._makeDstar02D0Pi0( 'UPKsHHDD', 'all', self.d.kshh_dd_up )

          # Jordi: create the lists of selections for D*0 -> D0 gamma for LL and DD Ks.
          self.d0gamma_kshh_ll = self._makeDstar02D0Gamma( 'KsHHLL', self.d.kshh_ll )
          self.d0gamma_kshh_dd = self._makeDstar02D0Gamma( 'KsHHDD', self.d.kshh_dd )
          self.d0gamma_kshh_ll_pid = self._makeDstar02D0Gamma( 'KsHHLLPID', self.d.kshh_ll_pid )
          self.d0gamma_kshh_dd_pid = self._makeDstar02D0Gamma( 'KsHHDDPID', self.d.kshh_dd_pid )
          # With Upstream D's
          self.d0gamma_kshh_ll_dup = self._makeDstar02D0Gamma( 'UPKsHHLL', self.d.kshh_ll_up )
          self.d0gamma_kshh_dd_dup = self._makeDstar02D0Gamma( 'UPKsHHDD', self.d.kshh_dd_up )

          self.d0gamma_hh      = self._makeDstar02D0Gamma( 'HH', self.d.hh )
          self.d0gamma_k3h     = self._makeDstar02D0Gamma( 'K3H', self.d.k3h )
          self.d0gamma_hh_pid  = self._makeDstar02D0Gamma( 'HHPID', self.d.hh_pid )
          self.d0gamma_k3h_pid = self._makeDstar02D0Gamma( 'K3HPID', self.d.k3h_pid )

          d0pi_2460 = self._makeDstar24602D0pi()
          self.d0pi_2460_pid = [filterPID('Dstar24602D0PiPID',d0pi_2460,
                                          config_pid,2)]
          dpi_2460 = self._makeD0star24602Dpi()
          self.dpi_2460_pid = [filterPID('D0star24602DPiPID',dpi_2460,
                                         config_pid,2)]

     def _makeHc2Dpi(self,name,massCut,decays,inputs):
          comboCuts = [massCut,LoKiCuts(['ADOCA12'],self.config).code()]
          comboCuts = LoKiCuts.combine(comboCuts)
          momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                             self.config).code()
          cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                                DecayDescriptors=decays)
          return [Selection(name+'2D0PiBeauty2Charm',Algorithm=cp,
                            RequiredSelections=inputs)]

     def _makeDstar2D0pi(self,name,d2x,up=False):
          '''Makes D*+ -> D0 pi+ + c.c.'''
          pions = [self.pions]
          if up :
               name += 'UP'
               pions += [self.uppions]
          massCut = "(ADAMASS('D*(2010)+') < %(MASS_WINDOW)s) " % self.config
          decays=["D*(2010)+ -> pi+ D0","D*(2010)- -> pi- D0"]
          return self._makeHc2Dpi('Dstar'+name,massCut,decays,d2x+pions)

     def _makeDstar24602D0pi(self):
          '''Makes D*2(2460)+- -> D0 pi+-'''
          massCut = "(ADAMASS('D*_2(2460)+') < 100*MeV) "
          decays=["D*_2(2460)+ -> pi+ D0","D*_2(2460)- -> pi- D0"]
          return self._makeHc2Dpi('Dstar2460',massCut,decays,
                                  self.d.kpi_pid+[self.pions])

     def _makeD0star24602Dpi(self):
          '''Makes D*2(2460)0 -> D+- pi-+'''
          massCut = "(ADAMASS('D*_2(2460)0') < 100*MeV) "
          decays=["D*_2(2460)0 -> pi+ D-","D*_2(2460)0 -> pi- D+"]
          return self._makeHc2Dpi('D0star2460',massCut,decays,
                                  self.d.hhh_cf_pid+[self.pions])


     def _makeDstar02D0X0(self,name,decays,inputs):
          ''' Makes all X -> HH selections involving neutrals.'''
          comboCuts = "(ADAMASS('D*(2007)0') < %(MASS_WINDOW)s) " % self.config
          momCuts = "ALL"
          cp = CombineParticles( CombinationCut   = comboCuts,
                                 MotherCut        = momCuts  ,
                                 DecayDescriptors = decays    )
          cp = cp.configurable( name + 'Beauty2CharmCombiner' )
          #cp.ParticleCombiners.update( { '' : 'MomentumCombiner' } )
          return Selection( 'Dstar02D0' + name + 'Beauty2Charm',
                            Algorithm          = cp            ,
                            RequiredSelections = inputs         )

     def _makeDstar2DX0(self,name,decays,inputs):
          ''' Makes all X+ -> HH selections involving neutrals.'''
          comboCuts = "(ADAMASS('D*_s+') < %(MASS_WINDOW)s) " % self.config
          momCuts = "ALL"
          cp = CombineParticles( CombinationCut   = comboCuts,
                                 MotherCut        = momCuts  ,
                                 DecayDescriptors = decays    )
          cp = cp.configurable( name + 'Beauty2CharmCombiner' )
          #cp.ParticleCombiners.update( { '' : 'MomentumCombiner' } )
          return Selection( 'Dstar2D' + name + 'Beauty2Charm',
                            Algorithm          = cp            ,
                            RequiredSelections = inputs         )

     # CRJ: make the list of selections of D*+ -> D+ pi0 with given selection of D and pi0 type.
     def _makeDstar2DPi0( self, name, pi0type, d2x ):
          decays = [ "[D*_s+ -> D+ pi0]cc" ]
          return [ self._makeDstar2DX0( name + 'Pi0' + pi0type, decays, d2x + self.pi0[ pi0type ] ) ]

     # Jordi: make the list of selections of D*0 -> D0 pi0 with given selection of D and pi0 type.
     def _makeDstar02D0Pi0( self, name, pi0type, d2x ):
          decays = [ "D*(2007)0 -> D0 pi0" ]
          return [ self._makeDstar02D0X0( name + 'Pi0' + pi0type, decays, d2x + self.pi0[ pi0type ] ) ]

     # Jordi: make the list of selections of D*0 -> D0 gamma with given selection of D.
     def _makeDstar02D0Gamma( self, name, d2x ):
          decays = [ "D*(2007)0 -> D0 gamma" ]
          # return [ self._makeDstar02D0X0( name + 'Gamma', decays, d2x + [ self.photons ] ) ]

          combinationCuts = "(AALL)"
          motherCuts      = "(M-MAXTREE(ABSID=='D0',M)<200*MeV)"

          cp = CombineParticles( CombinationCut   = combinationCuts,
                                 MotherCut        = motherCuts     ,
                                 DecayDescriptors = decays          )

          cp = cp.configurable( name + 'Beauty2CharmCombiner' )
          #cp.ParticleCombiners.update( { '' : 'MomentumCombiner' } )

          return [ Selection( 'Dstar02D0' + name + 'Beauty2Charm'        ,
                              Algorithm          = cp                    ,
                              RequiredSelections = d2x + [ self.photons ] ) ]

     # CRJ: make the list of selections of D*+ -> D+ gamma with given selection of D.
     def _makeDstar2DGamma( self, name, d2x ):
          decays = [ "[D*_s+ -> D+ gamma]cc" ]

          combinationCuts = "(AALL)"
          motherCuts      = "(M-MAXTREE(ABSID=='D*_s+',M)<200*MeV)"

          cp = CombineParticles( CombinationCut   = combinationCuts,
                                 MotherCut        = motherCuts     ,
                                 DecayDescriptors = decays          )

          cp = cp.configurable( name + 'Beauty2CharmCombiner' )

          return [ Selection( 'Dstar2DGamma' + name + 'Beauty2Charm'        ,
                              Algorithm          = cp                    ,
                              RequiredSelections = d2x + [ self.photons ] ) ]

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
