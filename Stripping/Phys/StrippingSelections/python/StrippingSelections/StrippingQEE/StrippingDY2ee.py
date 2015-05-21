# Stripping Lines for DY->ee
# Electroweak Group (Conveners: S.Bifani, J.Anderson; Stripping contact: W.Barter)
#
# S.Bifani and D.Ward
#
# DY2ee3 (10-20GeV): StdAllNoPIDsElectrons & pT>3GeV & p>10GeV & PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05
# DY2ee4 (20-40GeV): StdAllNoPIDsElectrons & pT>5GeV & p>10GeV & PRS>50Mev & E_ECal/P>0.1 & E_HCal/P<0.05

__all__ = ( 'DY2eeConf', 'default_config' )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdAllNoPIDsElectrons
from GaudiKernel.SystemOfUnits import GeV


default_config = {
    'NAME'        : 'DY2ee',
    'BUILDERTYPE' : 'DY2eeConf',
    'STREAMS'     : [ 'EW'  ],
    'WGs'         : [ 'QEE' ],
    'CONFIG'      : { 
        'DY2eeLine3Prescale' : 1.0,
        'DY2eeLine4Prescale' : 1.0, 
        'DY2eeLinePostscale' : 1.0,  
        'DY3MinMass'  : 10. * GeV,
        'DY3MaxMass'  : 20. * GeV,
        'DY4MinMass'  : 20. * GeV,
        'DY4MaxMass'  : 40. * GeV,
        'ePID'        :  1.,
        'PrsCalMin'   : 50.,
        'ECalMin'     :  0.1,
        'HCalMax'     :  0.05,
        'pT3'         :  3. * GeV,
        'pT4'         :  5. * GeV,
        'p3'          : 10. * GeV,
        'p4'          : 10. * GeV,
    },
}

class DY2eeConf(LineBuilder) :

    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )


        # Define the cuts

        _cut3 = '(P>%(p3)s) &((PT>%(pT3)s) & (PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config
        _cut4 = '(P>%(p4)s) &((PT>%(pT4)s) & (PPINFO(LHCb.ProtoParticle.CaloPrsE,0)>%(PrsCalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloEcalE,0)>P*%(ECalMin)s) & (PPINFO(LHCb.ProtoParticle.CaloHcalE,99999)<P*%(HCalMax)s))'%config

        _DY3MassCut = '(MM>%(DY3MinMass)s) & (MM<%(DY3MaxMass)s)'%config
        _DY4MassCut = '(MM>%(DY4MinMass)s) & (MM<%(DY4MaxMass)s)'%config


        # DY2ee3

        sel_DY2ee3 = makeCombination( name + 'DY2ee3', 
                                           StdAllNoPIDsElectrons,
                                           _cut3,
                                           _DY3MassCut
                                           )
     
        line_DY2ee3 = StrippingLine( name + 'Line3',
                                          prescale  = config[ 'DY2eeLine3Prescale' ],
                                          postscale = config[ 'DY2eeLinePostscale' ],
                                          RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                          selection = sel_DY2ee3
                                          )

        self.registerLine( line_DY2ee3 )


        # DY2ee4

        sel_DY2ee4 = makeCombination( name + 'DY2ee4', 
                                           StdAllNoPIDsElectrons,
                                           _cut4,
                                           _DY4MassCut
                                           )
     
        line_DY2ee4 = StrippingLine( name + 'Line4',
                                          prescale  = config[ 'DY2eeLine4Prescale' ],
                                          postscale = config[ 'DY2eeLinePostscale' ],
                                          RequiredRawEvents = ["Muon","Calo","Rich","Velo","Tracker"],
                                          selection = sel_DY2ee4
                                          )

        self.registerLine( line_DY2ee4 )


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
