'''
Helpers for construction of Archived Stripping Selections

Obtain both the configuration dictionary and the lineBuilders used for a given stripping by name

e.g. to get the configured line builder:

lb=lineBuilder("Stripping13","B0q2DplusMuX")

This can be used to get the outputLocation of a given line

lb.lines()[0].outputLocation()

But, depending on the exact setup, the stream name will not appear in the outputLocation.

To find the true outputLocation as it was in the Stripping, it's better to use the outputLocations function:

locations=outputLocations('Stripping13','B0q2DplusMuX')

or usually if you need also to instantiate the line itself:

lb=lineBuilder("Stripping13","B0q2DplusMuX")
locations=outputLocations('Stripping13','B0q2DplusMuX',lines=lb.lines())
'''
__author__ = "Rob Lambert rob.lambert@cern.ch"
__date__ = "30/06/2010"

__all__ = ('lineBuilder', 'buildStream','streamNames','cloneLinesFromStream','cloneStream','buildStreams','lineBuilderAndConf', 'outputLocations')

from StrippingSettings.Utils import lineBuilderConfiguration as _lbConf
from StrippingSettings.Utils import strippingConfiguration as _sConf
from StrippingUtils.Utils import lineFromName as _lfName

def lineBuilder(stripping, lineBuilderName, archive=None) :
    """
    Create a line builder from a stripping version and a line builder
    instance name.  The instance name must be registered in the database.
    Usage:
    lb = lineBuilder('Stripping13', 'B2XGamma')
    print lb.lines()
    This already configures the line builder with the stripping cuts
    """
    if isinstance(stripping, basestring) :
        _config = _lbConf(stripping, lineBuilderName)
        from StrippingArchive import strippingArchive
        archive=strippingArchive(stripping)
    else :
        _config = stripping[lineBuilderName]

    return archive.lineBuilders()[_config['BUILDERTYPE']](lineBuilderName,
                                                          _config['CONFIG'])


def addBuilderToStreamGrouping( streams, config, lb ):
    """
    Append lines from a line builder to a dictionary
    mapping lines to stream names.
    """

    for stream in config['STREAMS']:
        if stream in streams:
            if isinstance(config['STREAMS'],dict):
                for linename in config['STREAMS'][stream]:
                    line = _lfName( lb,   linename )
                    if line:
                        streams[stream] += [ line ]
                    else:
                        raise Exception('The line you have requested does not exist '+linename)
            elif isinstance(config['STREAMS'],list):
                streams[stream] += [ line for line in lb.lines() ]
            else:
                raise Exception( 'Unsupported type, expected list' +
                                 'or dict for line-to-STREAM mapping' )
        else:
            if isinstance(config['STREAMS'],dict):
                for linename in config['STREAMS'][stream]:
                    line = _lfName( lb,   linename )
                    if line:
                        if stream not in streams:
                            streams[stream] = [ line ]
                        else:
                            streams[stream] += [ line ]
                    else:
                        raise Exception('The line you have requested does not exist '+linename)
            elif isinstance(config['STREAMS'],list):
                streams[stream] = [ line for line in lb.lines() ]
            else:
                raise Exception( 'Unsupported type, expected list' +
                                 'or dict for line-to-STREAM mapping' )
    return


def lineBuilderAndConf(stripping, lineBuilderName, archive=None):
    """
    If you want to try new cuts on an old line, better to get just the builder itself
    returns a tuple of (builder, config)
    modify config as you wish, then instantiate the lines

    builder,config = lineBuilderAndConf("Stripping13","B0q2DplusMuX")
    config["Tuned"]["MuPT"]=1000 #MeV / c
    lines=builder("MyB0q2DplusMuX",config).lines()
    """

    if isinstance(stripping, basestring) :
        _config = _lbConf(stripping, lineBuilderName)
        from StrippingArchive import strippingArchive
        archive=strippingArchive(stripping)
    else :
        _config = stripping[lineBuilderName]

    return (archive.lineBuilders()[_config['BUILDERTYPE']], _config['CONFIG'])


