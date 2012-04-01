# Stripping Lines for Z->TauTau
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Farry and P.Ilten
#
# Z02TauTauMuX signal
# Z02TauTauMuX same sign
# Z02TauTauEX  signal
# Z02TauTauEX  same sign

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons, StdAllNoPIDsElectrons, StdAllNoPIDsPions

confdict_Z02TauTau = { 'Z02TauTauLinePrescale'  : 1.0,
                       'Z02TauTauLinePostscale' : 1.0,
                       'PrsCalMin' : 50.,
                       'ECalMin'   :  0.1,
                       'HCalMax'   :  0.05,
                       'trkpchi2'  :  0.001,
                       'mupT'      : 15.,
                       'epT'       : 15.,
                       'pipT'      :  5.,
                       'MMmin'     : 20.
                       }

default_name = 'Z02TauTau'

class Z02TauTauConf( LineBuilder ) :

    __configuration_keys__ = ( 'Z02TauTauLinePrescale',
                               'Z02TauTauLinePostscale',
                               'trkpchi2',
                               'PrsCalMin',
                               'ECalMin',
                               'HCalMax',
                               'mupT',
                               'epT',
                               'pipT',
                               'MMmin',
                               )
   
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
       

        # Define the cuts

        _mucut = '(PT>%(mupT)s*GeV) & (TRPCHI2>%(trkpchi2)s)'%config
        _ecut  = '((PT>%(epT)s*GeV) & (TRPCHI2>%(trkpchi2)s) & (PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config
        _picut = '(PT>%(pipT)s*GeV) & (TRPCHI2>%(trkpchi2)s)'%config
       
        _MMmin = '(MM>%(MMmin)s*GeV)'%config


        # Z02TauTauMuX signal

        self.selZ02TauTauMuX = makeZ02TauTauMuX( self._myname + 'MuX',
                                                 _MMmin,
                                                 _mucut,
                                                 _picut    
                                                 )
       
        self.Z02TauTauMuX_line = StrippingLine( self._myname + '_MuXLine',
                                                prescale  = config[ 'Z02TauTauLinePrescale' ],
                                                postscale = config[ 'Z02TauTauLinePostscale' ],
                                                checkPV   = True,
                                                selection = self.selZ02TauTauMuX
                                                )

        self.registerLine( self.Z02TauTauMuX_line )
       

        # Z02TauTauMuX same sign

        self.selZ02TauTauMuX_ss = makeZ02TauTauMuX_ss( self._myname + 'MuX_ss',
                                                       _MMmin,
                                                       _mucut,
                                                       _picut
                                                       )

        self.Z02TauTauMuX_liness = StrippingLine( self._myname + '_MuXLineSS',
                                                  prescale  = config[ 'Z02TauTauLinePrescale' ],
                                                  postscale = config[ 'Z02TauTauLinePostscale' ],
                                                  checkPV   = True,
                                                  selection = self.selZ02TauTauMuX_ss
                                                  )

        self.registerLine( self.Z02TauTauMuX_liness )


        # Z02TauTauEX signal

        self.selZ02TauTauEX = makeZ02TauTauEX( self._myname + 'EX',
                                               _MMmin,
                                               _ecut,
                                               _picut
                                               )
       
        self.Z02TauTauEX_line = StrippingLine( self._myname + '_EXLine',
                                               prescale  = config[ 'Z02TauTauLinePrescale' ],
                                               postscale = config[ 'Z02TauTauLinePostscale' ],
                                               checkPV   = True,
                                               selection = self.selZ02TauTauEX
                                               )

        self.registerLine( self.Z02TauTauEX_line )


        # Z02TauTauEX same sign

        self.selZ02TauTauEX_ss = makeZ02TauTauEX_ss( self._myname+'EX_ss',
                                                     _MMmin,
                                                     _ecut,
                                                     _picut
                                                     )

        self.Z02TauTauEX_liness = StrippingLine( self._myname + '_EXLineSS',
                                                 prescale  = config[ 'Z02TauTauLinePrescale' ],
                                                 postscale = config[ 'Z02TauTauLinePostscale' ],
                                                 checkPV   = True,
                                                 selection = self.selZ02TauTauEX_ss
                                                 )

        self.registerLine( self.Z02TauTauEX_liness )


def makeZ02TauTauMuX( name, _MMmin, _mucut, _picut ) :

    _Z0 = CombineParticles(DecayDescriptor = '[Z0 -> mu+ pi-]cc',
                           DaughtersCuts = { 'mu+' : _mucut ,
                                             'mu-' : _mucut ,
                                             'pi+' : _picut ,
                                             'pi-' : _picut },
                           MotherCut = _MMmin,
                           WriteP2PVRelations = False
                           )

    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallloosemuons = StdAllLooseMuons
    _stdallnopidspions = StdAllNoPIDsPions

    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallloosemuons, _stdallnopidspions])


def makeZ02TauTauMuX_ss( name, _MMmin, _mucut, _picut ) :

    _Z0 = CombineParticles( DecayDescriptor = '[Z0 -> mu+ pi+]cc',
                            DaughtersCuts = { 'mu+' : _mucut ,
                                              'mu-' : _mucut ,
                                              'pi+' : _picut ,
                                              'pi-' : _picut },
                            MotherCut = _MMmin,
                            WriteP2PVRelations = False
                            )

    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallloosemuons = StdAllLooseMuons
    _stdallnopidspions = StdAllNoPIDsPions

    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallloosemuons, _stdallnopidspions])


def makeZ02TauTauEX( name, _MMmin, _ecut, _picut ) :

    _Z0 = CombineParticles( DecayDescriptor = '[Z0 -> e+ pi-]cc',
                            DaughtersCuts = { 'e+' : _ecut ,
                                              'e-' : _ecut ,
                                              'pi+' : _picut ,
                                              'pi-' : _picut },
                            MotherCut = _MMmin,
                            WriteP2PVRelations = False
                            )

    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallnopidselectrons  = StdAllNoPIDsElectrons
    _stdallnopidspions      = StdAllNoPIDsPions

    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallnopidselectrons, _stdallnopidspions])


def makeZ02TauTauEX_ss( name, _MMmin, _ecut, _picut ) :

    _Z0 = CombineParticles( DecayDescriptor = '[Z0 -> e+ pi+]cc',
                            DaughtersCuts = { 'e+' : _ecut ,
                                              'e-' : _ecut ,
                                              'pi+' : _picut ,
                                              'pi-' : _picut },
                            MotherCut = _MMmin,
                            WriteP2PVRelations = False
                            )

    _Z0Conf = _Z0.configurable("Combine_"+name+"_Z")
    _Z0Conf.ParticleCombiners.update({'':'MomentumCombiner'})
    _stdallnopidselectrons = StdAllNoPIDsElectrons
    _stdallnopidspions     = StdAllNoPIDsPions

    return Selection ( name,
                       Algorithm = _Z0,
                       RequiredSelections = [_stdallnopidselectrons, _stdallnopidspions])
