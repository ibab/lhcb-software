#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from copy import deepcopy
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2Charm_Utils import *
#from DecayFilter import DecayFilter

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class DBuilder(object):
    '''Produces all D mesons for the Beauty2Charm module.'''

    def __init__(self,pions,config):
        self.pions = pions
        self.config = config
        self.hh = self._makeD2hh()
        self.hhh = self._makeD2hhh()
        self.hhhh = self._makeD2hhhh()

    def _makeD2X(self,name,decays,amass,mass,config):
        ''' Makes all D -> X selections.'''
        comboCuts = [LoKiCuts(['ASUMPT'],config).code(),amass,hasTopoChild()]
        # require at least one daughter to IP chi2 > D min FD chi2
        comboCuts.append("AHASCHILD(MIPCHI2DV(PRIMARY) > %s)" \
                         % config['BPVVDCHI2_MIN'])
        comboCuts.append(LoKiCuts(['AMAXDOCA'],config).code())
        comboCuts = LoKiCuts.combine(comboCuts)
        momCuts = [mass,
                   LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()]
        momCuts = LoKiCuts.combine(momCuts)
        cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                              DecayDescriptors=decays)
        print 'makeD2X',name,'combo:',comboCuts,'mom:',momCuts
        return Selection(name+'Beauty2Charm',Algorithm=cp,
                         RequiredSelections=[self.pions])

    def _makeD2hh(self):
        '''Makes D->hh'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        min = 1864.84 - dm # D0 - dm
        max = 1864.84 + dm # D0 + dm
        #massCuts = <ADD WM Functor code from Albert here!>
        amass = 'in_range(%d*%s,AM,%d*%s)' % (min-10,units,max+10,units)
        mass = 'in_range(%d*%s,M,%d*%s)' % (min,units,max,units)
        protoD2hh = self._makeD2X('D2HH',['D0 -> pi+ pi-'],amass,mass,
                                  self.config)
        return protoD2hh
        # Add Albert's ComboEngine stuff here!!!!

    def _makeD2hhh(self):
        '''Makes D->hhh'''
        dm,units = LoKiCuts.cutValue(self.config['MASS_WINDOW'])
        min = 1869.62 - dm # D+ - dm
        max = 1968.49 + dm # Ds+ + dm
        #massCuts = <ADD WM Functor code from Albert here!>
        amass = 'in_range(%d*%s,AM,%d*%s)' % (min-10,units,max+10,units)
        mass = 'in_range(%d*%s,M,%d*%s)' % (min,units,max,units)
        protoD2hhh = self._makeD2X('D2HHH',['[D+ -> pi+ pi+ pi-]cc'],amass,
                                   mass,self.config)
        return protoD2hhh
        # Add Albert's ComboEngine stuff here!!!!

    def _makeD2hhhh(self):
        '''Makes D->hhhh'''
        conf = deepcopy(self.config)
        conf['ASUMPT_MIN'] = self.config['4H_ASUMPT_MIN']
        dm,units = LoKiCuts.cutValue(conf['MASS_WINDOW'])
        min = 1864.84 - dm # D0 - dm
        max = 1864.84 + dm # D0 + dm
        #massCuts = <ADD WM Functor code from Albert here!>
        amass = 'in_range(%d*%s,AM,%d*%s)' % (min-10,units,max+10,units)
        mass = 'in_range(%d*%s,M,%d*%s)' % (min,units,max,units)
        protoD2hhhh = self._makeD2X('D2HHHH',['D0 -> pi+ pi+ pi- pi-'],amass,
                                    mass,conf)
        return protoD2hhhh
        # Add Albert's ComboEngine stuff here!!!!

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
