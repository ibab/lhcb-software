#Stripping Lines for Study of SameSign Background to DY->MuMu
#Electroweak Group (Convenor: Tara Shears)
#Adaptation of lines (to use line builders) by Will Barter

#4 standard lines cover different mass ranges.

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons

confdict_MuMuSS={
    'MuMuSSLine1Prescale' : .01
    , 'MuMuSSLine2Prescale' : .1
    , 'MuMuSSLine3Prescale' : 1.0
    , 'MuMuSSLine4Prescale' : 1.0
    , 'MuMuSSLine1Postscale' : 1.0
    , 'MuMuSSLine2Postscale' : 1.0
    , 'MuMuSSLine3Postscale' : 1.0
    , 'MuMuSSLine4Postscale' : 1.0 
    , 'MuMuSSLine1MinMass' : 2.5
    , 'MuMuSSLine1jpsiexclow' : 3.0
    , 'MuMuSSLine1jpsiexchigh' : 3.2
    , 'MuMuSSLine1MaxMass' : 5.
    , 'MuMuSSLine2MinMass' : 5.
    , 'MuMuSSLine2MaxMass' : 10.
    , 'MuMuSSLine3MinMass' : 10.
    , 'MuMuSSLine3MaxMass' : 20.
    , 'MuMuSSLine4MinMass' : 20.
    , 'mucut' : 1.                                   
 }

name = "MuMuSS"

class MuMuSSConf(LineBuilder) :

    __configuration_keys__ = ( 'MuMuSSLine1Prescale',
                               'MuMuSSLine2Prescale',
                               'MuMuSSLine3Prescale',
                               'MuMuSSLine4Prescale',
                               'MuMuSSLine1Postscale',
                               'MuMuSSLine2Postscale',
                               'MuMuSSLine3Postscale',
                               'MuMuSSLine4Postscale', 
                               'MuMuSSLine1MinMass',
                               'MuMuSSLine1jpsiexclow',
                               'MuMuSSLine1jpsiexchigh',
                               'MuMuSSLine2MinMass', 
                               'MuMuSSLine3MinMass', 
                               'MuMuSSLine4MinMass',
                               'MuMuSSLine1MaxMass', 
                               'MuMuSSLine2MaxMass', 
                               'MuMuSSLine3MaxMass', 
                               'mucut'                            
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _mucut= '(PT>%(mucut)s*GeV)'%config
        _MuMuSS1MassCut = '((MM>%(MuMuSSLine1MinMass)s*GeV)&(MM<%(MuMuSSLine1jpsiexclow)s*GeV))|((MM>%(MuMuSSLine1jpsiexchigh)s*GeV)&(MM<%(MuMuSSLine1MaxMass)s*GeV))'%config
        _MuMuSS2MassCut = '(MM>%(MuMuSSLine2MinMass)s*GeV)&(MM<%(MuMuSSLine2MaxMass)s*GeV)'%config
        _MuMuSS3MassCut = '(MM>%(MuMuSSLine3MinMass)s*GeV)&(MM<%(MuMuSSLine3MaxMass)s*GeV)'%config
        _MuMuSS4MassCut = '(MM>%(MuMuSSLine4MinMass)s*GeV)'%config
        
        self.selMuMuSS1 = makeMuMuSS(self._myname+'MuMuSS1', 
                                     _MuMuSS1MassCut,
                                     _mucut)
     
        self.MuMuSS_line1 = StrippingLine(self._myname+"Line1",
                                          prescale = config['MuMuSSLine1Prescale'],
                                          postscale = config['MuMuSSLine1Postscale'],
                                          selection = self.selMuMuSS1
                                          )
        self.registerLine(self.MuMuSS_line1)


        self.selMuMuSS2 = makeMuMuSS(self._myname+'MuMuSS2', 
                                     _MuMuSS2MassCut,
                                     _mucut)
     
        self.MuMuSS_line2 = StrippingLine(self._myname+"Line2",
                                          prescale = config['MuMuSSLine2Prescale'],
                                          postscale = config['MuMuSSLine2Postscale'],
                                          selection = self.selMuMuSS2
                                          )
        self.registerLine(self.MuMuSS_line2)


        self.selMuMuSS3 = makeMuMuSS(self._myname+'MuMuSS3', 
                                     _MuMuSS3MassCut,
                                     _mucut)
     
        self.MuMuSS_line3 = StrippingLine(self._myname+"Line3",
                                          prescale = config['MuMuSSLine3Prescale'],
                                          postscale = config['MuMuSSLine3Postscale'],
                                          selection = self.selMuMuSS3
                                          )
        self.registerLine(self.MuMuSS_line3)

        
        self.selMuMuSS4 = makeMuMuSS(self._myname+'MuMuSS4', 
                                     _MuMuSS4MassCut,
                                     _mucut)
     
        self.MuMuSS_line4 = StrippingLine(self._myname+"Line4",
                                          prescale = config['MuMuSSLine4Prescale'],
                                          postscale = config['MuMuSSLine4Postscale'],
                                          selection = self.selMuMuSS4
                                          )
        self.registerLine(self.MuMuSS_line4)
      

def makeMuMuSS(name, _MuMuSSMassCut, _mucut) :
    _MuMuSS = CombineParticles(DecayDescriptor = '[Z0 -> mu- mu-]cc',
                               DaughtersCuts = { 'mu-' : _mucut },
                               MotherCut = _MuMuSSMassCut,
                               WriteP2PVRelations = False
                               )
    _stdallloosemuons = StdAllLooseMuons
    return Selection ( name,
                       Algorithm = _MuMuSS,
                       RequiredSelections = [_stdallloosemuons])