def buildStream(stripping, streamName = '', archive=None, WGs = None ):
    """
    Create a StrippingStream from the lineBuilder database
    Usage:
    >>> streamDimuon = strippingStream('Stripping13','Dimuon')
    or:
    >>> conf = strippingConfiguration('Stripping13')
    >>> streamDimuon = strippingStream(conf,'Dimuon')

    """

    from StrippingConf.StrippingStream import StrippingStream

    stream = StrippingStream( streamName )

    if isinstance(stripping, basestring) :
        _db = _sConf( stripping )
        from StrippingArchive import strippingArchive
        archive=strippingArchive(stripping)
    else :
        _db = stripping

    for key in _db.keys():
        _conf = _db[key]
        if stream.name() in _conf['STREAMS']:
            if WGs and not any( WG for WG in _conf['WGs'] if WG in WGs): continue
            _lb = archive.lineBuilders()[_conf['BUILDERTYPE']](key,_conf['CONFIG'])

            if isinstance(_conf['STREAMS'],dict):
                for linename in _conf['STREAMS'][stream.name()]:
                    line = _lfName( _lb, linename )
                    if line:
                        stream.appendLines( [ line ] )
                    else:
                        raise Exception('The line you have requested does not exist '+linename)
            elif isinstance(_conf['STREAMS'],list):
                stream.appendLines( _lb.lines() )
            else:
                raise Exception( 'Unsupported type, expected list ' +
                                 'or dict for line-to-STREAM mapping' )

            stream.appendLines( _lb.lines() )
    return stream

def streamNames(stripping) :
    """ return a list of all streams defined in a given stripping
    """
    from SelPy.utils import flattenList, removeDuplicates
    if isinstance(stripping, basestring) :
        _db = _sConf( stripping )
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

def buildStreams(stripping,archive=None, WGs = None) :
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

    streams = {}

    if isinstance(stripping, basestring) :
        scdb = _sConf(stripping)
        from StrippingArchive import strippingArchive
        archive=strippingArchive(stripping)
    else :
        scdb = stripping

    for k, v in scdb.iteritems() :
        if 'STREAMS' in v.keys() and not WGs or 'WGs' in v.keys():
            if not WGs or any( WG for WG in v['WGs'] if WG in WGs ):
                lb = archive.lineBuilders()[v['BUILDERTYPE']](k,v['CONFIG'])
                addBuilderToStreamGrouping( streams, v, lb )
        else:
            raise Exception('Config',k,'missing either STREAM or WG data data.')

    strippingStreams=[]
    for stream in streams:
        lines = [ line for line in streams[stream] ]
        print ( 'Creating ' + stream + ' stream with '
                + str( len(lines) ) + ' lines' )
        strippingStreams.append( StrippingStream( stream, Lines = lines ) )

    return strippingStreams

def lineBuilderClone(stripping, line=None, prefix='Clone', prescale=1.0, archive=None):
    '''
    Return a clone of a single lineBuilder from the stripping, with a further prescale
    '''
    _line=lineBuilder(stripping, line, archive)
    return _line.clone( prefix + _line.name().lstrip('Stripping'),
                                  prescale = _line.prescale()*prescale )

def outputLocations(stripping, lineName, lines=[], archive=None):
    '''
    Return the output location(s) [as a list] of a given line in a given stripping
    if lines is an empty list, outputLocations will instantiate the lines itself
    or else it is assumed they are proper line objects with an outputLocation method

    e.g.:

    line=lineBuilder('Stripping15','B0q2DplusMuX')
    oneline=line.lines()[0]

    location=outputLocations('Stripping15', 'B0q2DplusMuX', [oneline])

    Essentially it returns /Event/<stream>/Phys/<line>/Particles, avoiding the need for
    the user to construct this manually
    '''

    if isinstance(stripping, basestring) :
        _db = _sConf( stripping )
        from StrippingArchive import strippingArchive
        archive=strippingArchive(stripping)
    else :
        _db = stripping

    if lineName not in _db:
        raise KeyError, "the line "+lineName+" does not exist in the chosen stripping"

    if len(lines)==0:
        _line=lineBuilder(_db, lineName, archive)
        lines=_line.lines()

    _conf=_db[lineName]

    retlist=[]
    #add individual lines if the lines are a dictionary
    if isinstance(_conf['STREAMS'],dict):
            for stream in _conf['STREAMS']:
                for linename in _conf['STREAMS'][streams]:
                    line = lineFromList( _line, lines )
                    if line: retlist.append('/Event/'+stream+'/'+line.outputLocation().replace('/Event','').strip('/'))
    #add whole list if the streams are a list
    elif isinstance(_conf['STREAMS'],list):
        for aline in lines:
            for stream in _conf['STREAMS']:
                retlist.append('/Event/'+stream+'/'+aline.outputLocation().replace('/Event','').strip('/'))

    else:
        raise Exception( 'Unsupported type, expected list ' +
                         'or dict for line-to-STREAM mapping' )
    return retlist


def lineFromList(lines, lineName) :
    """
    Get the line that has a given name. If not available, return None.
    """
    rlines = filter(lambda x: x.name()==lineName , lines)
    if len(rlines)>1 :
        raise Exception("More than one line with name ", lineName)
    if len(rlines) == 0 : return None
    return rlines[0]
