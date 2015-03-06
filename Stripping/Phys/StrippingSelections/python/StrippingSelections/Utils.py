'''
Helpers for construction of Stripping Selections
'''
__author__ = "Juan Palacios juan.palacios@cern.ch"
__date__   = "30/06/2010"

__all__ = ( 'lineBuilder', )

# import inspect
# from os import environ
# from pprint import *
# from Gaudi.Configuration import *
# def checkConfig(reference_keys, configuration) :
    # pass


if 'log' not in globals():
  import logging
  log = logging.getLogger(__name__)


def lineBuilder(stripping, lineBuilderName, WGs = None) :
  """
  Create a line builder from a stripping version and a line builder
  instance name.  The instance name must be registered in the database.
  
  Usage:
      lb = lineBuilder('Stripping13', 'B2XGamma')
      print lb.lines()
  """
  from StrippingSettings.Utils import lineBuilderConfiguration
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
    from SelPy.utils import flattenList
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


def buildStreams( src_scdb, request_keys=None ):
  """
  Build and return a set of StrippingStreams for a given line builder configuration

  Usage: 

  ## Using builderConf as source (builderConf is a dict of name:conf )
  >>> builder = StrippingSelections.builderConf()
  >>> streams = buildStreams(builder)

  ## Using stripping name (string) as source, shortcut via strippingConfiguration
  >>> streams = buildStreams('stripping21')
  
  ## Filter by WG 
  >>> streams = buildStreams(confs, 'EW')                  # Single
  >>> streams = buildStreams(confs, ['EW', 'Charm'])       # List

  ## Filter by line name
  >>> streams = buildStreams(confs, 'Z02MuMu')               # Single
  >>> streams = buildStreams(confs, ['Z02MuMu', 'MuMuSS'])   # List

  ## Try not to mix the request together, not cool.
  >>> streams = buildStreams(confs, ['EW', 'Z02MuMu'])
  ValueError: Invalid filter requested.

  Args:
    src_scdb (str,dict): Can be either 
                          - String, the stripping name as indexed in strippingConfiguration
                          - builder, a dict of { name:LineBuilder }
    request_keys (str,list): Whitelist filter, either by WG name or lineBuilder's name.
  """
  
  from collections import defaultdict
  from StrippingConf.StrippingStream import StrippingStream
  from StrippingSelections import lineBuilders
  from StrippingSettings.Utils import strippingConfiguration

  ## First, identfy the type if database.
  if isinstance(src_scdb, basestring):
    allconf = strippingConfiguration(src_scdb)
  elif isinstance(src_scdb, dict) and len(src_scdb)>0:
    allconf = src_scdb
  else:
    raise ValueError('Unexpected argument `src_scdb`:\n {}'.format(src_scdb))

  ## Prepare set of all WG names
  import os
  dirname  = os.path.expandvars('$STRIPPINGSELECTIONSROOT/python/StrippingSelections')
  allwgs   = { s.replace('Stripping','') for s in os.listdir(dirname) if os.path.isdir(os.path.join(dirname, s)) }
  allnames = allconf.keys()

  ## Clean request_keys 
  if request_keys is None:
    request_keys = set()
  elif isinstance(request_keys, basestring):
    request_keys = set([request_keys])
  elif isinstance(request_keys, list) or isinstance(request_keys, tuple):
    request_keys = set(request_keys)
  else:
    raise TypeError('Invalid request type: %r'%request_keys)

  ## Identify query
  if not request_keys:
    anywg   = True
    anyname = True
  elif request_keys.issubset(allwgs) and not request_keys.intersection(allnames):
    anywg   = False
    anyname = True
  elif request_keys.issubset(allnames) and not request_keys.intersection(allwgs):
    anywg   = True
    anyname = False
  else:
    raise ValueError('Invalid filter requested. Please provide correct WG/line name(s).\n%r'%request_keys)


  ## Prepare queue of conf, then build into a line, inflate & group by streamname
  lbuilders = lineBuilders()    # Instantiate once, before loop
  streams   = defaultdict(list)
  for name,conf in allconf.iteritems():
    if anyname or (name in request_keys):
      try:
        lines = lbuilders[conf['BUILDERTYPE']](name,conf['CONFIG']).lines()
        for wg in conf['WGs']:
          if anywg or (wg in request_keys):
            for streamname in conf['STREAMS']:
              streams[streamname].extend(lines)  # hmm, can there be duplication here?
      except Exception, x:
        log.exception("Unable to configure %s\n%s" %(conf['BUILDERTYPE'], repr(x)))

  ## Finally, return list of StrippingStream object
  return [ StrippingStream(streamname, lines) for streamname,lines in streams.iteritems() ]



def buildStreamsFromBuilder(conf, name):
  log.exception('Function depreciated. Recommending `StrippingSelections.buildStreams` instead.')
  return buildStreams(conf, name)

def buildStream( *args, **kwargs ):
  # Via grep, I found no usage of this function anymore...
  raise Exception('Function depreciated. Recommending `StrippingSelections.buildStreams` instead.')

