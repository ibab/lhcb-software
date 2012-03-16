# Stripping Lines for Z->ee and studies of their background
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani and D.Ward
#
# Z02ee signal:    StdAllNoPIDsElectrons, PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05 & pT>10GeV & MM>40GeV
# Z02ee same sign: StdAllNoPIDsElectrons, PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05 & pT>10GeV & MM>40GeV

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllNoPIDsElectrons

confdict_Z02ee = { 'Z02ee_Prescale'  : 1.0,
                   'Z02ee_Postscale' : 1.0,
                   'PrsCalMin'      : 50.,
                   'ECalMin'        :  0.1,
                   'HCalMax'        :  0.05,
                   'pT'             : 10.,
                   'MMmin'          : 40.
                   }

name = 'Z02ee'

class Z02eeConf( LineBuilder ) :

    __configuration_keys__ = ( 'Z02ee_Prescale',
                               'Z02ee_Postscale',
                               'PrsCalMin',
                               'ECalMin',
                               'HCalMax',
                               'pT',
                               'MMmin'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        

        # Define the cuts

        _cut   = '((PT>%(pT)s*GeV) & (PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config
        _MMmin = '(MM>%(MMmin)s*GeV)'%config


        # Z02ee signal

        self.sel_Z02ee = makeCombination( self._myname + 'Z02ee', 
                                          StdAllNoPIDsElectrons,
                                          _cut,
                                          _MMmin
                                          )

        self.line_Z02ee = StrippingLine( self._myname + 'Line',
                                         prescale  = config[ 'Z02ee_Prescale' ],
                                         postscale = config[ 'Z02ee_Postscale' ],
                                         selection = self.sel_Z02ee
                                         )

        self.registerLine( self.line_Z02ee )


        # Z02ee same sign

        self.sel_Z02eeSS = makeSSCombination( self._myname + 'Z02eeSS', 
                                              StdAllNoPIDsElectrons,
                                              _cut,
                                              _MMmin
                                              )

        self.line_Z02eeSS = StrippingLine( self._myname + 'SSLine',
                                           prescale  = config[ 'Z02ee_Prescale' ],
                                           postscale = config[ 'Z02ee_Postscale' ],
                                           selection = self.sel_Z02eeSS
                                           )

        self.registerLine( self.line_Z02eeSS )


def makeCombination( name, _input, _daughters, _mother ) :

    _combination = CombineParticles( DecayDescriptor    = 'Z0 -> e+ e-',
                                     DaughtersCuts      = { 'e+' : _daughters,
                                                            'e-' : _daughters },
                                     MotherCut          = _mother,
                                     WriteP2PVRelations = False
                                     )

    return Selection ( name,
                       Algorithm          = _combination,
                       RequiredSelections = [ _input ]
                       )


def makeSSCombination( name, _input, _daughters, _mother ) :

    _combination = CombineParticles( DecayDescriptor    = '[Z0 -> e- e-]cc',
                                     DaughtersCuts      = { 'e-' : _daughters },
                                     MotherCut          = _mother,
                                     WriteP2PVRelations = False
                                     )

    return Selection ( name,
                       Algorithm          = _combination,
                       RequiredSelections = [ _input ]
                       )
