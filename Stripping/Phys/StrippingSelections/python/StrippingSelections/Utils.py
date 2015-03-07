'''
Helpers for construction of Stripping Selections
'''
__author__ = "Juan Palacios juan.palacios@cern.ch"
__date__ = "30/06/2010"

__all__ = ('lineBuilder')

import inspect
from os import environ
from pprint import *
from Gaudi.Configuration import *

from StrippingSettings.Utils import lineBuilderConfiguration
from StrippingSettings.Utils import strippingConfiguration

def checkConfig(reference_keys, configuration) :
    # IMPORTANT: don't remove this function
    # as it is needed to check the correctness of
    # LineBuilder configurations
    pass


def lineBuilder(stripping, lineBuilderName, WGs = None) :
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

    try:
        _lb = lineBuilders(WGs)[_config['BUILDERTYPE']](lineBuilderName,_config['CONFIG'])
    except Exception, x:
        log.error("Unable to configure %s because of %s" %(_config['BUILDERTYPE'],str(x)))
    else:
        return _lb


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



def addBuilderToStreamGrouping( streams, config, lb ):
    """
    Append lines from a line builder to a dictionary
    mapping lines to stream names.
    """
    from StrippingUtils.Utils import lineFromName

    for stream in config['STREAMS']:
        if stream in streams:
            if isinstance(config['STREAMS'],dict):
                for linename in config['STREAMS'][stream]:
                    line = lineFromName( lb,   linename )
                    if line:
                        streams[stream] += [ line ]
                    else:
                        raise Exception('The line you have requested does not exist '+linename + \
                                        ", stream " + stream + ", builder " + config['BUILDERTYPE'])
            elif isinstance(config['STREAMS'],list):
                streams[stream] += [ line for line in lb.lines() ]
            else:
                raise Exception( 'Unsupported type, expected list' +
                                 'or dict for line-to-STREAM mapping' )
        else:
            if isinstance(config['STREAMS'],dict):
                for linename in config['STREAMS'][stream]:
                    line = lineFromName( lb,   linename )
                    if line:
                        if stream not in streams:
                            streams[stream] = [ line ]
                        else:
                            streams[stream] += [ line ]
                    else:
                        raise Exception('The line you have requested does not exist '+linename + \
                                        ", stream " + stream + ", builder " + config['BUILDERTYPE'])
            elif isinstance(config['STREAMS'],list):
                streams[stream] = [ line for line in lb.lines() ]
            else:
                raise Exception( 'Unsupported type, expected list' +
                                 'or dict for line-to-STREAM mapping' )
    return


def buildStream( stripping, streamName = '', WGs = None ):
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
    from StrippingUtils.Utils import lineFromName

    stream = StrippingStream( streamName )

    if isinstance(stripping, basestring) :
        _db = strippingConfiguration( stripping )
    else :
        _db = stripping


    for key in _db.keys():
        _conf = _db[key]
        if stream.name() in _conf['STREAMS']:
            if WGs and not any( WG for WG in _conf['WGs'] if WG in WGs): continue
            try:
                _lb = lineBuilders()[_conf['BUILDERTYPE']](key,_conf['CONFIG'])
            except Exception, x:
                log.error("Unable to configure %s because of %s" %(_conf['BUILDERTYPE'], str(x)))
            else:
                if isinstance(_conf['STREAMS'],dict):
                    for linename in _conf['STREAMS'][stream.name()]:
                        line = lineFromName( _lb, linename )
                        if line:
                            stream.appendLines( [ line ] )
                        else:
                            raise Exception('The line you have requested does not exist '+linename)
                elif isinstance(_conf['STREAMS'],list):
                    stream.appendLines( _lb.lines() )
                else:
                    raise Exception( 'Unsupported type, expected list ' +
                                     'or dict for line-to-STREAM mapping' )
    return stream


def buildStreams( stripping, WGs = None ):
    """
    Build and return a set of StrippingStreams for a given
    stripping configuration.
    Usage:

    >>> streams = buildStreams('Stripping13')
    >>> for s in streams :
    ...   print s.name(), s.lines


    It is also possible to use a configuration dictionary
    directly:

    >>> conf = strippingConfiguration('Stripping13')
    >>> streams = buildStreams(conf)

    To select only streams belonging to the RD or the
    Gamma from trees WGs:

    >>> streams = buildStreams('stripping13', WGs = ['RD','GammaFromTrees'] )
    >>>

    """
    from StrippingConf.StrippingStream import StrippingStream
    from StrippingSettings.Utils import strippingConfiguration
    from StrippingSelections import lineBuilders

    streams = {}

    if isinstance(stripping, basestring) :
        scdb = strippingConfiguration(stripping)
    else :
        scdb = stripping

    for k, v in scdb.iteritems() :
        if 'STREAMS' in v.keys() and not WGs or 'WGs' in v.keys():
            if not WGs or any( WG for WG in v['WGs'] if WG in WGs ):
                try:
                    lb = lineBuilders()[v['BUILDERTYPE']](k,v['CONFIG'])
                except Exception, x:
                    log.error("Unable to configure %s because of %s" %(v['BUILDERTYPE'],str(x)))
                else:
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

