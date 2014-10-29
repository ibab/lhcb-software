
'''
Module for construction of a StrippingLine taking all BeamGas triggers.
The Hlt requirement coincides with the definition of the BeamGas routing bit.
Line originally written by Plamen Hopchev and converted to the LineBuilder framwork by Thomas Latham.
'''

__author__ = ['Thomas Latham','Plamen Hopchev']
__date__ = '08/03/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('BeamGasConf')

from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine

default_config = {'Prescale' : 0.05, 'Postscale' : 1.0}

class BeamGasConf(LineBuilder) :
    '''
    Builder of pre-scaled BeamGas StrippingLine.
    Usage:
    >>> config = { ... }
    >>> bgConf = BeamGasConf('BeamGasTest',config)
    >>> bgLines = bgConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.
    '''

    __configuration_keys__ = ('Prescale','Postscale')

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self.line = StrippingLine( name+"Line"
                            , prescale = config['Prescale']
                            , HLT = "HLT_PASS_SUBSTR('Hlt1BeamGas')"
                            , checkPV = False
                            , postscale = config['Postscale']
                            )

        self.registerLine( self.line )

