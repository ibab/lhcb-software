'''
Helpers for construction of Stripping Selections
'''
__author__ = "Juan Palacios juan.palacios@cern.ch"
__date__ = "30/06/2010"

__all__ = ('lineBuilder')

from StrippingSettings.Utils import lineBuilderConfiguration
from StrippingSettings.Utils import strippingConfiguration

def checkConfig(reference_keys, configuration) :
    pass
def lineBuilder(stripping, lineBuilderName) :
    """
    Create a line builder from a stripping version and a line builder
    instance name.  The instance name must be registered in the database.
    Usage:
    lb = lineBuilder('Stripping13', 'B2XGamma')
    print lb.lines()
    """
    from StrippingSelections import lineBuilders
    if isinstance(stripping, basestring) :
        _config = lineBuilderConfiguration(stripping, lineBuilderName)
    else :
        _config = stripping[lineBuilderName]

    return lineBuilders()[_config['BUILDERTYPE']](lineBuilderName,
                                                  _config['CONFIG'])

def buildStream(stripping, streamName = ''):
    """
    Create a StrippingStream from the lineBuilder database
    Usage:
    >>> streamDimuon = strippingStream('Stripping13','Dimuon')
    or:
    >>> conf = strippingConfiguration('Stripping13')
    >>> streamDimuon = strippingStream(conf,'Dimuon')
    
    """
    
    from StrippingConf.StrippingStream import StrippingStream
    from StrippingSettings.Utils import strippingConfiguration
    from StrippingSelections import lineBuilders

    stream = StrippingStream( streamName )

    if isinstance(stripping, basestring) :
        _db = strippingConfiguration( stripping )
    else :
        _db = stripping
        
    for key in _db.keys():
        _conf = _db[key]
        if stream.name() in _conf['STREAMS']:
            _lb = lineBuilders()[_conf['BUILDERTYPE']](key,_conf['CONFIG'])
            stream.appendLines( _lb.lines() )
    return stream

def streamNames(stripping) :
    from SelPy.utils import flattenList, removeDuplicates
    from StrippingSettings.Utils import strippingConfiguration
    if isinstance(stripping, basestring) :
        _db = strippingConfiguration( stripping )
    else :
        _db = stripping

    streams =  [v['STREAMS'] for v in _db.values()]
    return list(set(flattenList(streams)))

def cloneLinesFromStream(stream, prefix = 'Clone' , prescale = 1.0):
    """
    Clone the lines belonging to a StrippingStream with a
    global prescale and return them as a flat list
    >>> stream = StrippingStream('Test')
    >>> lines  = cloneLinesFromStream(stream,prefix='Clone',prescale=1.0)
    """

    clonedLines = []
    
    for _line in stream.lines:
        clonedLine = _line.clone( prefix + _line.name().lstrip('Stripping'),
                                  prescale = _line.prescale()*prescale )
        clonedLines.append( clonedLine )
    return clonedLines


def cloneStream( stream, newStreamName = 'Full', linePrefix = 'Full', prescale = 1.0 ):
	"""
	Clone the stream
	"""
	from StrippingConf.StrippingStream import StrippingStream
	lines = cloneLinesFromStream( stream, linePrefix, prescale )
	stream = StrippingStream( newStreamName )
	stream.appendLines( lines )
	return stream

def buildStreams(stripping) :
    """
    Build and return a set of StrippingStreams for a given stripping
    configuration.
    Usage:

    >>> streams = buildStreams('Stripping13')
    >>> for s in streams :
    ...   print s.name(), s.lines


    It is also possible to use a configuration dictionary directly:

    >>> conf = strippingConfiguration('Stripping13')
    >>> streams = buildStreams(conf)
    
    """
    from StrippingConf.StrippingStream import StrippingStream
    streams = {}
    if isinstance(stripping, basestring) :
        scdb = strippingConfiguration(stripping)
    else :
        scdb = stripping
        
    for k, v in scdb.iteritems() :
        if 'STREAMS' in v.keys() :
            for stream in v['STREAMS'] :
                if stream in streams.keys() :
                    streams[stream] += [k]
                else :
                    streams[stream] = [k]
        else :
            print 'ERROR: config',k,'had no STREAMS data. Ignore!!!'
    builderMap = {}
    for builder in scdb.keys() :
        builderMap[builder] = lineBuilder(scdb, builder)

    strippingStreams=[]
    for stream, builderNames in streams.iteritems() :
        lines=[]
        for b in builderNames :
            lines +=  [line for line in builderMap[b].lines()] 
        print 'Creating steam', stream, 'with lines', lines
        strippingStreams.append(StrippingStream(stream, Lines = lines))
    return strippingStreams


    
    