def buildStreamsFromBuilder(conf, names ):
    """
    Build and return a set of StrippingStreams for a given line builder configuration
    Usage:

    >>> streams = buildStreamsFromBuilder(somelinebuilder, 'B2ppipiSigmacmm_Lcpi')
    """
    
    from StrippingConf.StrippingStream import StrippingStream
    from StrippingSettings.Utils import strippingConfiguration
    from StrippingSelections import lineBuilders

    streams = {}

    if not isinstance(names, list):
      names = [names]
    
    for name in names:
      _conf = {}
      if conf.has_key(name) :
        _conf[name] = conf[name]
      else:
        log.error("The requested name %s is not present in the default_config of any LineBuilder" %name) 
    
      #_conf=lb.default_config
      #if isinstance(stripping, basestring) :
          #scdb = strippingConfiguration(stripping)
      #else :
          #scdb = stripping

      for k, v in _conf.iteritems() :
          if 'STREAMS' in v.keys():
            
              try:
                  lb = lineBuilders()[v['BUILDERTYPE']](k,v['CONFIG'])
              except Exception, x:
                  log.error("Unable to configure %s because of %s" %(v['BUILDERTYPE'],str(x)))
              else:
                  addBuilderToStreamGrouping( streams, v, lb )
          else:
              raise Exception('Config',k,'missing either STREAM.')
    ##

    strippingStreams=[]
    for stream in streams:
        lines = [ line for line in streams[stream] ]
        print ( 'Creating ' + stream + ' stream with '
                + str( len(lines) ) + ' lines' )
        strippingStreams.append( StrippingStream( stream, Lines = lines ) )
    return strippingStreams

## def buildStream(stripping, streamName = ''):
##     """
##     Create a StrippingStream from the lineBuilder database
##     Usage:
##     >>> streamDimuon = strippingStream('Stripping13','Dimuon')
##     or:
##     >>> conf = strippingConfiguration('Stripping13')
##     >>> streamDimuon = strippingStream(conf,'Dimuon')

##     """

##     from StrippingConf.StrippingStream import StrippingStream
##     from StrippingSettings.Utils import strippingConfiguration
##     from StrippingSelections import lineBuilders

##     stream = StrippingStream( streamName )

##     if isinstance(stripping, basestring) :
##         _db = strippingConfiguration( stripping )
##     else :
##         _db = stripping

##     for key in _db.keys():
##         _conf = _db[key]
##         if stream.name() in _conf['STREAMS']:
##             _lb = lineBuilders()[_conf['BUILDERTYPE']](key,_conf['CONFIG'])
##             stream.appendLines( _lb.lines() )
##     return stream

## def buildStreams(stripping) :
##     """
##     Build and return a set of StrippingStreams for a given stripping
##     configuration.
##     Usage:

##     >>> streams = buildStreams('Stripping13')
##     >>> for s in streams :
##     ...   print s.name(), s.lines


##     It is also possible to use a configuration dictionary directly:

##     >>> conf = strippingConfiguration('Stripping13')
##     >>> streams = buildStreams(conf)

##     """
##     from StrippingConf.StrippingStream import StrippingStream
##     streams = {}
##     if isinstance(stripping, basestring) :
##         scdb = strippingConfiguration(stripping)
##     else :
##         scdb = stripping

##     for k, v in scdb.iteritems() :
##         if 'STREAMS' in v.keys() :
##             for stream in v['STREAMS'] :
##                 if stream in streams.keys() :
##                     streams[stream] += [k]
##                 else :
##                     streams[stream] = [k]
##         else :
##             print 'ERROR: config',k,'had no STREAMS data. Ignore!!!'
##     builderMap = {}
##     for builder in scdb.keys() :
##         builderMap[builder] = lineBuilder(scdb, builder)

##     strippingStreams=[]
##     for stream, builderNames in streams.iteritems() :
##         lines=[]
##         for b in builderNames :
##             lines +=  [line for line in builderMap[b].lines()]
##         print 'Creating steam', stream, 'with lines', lines
##         strippingStreams.append(StrippingStream(stream, Lines = lines))
##     return strippingStreams

