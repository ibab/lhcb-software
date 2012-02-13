#Stripping Lines for Z->TauTau
#Electroweak Group (Convenor: Tara Shears)


# Lines needed:
# Z02MuPi. stdloosemuons, stdloosenopidspions


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdAllLooseMuons, StdAllLooseElectrons

confdict_Z02TauTau={
    'Z02TauTauLinePrescale'    : 1.0 
    ,  'Z02TauTauLinePostscale'   : 1.0
    ,  'Z0MinMass' : 14.
    ,  'mucut' : 10.
    ,  'picut' : 5.
    ,  'mutrpchi2' : 0.001
    ,  'pitrpchi2' : 0.001
    }

default_name = "Z02TauTau"

class Z02TauTauConf(LineBuilder) :

    __configuration_keys__ = ('Z0MinMass',
                              'mucut',
                              'picut',
                              'Z02TauTauLinePrescale',
                              'Z02TauTauLinePostscale',
                              'mutrpchi2',
                              'pitrpchi2'
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _mucut= '(PT>%(mucut)s*GeV)&(TRPCHI2>%(mutrpchi2)s)'%config
        _picut= '(PT>%(picut)s*GeV)&(TRPCHI2>%(pitrpchi2)s)'%config
        
        _Z0MinMass = '(MM>%(Z0MinMass)s*GeV)'%config


        self.selZ02TauTau = makeZ02TauTau(self._myname+'Z02TauTau', 
                                          _Z0MinMass,
                                          _mucut,
                                          _picut     
                                          )
        
        self.Z02TauTau_line = StrippingLine(self._myname+"Line",
                                            prescale = config['Z02TauTauLinePrescale'],
                                            postscale = config['Z02TauTauLinePostscale'],
                                            checkPV = True,
                                            selection = self.selZ02TauTau
                                            )
        self.registerLine(self.Z02TauTau_line)
        
        self.selZ02TauTau_ss = makeZ02TauTau_ss(self._myname+'Z02TauTau_ss',
                                                _Z0MinMass,
                                                _mucut,
                                                _picut
                                                )
        self.Z02TauTau_liness = StrippingLine(self._myname+"SameSignLine",
                                              prescale=config['Z02TauTauLinePrescale'],
                                              postscale=config['Z02TauTauLinePostscale'],
                                              checkPV=True,
                                              selection=self.selZ02TauTau_ss
                                              )
        self.registerLine(self.Z02TauTau_liness)
        
def makeZ02TauTau(name, _Z0MinMass, _mucut, _picut) :
    _Z0 = CombineParticles(DecayDescriptor = '[Z0 -> mu+ pi-]cc',
                           DaughtersCuts = { 'mu+' : _mucut , 
                                             'mu-' : _mucut ,
                                             'pi+' : _picut ,
                                             'pi-' : _picut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _stdallloosemuons = StdAllLooseMuons
    _stdallnopidspions = StdAllNoPIDsPions
    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallloosemuons, _stdallnopidspions])

def makeZ02TauTau_ss(name, _Z0MinMass, _mucut, _picut) :
    _Z0 = CombineParticles(DecayDescriptor = '[Z0 -> mu+ pi+]cc',
                           DaughtersCuts = { 'mu+' : _mucut ,
                                             'mu-' : _mucut ,
                                             'pi+' : _picut ,
                                             'pi-' : _picut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _stdallloosemuons = StdAllLooseMuons
    _stdallnopidspions = StdAllNoPIDsPions
    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallloosemuons, _stdallnopidspions])
