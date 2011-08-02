#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
from Beauty2Charm_ComboEngine import *
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
        # subset decays
        oneK = "NINTREE(ABSID=='K+') == 1"
        self.kpipipi = [filterSelection('D2K3Pi',oneK,self.hhhh)]
        self.pi0kpi_merged = [filterSelection('D2Pi0KPi_Merged',oneK,
                                              self.pi0hh_merged)]
        self.pi0kpi_resolved = [filterSelection('D2Pi0KPi_Resolved',oneK,
                                                self.pi0hh_resolved)]

    def _makeD2X(self,name,decays,wm,config,extrainputs=[]):
        ''' Makes all D -> X selections.'''
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

    def _makeD2hh(self):
        '''Makes D->hh'''
        min,max  = self._massWindow('D0')
        decays = [['pi+','pi-'],['pi+','K-'],['K+','pi-'],['K+','K-']]
        wm = awmFunctor(decays,min,max)
        protoD2hh = self._makeD2X('D2HH',['D0 -> pi+ pi-'],wm,self.config)
        return [subPIDSels(decays,'D2HH','',min,max,[protoD2hh])]

    def _makeD2hhh(self):
        '''Makes D->hhh'''
        min,max = self._massWindow('D+')
        decays = [['pi+','pi+','pi-'],['pi+','pi+','K-'],
                  ['K+','pi+','pi-'],['K+','pi+','K-'],
                  ['pi+','K+','pi-'],['pi+','K+','K-'],
                  ['K+','K+','pi-']]
        wm = awmFunctor(decays,min,max)
        protoDp2hhh = self._makeD2X('D+2HHH',['D+ -> pi+ pi+ pi-'],wm,
                                    self.config)
        psels = subPIDSels(decays,'D+2HH','',min,max,[protoDp2hhh])
        protoDm2hhh = self._makeD2X('D-2HHH',['D- -> pi- pi- pi+'],wm,
                                    self.config)
        msels = subPIDSels(getCCs(decays),'D-2HHH','',min,max,[protoDm2hhh])
        return [psels,msels]
        #return MergedSelection('D2HHHBeauty2Charm',
        #                       RequiredSelections=[psels,msels])
        
    def _makeD2KSh(self,which):
        '''Makes D->Ksh'''
        min,max  = self._massWindow('D+')
        decays = [['KS0','pi+'],['KS0','K+']]
        wm = awmFunctor(decays,min,max)
        protoDp2Ksh = self._makeD2X('D+2KsH_'+which,['D+ -> KS0 pi+'],
                                    wm,self.config,self.ks[which])
        psels = subPIDSels(decays,'D+2KsH',which,min,max,[protoDp2Ksh])
        protoDm2Ksh = self._makeD2X('D-2KsH_'+which,['D- -> KS0 pi-'],
                                    wm,self.config,self.ks[which])
        msels = subPIDSels(getCCs(decays),'D-2KsH',which,min,max,[protoDm2Ksh])
        return [psels,msels]
        #return MergedSelection('D2Ks%sHBeauty2Charm'%which,
        #                       RequiredSelections=[psels,msels])
   
    def _makeD2KShh(self,which):
        '''Makes D->Kshh'''
        min,max = self._massWindow('D0')
        decays = [['KS0','pi+','pi-'],['KS0','pi+','K-'],
                  ['KS0','K+','pi-'],['KS0','K+','K-']]
        wm = awmFunctor(decays,min,max)
        protoD2Kshh = self._makeD2X('D2KSHH_'+which,['D0 -> KS0 pi+ pi-'],
                                    wm,self.config,self.ks[which])
        return [subPIDSels(decays,'D2KsHH',which,min,max,[protoD2Kshh])]
  
    def _makeD2Pi0hhh(self,which):
        '''Makes D->Pi0hhh'''        
        min,max = self._massWindow('D+')
        decays = [['pi0','pi+','pi+','pi-'],['pi0','pi+','pi+','K-'],
                  ['pi0','K+','pi+','pi-'],['pi0','K+','pi+','K-'],
                  ['pi0','pi+','K+','pi-'],['pi0','pi+','K+','K-'],
                  ['pi0','K+','K+','pi-']]
        wm = awmFunctor(decays,min,max)
        protoDp2pi0hhh = self._makeD2X('D+2Pi0HHH_'+which,
                                       ['D+ -> pi0 pi+ pi+ pi-'],
                                       wm,self.config,self.pi0[which])
        psels = subPIDSels(decays,'D+2Pi0HHH',which,min,max,[protoDp2pi0hhh])
        protoDm2pi0hhh = self._makeD2X('D-2Pi0HHH_'+which,
                                       ['D- -> pi0 pi- pi- pi+'],
                                       wm,self.config,self.pi0[which])
        msels = subPIDSels(getCCs(decays),'D-2Pi0HHH',which,min,max,
                          [protoDm2pi0hhh])
        return [psels,msels]
        #return MergedSelection('D2Pi0%sHHHBeauty2Charm'%which,
        #                       RequiredSelections=[psels,msels])
    
    def _makeD2Pi0hh(self,which):
        '''Makes D->Pi0hh'''
        min,max = self._massWindow('D0')
        decays = [['pi0','pi+','pi-'],['pi0','pi+','K-'],
                  ['pi0','K+','pi-'],['pi0','K+','K-']]
        wm = awmFunctor(decays,min,max)
        protoD2pi0hh = self._makeD2X('D2Pi0HH_'+which,['D0 -> pi0 pi+ pi-'],
                                     wm,self.config,self.pi0[which])
        return [subPIDSels(decays,'D2Pi0HH',which,min,max,[protoD2pi0hh])]
    
    def _makeD2hhhh(self):
        '''Makes D->hhhh'''
        min,max = self._massWindow('D0')
        decays = [['pi+','pi+','pi-','pi-'],
                  ['pi+','pi+','K-','pi-'],['pi+','pi+','pi-','K-'],
                  ['K+','pi+','pi-','pi-'],['pi+','K+','pi-','pi-'],
                  ['K+','pi+','K-','pi-'],['K+','pi+','pi-','K-'],
                  ['pi+','K+','K-','pi-'],['pi+','K+','pi-','K-']]
        wm = awmFunctor(decays,min,max)
        protoD2hhhh = self._makeD2X('D2HHHH',['D0 -> pi+ pi+ pi- pi-'],wm,
                                    self.config)
        return [subPIDSels(decays,'D2HHHH','',min,max,[protoD2hhhh])]

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
        return [Selection('Dstar2DPiBeauty2Charm',Algorithm=cp,
                         RequiredSelections=self.d02hh+[self.pions])]

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
