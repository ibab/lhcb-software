#Stripping Lines for Z->TauTau
#Electroweak Group (Convenor: Tara Shears)


# Lines needed:
# Z02MuPi. stdloosemuons, stdloosenopidspions


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsPions, StdAllLooseMuons, StdAllNoPIDsElectrons

confdict_Z02TauTau={
    'Z02TauTauLinePrescale'    : 1.0 
    ,  'Z02TauTauLinePostscale'   : 1.0
    ,  'Z0MinMass' : 20.
    ,  'muptcut' : 15.
    ,  'eptcut'  : 15.
    ,  'piptcut' : 5.
    ,  'trpchi2' : 0.001
    ,  'HCalMax' : 0.05
    ,  'ECalMin' : 0.1
    ,  'PrsCalMin' : 50.
    }

default_name = "Z02TauTau"

class Z02TauTauConf(LineBuilder) :

    __configuration_keys__ = ('Z0MinMass',
                              'muptcut',
                              'piptcut',
                              'eptcut',
                              'Z02TauTauLinePrescale',
                              'Z02TauTauLinePostscale',
                              'trpchi2',
                              'HCalMax',
                              'ECalMin',
                              'PrsCalMin'
                              )
    
    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self._myname = name
        
        #Define the cuts
        _mucut= '(PT>%(muptcut)s*GeV)&(TRPCHI2>%(trpchi2)s)'%config
        _picut= '(PT>%(piptcut)s*GeV)&(TRPCHI2>%(trpchi2)s)'%config
        _ecut= '((PT>%(eptcut)s*GeV)&(TRPCHI2>%(trpchi2)s)&(PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s)&(PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config
        
        _Z0MinMass = '(MM>%(Z0MinMass)s*GeV)'%config


        self.selZ02TauTauMuX = makeZ02TauTauMuX(self._myname+'MuX', 
                                          _Z0MinMass,
                                          _mucut,
                                          _picut     
                                          )
        
        self.Z02TauTauMuX_line = StrippingLine(self._myname+"_MuXLine",
                                            prescale = config['Z02TauTauLinePrescale'],
                                            postscale = config['Z02TauTauLinePostscale'],
                                            checkPV = True,
                                            selection = self.selZ02TauTauMuX
                                            )
        self.registerLine(self.Z02TauTauMuX_line)
        
        self.selZ02TauTauMuX_ss = makeZ02TauTauMuX_ss(self._myname+'MuX_ss',
                                                _Z0MinMass,
                                                _mucut,
                                                _picut
                                                )
        self.Z02TauTauMuX_liness = StrippingLine(self._myname+"_MuXLineSS",
                                              prescale=config['Z02TauTauLinePrescale'],
                                              postscale=config['Z02TauTauLinePostscale'],
                                              checkPV = True,
                                              selection=self.selZ02TauTauMuX_ss
                                              )
        self.registerLine(self.Z02TauTauMuX_liness)


        self.selZ02TauTauEX = makeZ02TauTauEX(self._myname+'EX', 
                                          _Z0MinMass,
                                          _ecut,
                                          _picut     
                                          )
        
        self.Z02TauTauEX_line = StrippingLine(self._myname+"_EXLine",
                                            prescale = config['Z02TauTauLinePrescale'],
                                            postscale = config['Z02TauTauLinePostscale'],
                                            checkPV = True,
                                            selection = self.selZ02TauTauEX
                                            )
        self.registerLine(self.Z02TauTauEX_line)
        
        self.selZ02TauTauEX_ss = makeZ02TauTauEX_ss(self._myname+'EX_ss',
                                                _Z0MinMass,
                                                _ecut,
                                                _picut
                                                )
        self.Z02TauTauEX_liness = StrippingLine(self._myname+"_EXLineSS",
                                              prescale=config['Z02TauTauLinePrescale'],
                                              postscale=config['Z02TauTauLinePostscale'],
                                              checkPV = True,
                                              selection=self.selZ02TauTauEX_ss
                                              )
        self.registerLine(self.Z02TauTauEX_liness)
        
def makeZ02TauTauMuX(name, _Z0MinMass, _mucut, _picut) :
    _Z0 = CombineParticles(DecayDescriptor = '[Z0 -> mu+ pi-]cc',
                           DaughtersCuts = { 'mu+' : _mucut , 
                                             'mu-' : _mucut ,
                                             'pi+' : _picut ,
                                             'pi-' : _picut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallloosemuons = StdAllLooseMuons
    _stdallnopidspions = StdAllNoPIDsPions
    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallloosemuons, _stdallnopidspions])

def makeZ02TauTauMuX_ss(name, _Z0MinMass, _mucut, _picut) :
    _Z0 = CombineParticles(DecayDescriptor = '[Z0 -> mu+ pi+]cc',
                           DaughtersCuts = { 'mu+' : _mucut ,
                                             'mu-' : _mucut ,
                                             'pi+' : _picut ,
                                             'pi-' : _picut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallloosemuons = StdAllLooseMuons
    _stdallnopidspions = StdAllNoPIDsPions
    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallloosemuons, _stdallnopidspions])
        
def makeZ02TauTauEX(name, _Z0MinMass, _ecut, _picut) :
    _Z0 = CombineParticles(DecayDescriptor = '[Z0 -> e+ pi-]cc',
                           DaughtersCuts = { 'e+' : _ecut , 
                                             'e-' : _ecut ,
                                             'pi+' : _picut ,
                                             'pi-' : _picut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallnopidselectrons  = StdAllNoPIDsElectrons
    _stdallnopidspions      = StdAllNoPIDsPions
    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallnopidselectrons, _stdallnopidspions])

def makeZ02TauTauEX_ss(name, _Z0MinMass, _ecut, _picut) :
    _Z0 = CombineParticles(DecayDescriptor = '[Z0 -> e+ pi+]cc',
                           DaughtersCuts = { 'e+' : _ecut ,
                                             'e-' : _ecut ,
                                             'pi+' : _picut ,
                                             'pi-' : _picut },
                           MotherCut = _Z0MinMass,
                           WriteP2PVRelations = False
                           )
    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallnopidselectrons = StdAllNoPIDsElectrons
    _stdallnopidspions     = StdAllNoPIDsPions
    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallnopidselectrons, _stdallnopidspions])

