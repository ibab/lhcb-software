'''
Helpers for construction of Archived Stripping Selections

Obtain both the configuration dictionary and the lineBuilders used for a given stripping by name

e.g. to get the configured line builder:

lb=lineBuilder("Stripping13","B0q2DplusMuX")

This can be used to get the outputLocation of a given line

lb.lines()[0].outputLocation()
'''
__author__ = "Rob Lambert rob.lambert@cern.ch"
__date__ = "30/06/2010"

__all__ = ('lineBuilder', 'buildStream','streamNames','cloneLinesFromStream','cloneStream','buildStreams','lineBuilderAndConf')

from StrippingSettings.Utils import lineBuilderConfiguration
from StrippingSettings.Utils import strippingConfiguration

def lineBuilder(stripping, lineBuilderName, archive=None) :
    """
    Create a line builder from a stripping version and a line builder
    instance name.  The instance name must be registered in the database.
    Usage:
    lb = lineBuilder('Stripping13', 'B2XGamma')
    print lb.lines()
    This already configures the line builder with the stripping cuts
    """
    from StrippingArchive import strippingArchive
    if isinstance(stripping, basestring) :
        _config = lineBuilderConfiguration(stripping, lineBuilderName)
        archive=strippingArchive(stripping)
    else :
        _config = stripping[lineBuilderName]

    return archive.lineBuilders()[_config['BUILDERTYPE']](lineBuilderName,
                                                          _config['CONFIG'])

def lineBuilderAndConf(stripping, lineBuilderName, archive=None):
    """
    If you want to try new cuts on an old line, better to get just the builder itself
    returns a tuple of (builder, config)
    modify config as you wish, then instantiate the lines

    builder,config = lineBuilderAndConf("Stripping13","B0q2DplusMuX")
    config["Tuned"]["MuPT"]=1000 #MeV / c
    lines=builder("MyB0q2DplusMuX",config).lines()
    """

    from StrippingArchive import strippingArchive
    if isinstance(stripping, basestring) :
        _config = lineBuilderConfiguration(stripping, lineBuilderName)
        archive=strippingArchive(stripping)
    else :
        _config = stripping[lineBuilderName]

    return (archive.lineBuilders()[_config['BUILDERTYPE']], _config['CONFIG'])


def buildStream(stripping, streamName = '', archive=None):
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
    from StrippingArchive import strippingArchive

    stream = StrippingStream( streamName )

    if isinstance(stripping, basestring) :
        _db = strippingConfiguration( stripping )
        archive=strippingArchive(stripping)
    else :
        _db = stripping

    for key in _db.keys():
        _conf = _db[key]
        if stream.name() in _conf['STREAMS']:
            _lb = archive.lineBuilders()[_conf['BUILDERTYPE']](key,_conf['CONFIG'])
            stream.appendLines( _lb.lines() )
    return stream

def streamNames(stripping) :
    """ return a list of all streams defined in a given stripping
    """
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
	Clone an entire stream, with a new prefix and a different prescale
	"""
	from StrippingConf.StrippingStream import StrippingStream
	lines = cloneLinesFromStream( stream, linePrefix, prescale )
	stream = StrippingStream( newStreamName )
	stream.appendLines( lines )
	return stream

def buildStreams(stripping,archive=None) :
    """
    Build and return a set of StrippingStreams for a given stripping
    configuration.
    Usage:

    >>> streams = buildStreams('Stripping13')
    >>> for s in streams :
    ...   print s.name(), s.lines


    It is also possible to use a configuration dictionary directly:

    >>> conf = strippingConfiguration('Stripping13')
    >>> archive = strippingArchive('Stripping13')
    >>> streams = buildStreams(conf,archive)

    """
    from StrippingConf.StrippingStream import StrippingStream
    from StrippingArchive import strippingArchive

    streams = {}
    if isinstance(stripping, basestring) :
        scdb = strippingConfiguration(stripping)
        archive=strippingArchive(stripping)
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
        builderMap[builder] = lineBuilder(scdb, builder, archive)

    strippingStreams=[]
    for stream, builderNames in streams.iteritems() :
        lines=[]
        for b in builderNames :
            lines +=  [line for line in builderMap[b].lines()]
        print 'Creating steam', stream, 'with lines', lines
        strippingStreams.append(StrippingStream(stream, Lines = lines))
    return strippingStreams

def lineBuilderClone(stripping, line=None, prefix='Clone', prescale=1.0, archive=None):
    '''
    Return a clone of a single lineBuilder from the stripping, with a further prescale
    '''
    _line=lineBuilder(stripping, line, archive)
    return _line.clone( prefix + _line.name().lstrip('Stripping'),
                                  prescale = _line.prescale()*prescale )

