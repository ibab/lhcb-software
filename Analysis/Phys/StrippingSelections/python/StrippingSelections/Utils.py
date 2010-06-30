'''
Helpers for construction of Stripping Selections
'''
__author__ "Juan Palacios juan.palacios@cern.ch"
__date__ "30/06/2010"

__all__ = ('checkConfig')

def checkConfig(reference_keys, configuration) :
    """
    Check that all the required configuration parameters are present
    in configuration.keys(), and that the number of keys in configuration
    are as expected.
    """
    absentKeys = []
    for key in reference_keys :
        if key not in configuration.keys():
            absentKeys.append(key)

    if len(absentKeys) != 0 :
        raise KeyError('Keys missing in configuration: '+ str(absentKeys))

    if len(configuration.keys()) != len(reference_keys) :
        raise KeyError('Configuration has unexpected number of parameters.')
