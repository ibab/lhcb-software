#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Beauty2Charm_ComboEngine import *

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DBuilder(object):
    '''Produces all D mesons for the Beauty2Charm module.'''

    def __init__(self,pions,ks,pi0,config):
        self.pions = pions
        self.ks    = ks
        self.pi0   = pi0
        self.config = config
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

    def _makeD2X(self,name,decays,wm,useMaxIPChi2,config,extrainputs=[]):
        ''' Makes all D -> X selections.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),wm,hasTopoChild()]
        # require at least one daughter to IP chi2 > D min FD chi2
        if useMaxIPChi2:
            comboCuts.append("AHASCHILD(MIPCHI2DV(PRIMARY) > %s)" \
                             % config['BPVVDCHI2_MIN'])
        comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        print 'makeD2X',name,'combo:',comboCuts,'mom:',momCuts
        return Selection('Proto'+name+'Beauty2Charm',Algorithm=cp,
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

    def _awmFunctor(self,decays,min,max):
        wm = []
        for daughters in decays:
            pids = ','.join(["'%s'"%d for d in daughters])
            wm.append("in_range(%s,AWM(%s),%s)" % (min,pids,max))
        return '('+('|'.join(wm))+')'

    def _cc(self,decay):
        cc = []
        for d in decay:
            if d.find('+') >=0: cc.append(d.replace('+','-'))
            elif d.find('-') >=0: cc.append(d.replace('-','+'))
            else: cc.append(d)
        return cc

    def _makeD2hh(self):
        '''Makes D->hh'''
        min,max  = self._massWindow('D0')
        decays = [['pi+','pi-'],['pi+','K-'],['K+','pi-'],['K+','K-']]
        wm = self._awmFunctor(decays,min,max)
        protoD2hh = self._makeD2X('D2HH',['D0 -> pi+ pi-'],wm,True,self.config)
        sels = []
        for d in decays:
            tag = '%s%s'%(d[0],d[1])
            wm = "in_range(%s,WM('%s','%s'),%s)" % (min,d[0],d[1],max)
            sub = SubstitutePID('D2HHSubPIDBeauth2Charm_'+tag,
                                Code="DECTREE('Charm -> X+ X-') & (%s)" % wm)
            sub.MaxChi2PerDoF = -666
            sub.Substitutions = {'Charm -> ^X+ X-' : d[0],
                                 'Charm -> X+ ^X-' : d[1]}
            subsel = Selection('D2HHSubPIDSelBeauty2Charm_'+tag,Algorithm=sub,
                               RequiredSelections=[protoD2hh])
            mm = "in_range(%s,MM,%s)" % (min,max)
            sels.append(filterSelection('D2'+tag,mm,[subsel]))
        return MergedSelection('D2HHBeauty2Charm',RequiredSelections=sels)
                       
    def _makeD2hhh(self):
        '''Makes D->hhh'''
        min,max = self._massWindow('D+')

        decays = [['pi+','pi+','pi-'],['pi+','pi+','K-'],
                  ['K+','pi+','pi-'],['K+','pi+','K-'],
                  ['pi+','K+','pi-'],['pi+','K+','K-'],
                  ['K+','K+','pi-']]
        wm = self._awmFunctor(decays,min,max)
        protoD2hhh = self._makeD2X('D2HHH',['D+ -> pi+ pi+ pi-'],wm,True,
                                   self.config)
        sels = []
        for d in decays:
            dm = self._cc(d)
            tag = '%s%s%s_cc'%(d[0],d[1],d[2])
            wm = "in_range(%s,WM('%s','%s','%s'),%s)"%(min,d[0],d[1],d[2],max)
            sub = SubstitutePID('D2HHHSubPIDBeauth2Charm_'+tag,
                                Code="DECTREE('Charm -> X+ X+ X-') & (%s)"\
                                %wm)
            sub.MaxChi2PerDoF = -666
            sub.Substitutions = {'Charm -> ^pi+ pi+ X-' : d[0],
                                 'Charm -> %s ^pi+ X-'%d[0] : d[1],
                                 'Charm -> X+ X+ ^X-' : d[2]}
                                 #'Charm -> ^X- X- X+' : dm[0],
                                 #'Charm -> X- ^X- X+' : dm[1],
                                 #'Charm -> X- X- ^X+' : dm[2]}
            subsel = Selection('D2HHHSubPIDSelBeauty2Charm_'+tag,Algorithm=sub,
                               RequiredSelections=[protoD2hhh])
            mm = "in_range(%s,MM,%s)" % (min,max)
            sels.append(filterSelection('D2'+tag,mm,[subsel]))
        return MergedSelection('D2HHHBeauty2Charm',RequiredSelections=sels)

    def _makeD2KSh(self,which):
        '''Makes D->Ksh'''
        min,max  = self._massWindow('D+')
        decays = [['KS0','pi+'],['KS0','K+']]
        wm = self._awmFunctor(decays,min,max)
        protoD2Ksh = self._makeD2X('D2KsH_'+which,['[D- -> KS0 pi-]cc'],
                                   wm,False,self.config,self.ks[which])
        sels = []
        for dp in decays:
            tag = '%s%s%s_cc'%(dp[0],which,dp[1])
            wm = "in_range(%s,WM('%s','%s'),%s)" % (min,dp[0],dp[1],max)
            sub = SubstitutePID('D2KsHSubPIDBeauth2Charm_'+tag,
                                Code="DECTREE('Charm -> KS0 X+') & (%s)" \
                                % wm)
            sub.MaxChi2PerDoF = -666
            sub.Substitutions = {'Charm -> KS0 ^X+' : dp[1]}
            subsel = Selection('D2KsHSubPIDSelBeauty2Charm_'+tag,Algorithm=sub,
                               RequiredSelections=[protoD2Ksh])
            mm = "in_range(%s,MM,%s)" % (min,max)
            sels.append(filterSelection('D2'+tag,mm,[subsel]))
            dm = self._cc(dp)
            tag = '%s%s%s'%(dm[0],which,dm[1])
            sub = SubstitutePID('D2KsHSubPIDBeauth2Charm_'+tag,
                                Code="DECTREE('Charm -> KS0 X-') & (%s)" \
                                % wm)
            sub.MaxChi2PerDoF = -666
            sub.Substitutions = {'Charm -> KS0 ^X-' : dm[1]}
            subsel = Selection('D2KsHSubPIDSelBeauty2Charm_'+tag,Algorithm=sub,
                               RequiredSelections=[protoD2Ksh])
            sels.append(filterSelection('D2'+tag,mm,[subsel]))
            
        return MergedSelection('D2Ks%sHBeauty2Charm'%which,
                               RequiredSelections=sels)

    def _makeD2KShh(self,which):
        '''Makes D->Kshh'''
        min,max = self._massWindow('D0')
        decays = [['KS0','pi+','pi-'],['KS0','pi+','K-'],
                  ['KS0','K+','pi-'],['KS0','K+','K-']]
        wm = self._awmFunctor(decays,min,max)
        protoD2Kshh = self._makeD2X('D2KSHH_'+which,['D0 -> KS0 pi+ pi-'],
                                    wm,False,self.config,self.ks[which])
        sels = []
        for d in decays:
            tag = '%s%s%s%s'%(d[0],which,d[1],d[2])
            wm = "in_range(%s,WM('%s','%s','%s'),%s)"%(min,d[0],d[1],d[2],max)
            sub = SubstitutePID('D2KsHHSubPIDBeauth2Charm_'+tag,
                                Code="DECTREE('[Charm -> KS0 X+ X-]CC') & "\
                                "(%s)" % wm)
            sub.MaxChi2PerDoF = -666
            sub.Substitutions = {'Charm -> KS0 ^X+ X-' : d[1],
                                 'Charm -> KS0 X+ ^X-' : d[2]}
            subsel = Selection('D2KsHHSubPIDSelBeauty2Charm_'+tag,
                               Algorithm=sub,RequiredSelections=[protoD2Kshh])
            mm = "in_range(%s,MM,%s)" % (min,max)
            sels.append(filterSelection('D2'+tag,mm,[subsel]))
        return MergedSelection('D2Ks%sHHBeauty2Charm'%which,
                               RequiredSelections=sels)

    def _makeD2Pi0hhh(self,which):
        '''Makes D->Pi0hhh'''        
        min,max = self._massWindow('D+')
        wm = "in_range(%s,AM,%s)" % (min,max)
        protoD2Pi0hhh = self._makeD2X('D2Pi0HHH_'+which,
                                      ['[D+ -> pi0 pi+ pi- pi+]cc'],wm,True,
                                      self.config,self.pi0[which])
        return protoD2Pi0hhh

    def _makeD2Pi0hh(self,which):
        '''Makes D->Pi0hh'''
        min,max = self._massWindow('D0')
        decays = [['pi0','pi+','pi-'],['pi0','pi+','K-'],
                  ['pi0','K+','pi-'],['pi0','K+','K-']]
        wm = self._awmFunctor(decays,min,max)
        protoD2pi0hh = self._makeD2X('D2Pi0HH_'+which,['D0 -> pi0 pi+ pi-'],
                                     wm,False,self.config,self.pi0[which])
        sels = []
        for d in decays:
            tag = '%s%s%s%s'%(d[0],which,d[1],d[2])
            wm = "in_range(%s,WM('%s','%s','%s'),%s)"%(min,d[0],d[1],d[2],max)
            sub = SubstitutePID('D2Pi0HHSubPIDBeauth2Charm_'+tag,
                                Code="DECTREE('[Charm -> pi0 X+ X-]CC') & "\
                                "(%s)" % wm)
            sub.MaxChi2PerDoF = -666
            sub.Substitutions = {'Charm -> pi0 ^X+ X-' : d[1],
                                 'Charm -> pi0 X+ ^X-' : d[2]}
            subsel = Selection('D2Pi0HHSubPIDSelBeauty2Charm_'+tag,
                               Algorithm=sub,RequiredSelections=[protoD2pi0hh])
            mm = "in_range(%s,MM,%s)" % (min,max)
            sels.append(filterSelection('D2'+tag,mm,[subsel]))
        return MergedSelection('D2Pi0%sHHBeauty2Charm'%which,
                               RequiredSelections=sels)

    def _makeD2hhhh(self):
        '''Makes D->hhhh'''
        conf = deepcopy(self.config)
        conf['ASUMPT_MIN'] = self.config['4H_ASUMPT_MIN']
        min,max = self._massWindow('D0')
        decays = [['pi+','pi+','pi-','pi-'],
                  ['pi+','pi+','K-','pi-'],['pi+','pi+','pi-','K-'],
                  ['K+','pi+','pi-','pi-'],['pi+','K+','pi-','pi-'],
                  ['K+','pi+','K-','pi-'],['K+','pi+','pi-','K-'],
                  ['pi+','K+','K-','pi-'],['pi+','K+','pi-','K-']]
        wm = self._awmFunctor(decays,min,max)
        protoD2hhhh = self._makeD2X('D2HHHH',['D0 -> pi+ pi+ pi- pi-'],wm,
                                    True,conf)
        sels = []
        for d in decays:
            tag = '%s%s%s%s'%(d[0],d[1],d[2],d[3])
            wm = "in_range(%s,WM('%s','%s','%s','%s'),%s)" \
                 % (min,d[0],d[1],d[2],d[3],max)
            sub = SubstitutePID('D2HHHHSubPIDBeauth2Charm_'+tag,
                                Code="DECTREE('Charm -> X+ X+ X- X-') & (%s)" \
                                % wm)
            sub.MaxChi2PerDoF = -666
            sub.Substitutions = {'Charm -> ^X+ X+ X- X-' : d[0],
                                 'Charm -> X+ ^X+ X- X-' : d[1],
                                 'Charm -> X+ X+ ^X- X-' : d[2],
                                 'Charm -> X+ X+ X- ^X-' : d[3]}
            subsel = Selection('D2HHHHSubPIDSelBeauty2Charm_'+tag,
                               Algorithm=sub,
                               RequiredSelections=[protoD2hhhh])
            mm = "in_range(%s,MM,%s)" % (min,max)
            sels.append(filterSelection('D2'+tag,mm,[subsel]))
        return MergedSelection('D2HHHHBeauty2Charm',RequiredSelections=sels)
                       
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DstarBuilder(object):
    '''Makes D* mesons for Beauty2Charm.'''

    def __init__(self,d02hh,pions,config):
        self.d02hh = d02hh
        self.pions = pions
        self.config = config
        self.d0pi = self._makeDstar2D0pi()
        
    def _makeDstar2D0pi(self):
        '''Makes D*+ -> D0 pi+ + c.c.'''
        comboCuts = ["(ADAMASS('D*(2010)+') < %(MASS_WINDOW)s) " % self.config,
                     LoKiCuts(['AMAXDOCA'],self.config).code()]
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],
                           self.config).code()
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=["[D*(2010)+ -> pi+ D0]cc",
                                                "[D*(2010)+ -> pi+ D~0]cc"])
        print 'makeDstar','combo:',comboCuts,'mom:',momCuts
        return Selection('Dstar2DPiBeauty2Charm',Algorithm=cp,
                         RequiredSelections=[self.d02hh,self.pions])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
