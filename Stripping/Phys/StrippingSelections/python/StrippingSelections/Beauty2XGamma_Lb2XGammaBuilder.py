from copy import deepcopy
from Gaudi.Configuration import *
from Beauty2Charm_LoKiCuts import LoKiCuts
from Beauty2XGamma_Utils import *


class Lb2XGammaBuilder(object):
    '''Makes all Lambda_b -> X Gamma lines.'''

    def __init__(self, gamma, hh, config):
        self.gamma = gamma if isinstance(gamma, list) else [gamma]
        self.gammaL0TOS = [ MergedSelection("SelGamL0TOS",
                                            RequiredSelections = [
            TOSFilter("SelGamL0PhotonTOS", self.gamma,"L0PhotonDecision"),
            TOSFilter("SelGamL0ElectronTOS", self.gamma,"L0ElectronDecision")]
                                            )
                            ]
        self.hh = hh
        self.config = deepcopy(config)
        self.config['AM_MIN'] = '4500*MeV'
        self.config['AM_MAX'] = '6700*MeV'
        self.lines = []
        # Lambda_b -> Lambda(pH) Gamma
        #self._makeLb2PPiGamma()
        self._makeLb2PKGamma()
        self._makeLb2LGamma()
        # was for testing
        #self._makeLb2LGammaDD()
        #self._makeLb2LGammaLL()
        #self._makeLb2LGammaL0TOSDD()
        #self._makeLb2LGammaL0TOSLL()
        
        #self._makeLb2PHGamma()
        
    def _makeLb2PPiGamma(self):
        '''Makes RS Lb -> p+- Pi-+ gamma + c.c.'''
        decs = ["[Lambda_b0 -> Lambda0 gamma]cc"]
        decays = {'Lb2PPiGamma': decs}
        inputs = {'Lb2PPiGamma': self.gamma+self.hh.pPi_pid}
        rs = makeB2XSels(decays, 'Lb2PPiGamma', inputs, self.config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeLb2LGamma(self):
        '''Makes RS Lb -> L(p+ pi-) gamma + c.c.'''
        decs = ["[Lambda_b0 -> Lambda0 gamma]cc"]
        decays = {'Lb2LGamma': decs}
        inputs = {'Lb2LGamma': self.gammaL0TOS+self.hh.Lambda}
        rs = makeB2XSels(decays, 'Lb2LGamma', inputs, self.config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeLb2LGammaL0TOSLL(self):
        '''Makes RS Lb -> L(p+ pi-) gamma + c.c.'''
        decs = ["[Lambda_b0 -> Lambda0 gamma]cc"]
        decays = {'Lb2LGammaL0TOSLL': decs}
        inputs = {'Lb2LGammaL0TOSLL': self.gammaL0TOS+self.hh.LambdaLL}
        rs = makeB2XSels(decays, 'Lb2LGammaL0TOSLL', inputs, self.config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeLb2LGammaL0TOSDD(self):
        '''Makes RS Lb -> L(p+ pi-) gamma + c.c.'''
        decs = ["[Lambda_b0 -> Lambda0 gamma]cc"]
        decays = {'Lb2LGammaL0TOSDD': decs}
        inputs = {'Lb2LGammaL0TOSDD': self.gammaL0TOS+self.hh.LambdaDD}
        rs = makeB2XSels(decays, 'Lb2LGammaL0TOSDD', inputs, self.config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeLb2LGammaLL(self):
        '''Makes RS Lb -> L(p+ pi-) gamma + c.c.'''
        decs = ["[Lambda_b0 -> Lambda0 gamma]cc"]
        decays = {'Lb2LGammaLL': decs}
        inputs = {'Lb2LGammaLL': self.gamma + self.hh.LambdaLL}
        rs = makeB2XSels(decays, 'Lb2LGammaLL', inputs, self.config)
        self.lines.append(ProtoLine(rs,1.0))

    def _makeLb2LGammaDD(self):
        '''Makes RS Lb -> L(p+ pi-) gamma + c.c.'''
        decs = ["[Lambda_b0 -> Lambda0 gamma]cc"]
        decays = {'Lb2LGammaDD': decs}
        inputs = {'Lb2LGammaDD': self.gamma + self.hh.LambdaDD}
        rs = makeB2XSels(decays, 'Lb2LGammaDD', inputs, self.config)
        self.lines.append(ProtoLine(rs,1.0))


    def _makeLb2PKGamma(self):
        '''Makes RS Lb -> p+- K-+ gamma + c.c.'''
        decs = ["[Lambda_b0 -> Lambda(1520)0 gamma]cc"]
        decays = {'Lb2PKGamma': decs}
        inputs = {'Lb2PKGamma': self.gamma+self.hh.pK_pid}
        rs = makeB2XSels(decays, 'Lb2PKGamma', inputs, self.config)
        self.lines.append(ProtoLine(rs,1.0))
  
    #def _makeLb2PHGamma(self):
    #    '''Makes RS Lb -> D0(HH) p+- H-+ + c.c.'''
    #    decs = ["[Lambda_b0 -> Lambda0 gamma]cc"]
    #    decays = {'Lb2PHGamma': decs}
    #    inputs = {'Lb2PHGamma': self.gamma+self.hh.ph_pid}
    #    rs = makeB2XSels(decays, 'Lb2PHGamma', inputs, self.config)
    #    self.lines.append(ProtoLine(rs,1.0))
    
# EOF
