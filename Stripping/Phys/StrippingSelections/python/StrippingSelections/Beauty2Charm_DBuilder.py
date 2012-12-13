#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
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
                
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DBuilder(object):
    '''Produces all D mesons for the Beauty2Charm module.'''

    def __init__(self,pions,kaons,ks,pi0,uppions,muons,config,config_pid):
        self.pions = pions
        self.kaons = kaons
        self.ks    = ks
        self.pi0   = pi0
        self.uppions = uppions
        self.muons = muons
        self.config = config
        self.config_pid = config_pid
        self.hh = self._makeD2hh()
        self.hhh = self._makeD2hhh()
        self.hhhh = self._makeD2hhhh()
        self.ksh_ll  = self._makeD2KSh("LL")
        self.ksh_dd  = self._makeD2KSh("DD")
        self.kshh_ll  = self._makeD2KShh("LL")
        self.kshh_dd  = self._makeD2KShh("DD")
        self.pi0hh_merged = self._makeD2Pi0hh("Merged")
        self.pi0hh_resolved = self._makeD2Pi0hh("Resolved") 
        self.pi0hhh_merged = self._makeD2Pi0hhh("Merged")
        self.pi0hhh_resolved = self._makeD2Pi0hhh("Resolved")
        # UP tracks in D
        self.hh_up = self._makeD2hh(True)
        self.hhh_up = self._makeD2hhh(True)
        self.hhhh_up = self._makeD2hhhh(True)
        self.ksh_ll_up  = self._makeD2KSh("LL",True)
        self.ksh_dd_up  = self._makeD2KSh("DD",True)
        self.kshh_ll_up  = self._makeD2KShh("LL",True)
        self.kshh_dd_up  = self._makeD2KShh("DD",True)
        self.pi0hh_merged_up = self._makeD2Pi0hh("Merged",True)
        self.pi0hh_resolved_up = self._makeD2Pi0hh("Resolved",True)
        self.pi0hhh_merged_up = self._makeD2Pi0hhh("Merged",True)
        self.pi0hhh_resolved_up = self._makeD2Pi0hhh("Resolved",True)
        

        # PID filtered selections
        self.hh_pid = [filterPID('D2HHPID',self.hh,config_pid)]
        self.hhh_pid = [filterPID('D2HHHPID',self.hhh,config_pid)]
        self.hhh_pid_tight = [filterPID('D2HHHPIDTIGHT',self.hhh_pid,
                                        config_pid['TIGHT'])]
        # subset decays
        oneK = "NINTREE(ABSID=='K+') == 1"
        d_cf = "(((ID=='D+') & (NINTREE(ID=='K-')==1) & (%s)) | "\
               "((ID=='D-') & (NINTREE(ID=='K+')==1) & (%s)))" % (oneK,oneK)
        ds_cf = "((NINTREE(ID=='K-')==1) & (NINTREE(ID=='K+')==1))"
        self.kpi_pid = [filterSelection('D2KPIPID',oneK,self.hh_pid)]
        self.k3pi = [filterSelection('D2K3PI',oneK,self.hhhh)]
        self.k3pi_pid = [filterPID('D2K3PIPID',self.k3pi,config_pid)]
        self.d0_cf_pid = [MergedSelection('D0CFPID',
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
        
        
        self.ds_hhh_pid_custom = [MergedSelection('Ds2HHHPIDCUSTOM',
                                                  RequiredSelections=self.ds_pipipi_pid_tightpi+self.ds_kpipi_pid_special+self.ds_hhh_cf_custom)]
        #for use with the FULL DST B --> DD line
        #add D+ --> K+K-pi+ to the d_cf selection
        #remove trailing bracket to add this charge combo in
        d_cf_plus = d_cf_noMassWin[:-1] + " | ((NINTREE(ID=='K-')==1) & (NINTREE(ID=='K+')==1)))"
        d_cf_plus = LoKiCuts.combine([d_cf_plus,"in_range(%s,MM,%s)"%(d_min,d_max)])
        d_kkpi_custom = [filterSelection('Dplus2KKPiCUSTOM',"in_range(%s,MM,%s)"%(d_min,d_max),self.kkpi_custom)]
        self.d_cf_hhh_pid_tightpi = [filterSelection('Dplus2HHHCF',d_cf,self.hhh_pid_tightpi)]
        self.d_hhh_custom = [MergedSelection('Dplus2HHHCUSTOM',RequiredSelections=d_kkpi_custom+self.d_cf_hhh_pid_tightpi)]
        
        self.d_hhh_4_B2DD_custom = [MergedSelection('D2HHH_4_B2DD_CUSTOM_Beauty2Charm',
                                                    RequiredSelections=self.ds_hhh_pid_custom+self.d_hhh_custom)]
        #for use with FULL DST D0D0 line
        self.d0_cf_pid_tight = [filterPID('D0CFPIDTIGHT',
                                          self.d0_cf_pid,config_pid['TIGHT'])]

                
    def _makeD2X(self,name,decays,wm,up,config,extrainputs=[]):
        ''' Makes all D -> X selections.'''
        if up:
            wm += '& (ANUM(ISUP)==1)'
            extrainputs += [self.uppions]
            name += 'UP' 
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),wm,hasTopoChild()]
        comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        return  Selection('Proto'+name+'Beauty2Charm',Algorithm=cp,
                          RequiredSelections=[self.pions]+extrainputs)

    def _massWindow(self,which):
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        if which is 'D0':
            min = 1864.84 - dm # D0 - dm
            max = 1864.84 + dm # D0 + dm
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
        protoD2hh = self._makeD2X('D2HH',['D0 -> pi+ pi-'],wm,up,self.config)
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
        psel = self._makeD2X('D2HHWSPlus',['D0 -> pi+ pi+'],wm,up,self.config)
        psel = subPIDSels(decays,'D2HHWSPlus'+tag,'',min,max,[psel])
        msel = self._makeD2X('D2HHWSMinus',['D0 -> pi- pi-'],wm,up,self.config)
        msel = subPIDSels(getCCs(decays),'D2HHWSMinus'+tag,'',min,max,[msel])
        return [MergedSelection('D2HHWSBeauty2Charm'+tag,
                                RequiredSelections=[psel,msel])]

    def _makeD2hhh(self,up=False):
        '''Makes D->hhh'''
        tag = ''
        if up: tag = 'UP'        
        min,max = self._massWindow('D+')
        decays = [['pi+','pi+','pi-'],['pi+','pi+','K-'],
                  ['K+','pi+','pi-'],['K+','pi+','K-'],
                  ['pi+','K+','pi-'],['pi+','K+','K-'],
                  ['K+','K+','pi-']]
        wm = awmFunctor(decays,min,max)
        protoDp2hhh = self._makeD2X('D+2HHH',['D+ -> pi+ pi+ pi-'],wm,up,
                                    self.config)
        psels = subPIDSels(decays,'D+2HHH'+tag,'',min,max,[protoDp2hhh])
        protoDm2hhh = self._makeD2X('D-2HHH',['D- -> pi- pi- pi+'],wm,up,
                                    self.config)
        msels = subPIDSels(getCCs(decays),'D-2HHH'+tag,'',min,max,[protoDm2hhh])
        return [MergedSelection('D2HHHBeauty2Charm'+tag,RequiredSelections=[psels,msels])]

    def _makeD2hhhUP(self):
        '''Makes D->hhh with 1 UP pion'''
        min,max = self._massWindow('D+')
        decays = [['pi+','pi+','pi-'],['pi+','pi+','K-'],
                  ['K+','pi+','pi-'],['K+','pi+','K-'],
                  ['pi+','K+','pi-'],['pi+','K+','K-'],
                  ['K+','K+','pi-']]
        wm = awmFunctor(decays,min,max)
        wm = wm + '& (ANUM(ISUP)==1)'
        protoDp2hhh = self._makeD2X('D+2HHHUP',['D+ -> pi+ pi+ pi-'],wm,
                                    self.config,[self.uppions])
        psels = subPIDSels(decays,'D+2HHHUP','',min,max,[protoDp2hhh])
        protoDm2hhh = self._makeD2X('D-2HHHUP',['D- -> pi- pi- pi+'],wm,
                                    self.config,[self.uppions])
        msels = subPIDSels(getCCs(decays),'D-2HHHUP','',min,max,[protoDm2hhh])
        return [MergedSelection('D2HHHUPBeauty2Charm',
                                RequiredSelections=[psels,msels])]
                                                                                
    def _makeD2KSh(self,which,up=False):
        '''Makes D->Ksh'''
        tag = ''
        if up: tag = 'UP'        
        min,max  = self._massWindow('D+')
        decays = [['KS0','pi+'],['KS0','K+']]
        wm = awmFunctor(decays,min,max)
        config = deepcopy(self.config)    
        config.pop('AMAXDOCA_MAX')
        protoDp2Ksh = self._makeD2X('D+2KsH_'+which,['D+ -> KS0 pi+'],
                                    wm,up,config,self.ks[which])
        psels = subPIDSels(decays,'D+2KsH'+tag,which,min,max,[protoDp2Ksh])
        protoDm2Ksh = self._makeD2X('D-2KsH_'+which,['D- -> KS0 pi-'],
                                    wm,up,config,self.ks[which])
        msels = subPIDSels(getCCs(decays),'D-2KsH'+tag,which,min,max,[protoDm2Ksh])
        return [MergedSelection('D2KsHBeauty2Charm_%s%s'%(which,tag),
                                RequiredSelections=[psels,msels])]
   
    def _makeD2KShh(self,which,up=False):
        '''Makes D->Kshh'''
        min,max = self._massWindow('D0')
        decays = [['KS0','pi+','pi-'],['KS0','pi+','K-'],
                  ['KS0','K+','pi-'],['KS0','K+','K-']]
        wm = awmFunctor(decays,min,max)
        config = deepcopy(self.config)    
        config.pop('AMAXDOCA_MAX')
        config['ADOCA(2,3)_MAX'] = self.config['AMAXDOCA_MAX']
        protoD2Kshh = self._makeD2X('D2KSHH'+which,['D0 -> KS0 pi+ pi-'],
                                    wm,up,config,self.ks[which])
        name = 'D2KsHH'
        if up: name += 'UP'
        return [subPIDSels(decays,name,which,min,max,[protoD2Kshh])]

    def _makeD2KShhWS(self,which,up=False):
        '''Makes D->Kshh WS'''
        tag = ''
        if up: tag = 'UP'
        min,max = self._massWindow('D0')
        decays = [['KS0','pi+','pi+'],['KS0','pi+','K+'],
                  ['KS0','K+','pi+'],['KS0','K+','K+']]
        wm = awmFunctor(decays,min,max)
        config = deepcopy(self.config)    
        config.pop('AMAXDOCA_MAX')
        config['ADOCA(2,3)_MAX'] = self.config['AMAXDOCA_MAX']
        protoPlus = self._makeD2X('D2KSHHWSPlus'+which,['D0 -> KS0 pi+ pi+'],
                                  wm,up,config,self.ks[which])
        psels = subPIDSels(decays,'D2KSHHWSPlus'+tag,which,min,max,[protoPlus])
        protoMinus = self._makeD2X('D2KSHHWSMinus'+which,['D0 -> KS0 pi- pi-'],
                                   wm,up,config,self.ks[which])
        msels = subPIDSels(getCCs(decays),'D2KSHHWSMinus'+tag,which,min,max,
                           [protoMinus])
        return [MergedSelection('D2KsHHWSBeauty2Charm_%s%s'%(which,tag),
                                RequiredSelections=[psels,msels])]
  
    def _makeD2Pi0hhh(self,which,up=False):
        '''Makes D->Pi0hhh'''
        tag = ''
        if up: tag = 'UP'
        min,max = self._massWindow('D+')
        decays = [['pi0','pi+','pi+','pi-'],['pi0','pi+','pi+','K-'],
                  ['pi0','K+','pi+','pi-'],['pi0','K+','pi+','K-'],
                  ['pi0','pi+','K+','pi-'],['pi0','pi+','K+','K-'],
                  ['pi0','K+','K+','pi-']]
        wm = awmFunctor(decays,min,max)
        config = deepcopy(self.config)    
        config.pop('AMAXDOCA_MAX')
        config['ADOCA(2,3)_MAX'] = self.config['AMAXDOCA_MAX']
        config['ADOCA(2,4)_MAX'] = self.config['AMAXDOCA_MAX']
        config['ADOCA(3,4)_MAX'] = self.config['AMAXDOCA_MAX']
        protoDp2pi0hhh = self._makeD2X('D+2Pi0HHH_'+which,
                                       ['D+ -> pi0 pi+ pi+ pi-'],
                                       wm,up,config,self.pi0[which])
        psels = subPIDSels(decays,'D+2Pi0HHH'+tag,which,min,max,[protoDp2pi0hhh])
        protoDm2pi0hhh = self._makeD2X('D-2Pi0HHH_'+which,
                                       ['D- -> pi0 pi- pi- pi+'],
                                       wm,up,config,self.pi0[which])
        msels = subPIDSels(getCCs(decays),'D-2Pi0HHH'+tag,which,min,max,
                          [protoDm2pi0hhh])
        return [MergedSelection('D2Pi0HHHBeauty2Charm_%s%s'%(which,tag),
                                RequiredSelections=[psels,msels])]
    
    def _makeD2Pi0hh(self,which,up=False):
        '''Makes D->Pi0hh'''
        min,max = self._massWindow('D0')
        decays = [['pi0','pi+','pi-'],['pi0','pi+','K-'],
                  ['pi0','K+','pi-'],['pi0','K+','K-']]
        wm = awmFunctor(decays,min,max)
        config = deepcopy(self.config)    
        config.pop('AMAXDOCA_MAX')
        config['ADOCA(2,3)_MAX'] = self.config['AMAXDOCA_MAX']
        protoD2pi0hh = self._makeD2X('D2Pi0HH_'+which,['D0 -> pi0 pi+ pi-'],
                                     wm,up,config,self.pi0[which])
        name = 'D2Pi0HH'
        if up: name += 'UP'
        return [subPIDSels(decays,name,which,min,max,[protoD2pi0hh])]

    def _makeD2Pi0hhWS(self,which,up=False):
        '''Makes D->Pi0hh WS'''
        tag = ''
        if up: tag = 'UP'
        min,max = self._massWindow('D0')
        decays = [['pi0','pi+','pi+'],['pi0','pi+','K+'],
                  ['pi0','K+','pi+'],['pi0','K+','K+']]
        wm = awmFunctor(decays,min,max)
        config = deepcopy(self.config)    
        config.pop('AMAXDOCA_MAX')
        config['ADOCA(2,3)_MAX'] = self.config['AMAXDOCA_MAX']
        psel = self._makeD2X('D2Pi0HHWSPlus'+which,['D0 -> pi0 pi+ pi+'],wm,
                             up,config,self.pi0[which])
        psel = subPIDSels(decays,'D2Pi0HHWSPlus'+tag,which,min,max,[psel])
        msel = self._makeD2X('D2Pi0HHWSMinus'+which,['D0 -> pi0 pi- pi-'],wm,
                             up,config,self.pi0[which])
        msel = subPIDSels(getCCs(decays),'D2Pi0HHWSMinus'+tag,which,min,max,[msel])
        return [MergedSelection('D2Pi0HHWSBeauty2Charm'+which+tag,
                                RequiredSelections=[psel,msel])]
    
    def _makeD2hhhh(self,up=False):
        '''Makes D->hhhh'''
        min,max = self._massWindow('D0')
        decays = [['pi+','pi+','pi-','pi-'],
                  ['pi+','pi+','K-','pi-'],['pi+','pi+','pi-','K-'],
                  ['K+','pi+','pi-','pi-'],['pi+','K+','pi-','pi-'],
                  ['K+','pi+','K-','pi-'],['K+','pi+','pi-','K-'],
                  ['pi+','K+','K-','pi-'],['pi+','K+','pi-','K-']]
        wm = awmFunctor(decays,min,max)
        protoD2hhhh = self._makeD2X('D2HHHH',['D0 -> pi+ pi+ pi- pi-'],wm,
                                    up,self.config)
        name = 'D2HHHH'
        if up: name += 'UP'
        return [subPIDSels(decays,name,'',min,max,[protoD2hhhh])]

    def _makeD2hhhhWS(self,up=False):
        '''Makes D->hhhh WS'''
        tag = ''
        if up: tag = 'UP'
        min,max = self._massWindow('D0')
        decays = [['pi+','pi+','pi+','pi+'],
                  ['pi+','pi+','K+','pi+'],['pi+','pi+','pi+','K+'],
                  ['K+','pi+','pi+','pi+'],['pi+','K+','pi+','pi+'],
                  ['K+','pi+','K+','pi+'],['K+','pi+','pi+','K+'],
                  ['pi+','K+','K+','pi+'],['pi+','K+','pi+','K+']]
        wm = awmFunctor(decays,min,max)
        psel = self._makeD2X('D2HHHHWSPlus',['D0 -> pi+ pi+ pi+ pi+'],wm,up,
                             self.config)
        psel = subPIDSels(decays,'D2HHHHWSPlus'+tag,'',min,max,[psel])
        msel = self._makeD2X('D2HHHHWSMinus',['D0 -> pi- pi- pi- pi-'],wm,up,
                             self.config)
        msel = subPIDSels(getCCs(decays),'D2HHHHWSMinus'+tag,'',min,max,[msel])
        return [MergedSelection('D2HHHHWSBeauty2Charm'+tag,
                                RequiredSelections=[psel,msel])]

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
        #comboCuts = [LoKiCuts(['ASUMPT'],{'ASUMPT_MIN':'400*MeV'}).code()] # at rest the max is at 1400
        #comboCuts.append(hasTopoChild()) 
        #comboCuts = LoKiCuts.combine(comboCuts)
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

    def __init__(self,d,pions,pi0,config,config_pid):
        self.d = d
        self.pions = pions
        self.pi0 = pi0
        self.config = config
        self.d0pi = self._makeDstar2D0pi('',self.d.hh)
        self.d0pi_k3pi = self._makeDstar2D0pi('D2K3Pi',self.d.k3pi)
        self.d0pi_kshh_ll = self._makeDstar2D0pi('D2KSHHLL',self.d.kshh_ll)
        self.d0pi_kshh_dd = self._makeDstar2D0pi('D2KSHHDD',self.d.kshh_dd)
        self.d0pi_pid = [filterPID('Dstar2D0PiPID',self.d0pi,config_pid,2)]
        self.d0pi0_merged = self._makeDstar02D0Pi0('Merged')
        self.d0pi0_resolved = self._makeDstar02D0Pi0('Resolved')
        d0pi_2460 = self._makeDstar24602D0pi()
        self.d0pi_2460_pid = [filterPID('Dstar24602D0PiPID',d0pi_2460,
                                        config_pid,2)]
        dpi_2460 = self._makeD0star24602Dpi()
        self.dpi_2460_pid = [filterPID('D0star24602DPiPID',dpi_2460,
                                       config_pid,2)]

    def _makeHc2Dpi(self,name,massCut,decays,inputs):
        comboCuts = [massCut,LoKiCuts(['AMAXDOCA'],self.config).code()]
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                           self.config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        return [Selection(name+'2D0PiBeauty2Charm',Algorithm=cp,
                          RequiredSelections=inputs)]
        
    def _makeDstar2D0pi(self,name,d2x):
        '''Makes D*+ -> D0 pi+ + c.c.'''
        massCut = "(ADAMASS('D*(2010)+') < %(MASS_WINDOW)s) " % self.config
        decays=["D*(2010)+ -> pi+ D0","D*(2010)- -> pi- D0"]
        return self._makeHc2Dpi('Dstar'+name,massCut,decays,d2x+[self.pions])

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
        ''' Makes all X+ -> HH selections involving neutrals.'''
        comboCuts = "(ADAMASS('D*(2007)0') < %(MASS_WINDOW)s) " % self.config
        momCuts = "ALL" 
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        cp = cp.configurable(name+'Beauty2CharmCombiner')
        cp.ParticleCombiners.update({'':'MomentumCombiner'})
        return Selection('Dstar02D0'+name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=self.d.hh+inputs)

    def _makeDstar02D0Pi0(self,pi0):
        decays = ["D*(2007)0 -> D0 pi0"]
        return [self._makeDstar02D0X0('Pi0_'+pi0,decays,self.pi0[pi0])]


#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
