# Stripping Lines for Z->MuMu and studies of Mu efficiencies
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani
#
# Z02MuMu signal:     StdAllLooseMuons,  pT>15GeV & MM>40GeV


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons, StdAllNoPIDsMuons

confdict_Z02MuMu = { 'Z02MuMu_Prescale'  : 1.0,
                     'Z02MuMu_Postscale' : 1.0,
                     'pT'    : 15.,
                     'MMmin' : 40.
                     }

default_name = 'Z02MuMu'

class Z02MuMuConf( LineBuilder ) :

    __configuration_keys__ = ( 'Z02MuMu_Prescale',
                               'Z02MuMu_Postscale',
                               'pT',
                               'MMmin'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _pT    = '(PT>%(pT)s*GeV)'%config
        _MMmin = '(MM>%(MMmin)s*GeV)'%config


        # Z02MuMu signal

        self.sel_Z02MuMu = makeCombination( self._myname + 'Z02MuMu',
                                            StdAllLooseMuons,
                                            _pT,
                                            _MMmin
                                            )

        self.line_Z02MuMu = StrippingLine( self._myname + 'Line',
                                           prescale  = config[ 'Z02MuMu_Prescale' ],
                                           postscale = config[ 'Z02MuMu_Postscale' ],
                                           checkPV   = False,
                                           selection = self.sel_Z02MuMu
                                           )

        self.registerLine( self.line_Z02MuMu )


       

      

def makeCombination( name, _input, _daughters, _mother ) :

    _combination = CombineParticles( DecayDescriptor    = 'Z0 -> mu+ mu-',
                                     DaughtersCuts      = { 'mu+' : _daughters, 
                                                            'mu-' : _daughters },
                                     MotherCut          = _mother,
                                     WriteP2PVRelations = False
                                     )

    return Selection ( name,
                       Algorithm          = _combination,
                       RequiredSelections = [ _input ]
                       )
