# Stripping Lines for DY->ee
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani and D.Ward
#
# DY2ee3 (10-20GeV): StdTightElectrons     & pT>2GeV & PIDe>1
# DY2ee4 (20-40GeV): StdAllNoPIDsElectrons & pT>5GeV & PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdTightElectrons, StdAllNoPIDsElectrons

confdict_DY2ee = { 'DY2eeLine3Prescale' : 1.0,
                   'DY2eeLine4Prescale' : 1.0,
                   'DY2eeLinePostscale' : 1.0,
                   'DY3MinMass' : 10.,
                   'DY3MaxMass' : 20.,
                   'DY4MinMass' : 20.,
                   'DY4MaxMass' : 40.,
                   'ePID'      :  1.,
                   'PrsCalMin' : 50.,
                   'ECalMin'   :  0.1,
                   'HCalMax'   :  0.05,
                   'pT3'        : 2.,
                   'pT4'        : 5.
                   }

default_name = 'DY2ee'

class DY2eeConf(LineBuilder) :

    __configuration_keys__ = ( 'DY2eeLine3Prescale',
                               'DY2eeLine4Prescale',
                               'DY2eeLinePostscale',
                               'DY3MinMass',
                               'DY3MaxMass',
                               'DY4MinMass',
                               'DY4MaxMass',
                               'ePID',
                               'PrsCalMin',
                               'ECalMin',
                               'HCalMax',
                               'pT3',
                               'pT4'
                               )
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name


        # Define the cuts

        _cut3 = '((PT>%(pT3)s*GeV) & (PIDe>%(ePID)s))'%config
        _cut4 = '((PT>%(pT4)s*GeV) & (PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config

        _DY3MassCut = '(MM>%(DY3MinMass)s*GeV) & (MM<%(DY3MaxMass)s*GeV)'%config
        _DY4MassCut = '(MM>%(DY4MinMass)s*GeV) & (MM<%(DY4MaxMass)s*GeV)'%config


        # DY2ee3

        self.sel_DY2ee3 = makeCombination( self._myname + 'DY2ee3', 
                                           StdTightElectrons,
                                           _cut3,
                                           _DY3MassCut
                                           )
     
        self.line_DY2ee3 = StrippingLine( self._myname + 'Line3',
                                          prescale  = config[ 'DY2eeLine3Prescale' ],
                                          postscale = config[ 'DY2eeLinePostscale' ],
                                          selection = self.sel_DY2ee3
                                          )

        self.registerLine( self.line_DY2ee3 )


        # DY2ee4

        self.sel_DY2ee4 = makeCombination( self._myname + 'DY2ee4', 
                                           StdAllNoPIDsElectrons,
                                           _cut4,
                                           _DY4MassCut
                                           )
     
        self.line_DY2ee4 = StrippingLine( self._myname + 'Line4',
                                          prescale  = config[ 'DY2eeLine4Prescale' ],
                                          postscale = config[ 'DY2eeLinePostscale' ],
                                          selection = self.sel_DY2ee4
                                          )

        self.registerLine( self.line_DY2ee4 )


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
