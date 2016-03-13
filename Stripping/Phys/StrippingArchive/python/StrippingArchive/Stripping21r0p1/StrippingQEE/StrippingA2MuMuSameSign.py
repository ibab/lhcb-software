__all__ = (
  'A2MuMuSameSignConf',
  'default_config',
)


from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLooseMuons, StdAllNoPIDsMuons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
    'NAME'        : 'A2MuMuSameSign',
    'BUILDERTYPE' : 'A2MuMuSameSignConf',
    'WGs'         : [ 'QEE'],
    'STREAMS'     : [ 'EW' ],
    'CONFIG'      : { 
        'A2MuMuSameSign_Prescale'  : 1.0,
        'A2MuMuSameSign_Postscale' : 1.0,
        'pT'               : 2.5 * GeV,
        'MMmin'            : 12. * GeV,
        'MMmax'            : 60. * GeV,
    },
}


class A2MuMuSameSignConf( LineBuilder ) :

    __configuration_keys__ = default_config['CONFIG'].keys()

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        # Define the cuts
        _pT      = '(PT>%(pT)s)'%config
        _MMrange = '(MM>%(MMmin)s) & (MM<%(MMmax)s)'%config


        # A2MuMuSameSign signal
        sel_A2MuMuSameSign = makeCombination( name + 'A2MuMuSameSign',
                                            StdAllLooseMuons,
                                            _pT,
                                            _MMrange
                                            )

        line_A2MuMuSameSign = StrippingLine( name + 'Line',
                                           prescale  = config[ 'A2MuMuSameSign_Prescale' ],
                                           postscale = config[ 'A2MuMuSameSign_Postscale' ],
                                           RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                           checkPV   = False,
                                           selection = sel_A2MuMuSameSign
                                           )

        self.registerLine( line_A2MuMuSameSign )
      

def makeCombination( name, _input, _daughters, _mother ) :

    _combination = CombineParticles( DecayDescriptor    = '[Z0 -> mu+ mu+]cc',
                                     DaughtersCuts      = { 'mu+' : _daughters, 
                                                            'mu-' : _daughters },
                                     MotherCut          = _mother,
                                     WriteP2PVRelations = False
                                     )

    return Selection ( name,
                       Algorithm          = _combination,
                       RequiredSelections = [ _input ]
                       )