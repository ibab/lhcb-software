'''
Helpers for construction of Stripping Selections
'''
__author__ = "Juan Palacios juan.palacios@cern.ch"
__date__ = "30/06/2010"

__all__ = ('checkConfig',
           'LineBuilder',
           'lineNames',
           'outputLocations',
           'lineFromName'
           )


from StrippingConf.Configuration import StrippingLine

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

def lineNames(lineBuilder) :
    """
    Return a list with all the names of the lines handled by a lineBuilder.
    """
    return tuple([line.name() for line in lineBuilder.lines()])

def outputLocations(lineBuilder) :
    """
    Return a list with all the output locations of the lines handled by
    a lineBuilder.
    """
    return tuple([line.outputLocation() for line in lineBuilder.lines()])

def lineFromName(lineBuilder, lineName) :
    """
    Get the line that has a given name. If not available, return None.
    """
    lines = filter(lambda x: x.name()==lineName , lineBuilder.lines())
    if len(lines)>1 :
        raise Exception("More than one line with name ", lineName)
    if len(lines) == 0 : return None
    return lines[0]

class LineBuilder(object) :

    """

    """

    __configuration_keys__ = ()

    lineNames = lineNames
    outputLocations = outputLocations
    lineFromName = lineFromName

    def __init__(self, 
                 name,
                 config) :
        
        checkConfig(self.configKeys(),
                    config)
        self._config = config
        self._name = name
        self._lines = []

    def configKeys(self) :
        return self.__configuration_keys__

    def name(self) :
        return self._name

    def configurationParameter(self, key) :
        return self._config[key]

    def registerLine(self, line) :
        self.checkLine(line)
        self._lines += [line]

    def checkLine(self, line) :
        assert type(line) == StrippingLine
        # add other checks here. For example, outputLocations.
        # Do all lines have one?
        
    def lines(self) :
        return tuple(self._lines)


