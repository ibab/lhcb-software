# Stripping Lines for Z->MuMu and studies of Mu efficiencies
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani
#
# Z02MuMu/High Mass DY signal:     StdAllLooseMuons,  pT>3GeV & MM>40GeV

__all__ = (
  'Z02MuMuConf',
  'default_config',
)


from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons, StdAllNoPIDsMuons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
    'NAME'        : 'Z02MuMu',
    'BUILDERTYPE' : 'Z02MuMuConf',
    'WGs'         : [ 'QEE'],
    'STREAMS'     : [ 'EW' ],
    'CONFIG'      : { 
        'Z02MuMu_Prescale'  : 1.0,
        'Z02MuMu_Postscale' : 1.0,
        'pT'    : 3.  * GeV,
        'MMmin' : 40. * GeV,
    },
}


class Z02MuMuConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        # Define the cuts
        _pT    = '(PT>%(pT)s)'%config
        _MMmin = '(MM>%(MMmin)s)'%config


        # Z02MuMu signal
        sel_Z02MuMu = makeCombination( name + 'Z02MuMu',
                                            StdAllLooseMuons,
                                            _pT,
                                            _MMmin
                                            )

        line_Z02MuMu = StrippingLine( name + 'Line',
                                           prescale  = config[ 'Z02MuMu_Prescale' ],
                                           postscale = config[ 'Z02MuMu_Postscale' ],
                                           RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                           checkPV   = False,
                                           selection = sel_Z02MuMu
                                           )

        self.registerLine( line_Z02MuMu )
      

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
