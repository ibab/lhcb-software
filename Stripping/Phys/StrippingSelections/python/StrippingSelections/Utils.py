'''
Helpers for construction of Stripping Selections
'''
__author__ = "Juan Palacios juan.palacios@cern.ch"
__date__ = "30/06/2010"

__all__ = ('lineBuilder')

def lineBuilder(stripping, lineBuilderName) :
    """
    Create a line builder from a stripping version and a line builder
    instance name.  The instance name must be registered in the database.
    Usage:
    lb = lineBuilder('Stripping13', 'B2XGamma')
    print lb.lines()
    """
    from StrippingSelections import lineBuilders
    from StrippingSettings.Utils import lineBuilderConfiguration
    _config = lineBuilderConfiguration(stripping, lineBuilderName)

    return lineBuilders()[_config['BUILDERTYPE']](lineBuilderName,
                                                  _config['CONFIG'])
