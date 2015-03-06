#!/usr/bin/env py.test

## Agile-dev PATCH

import os
import sys 
os.environ['STRIPPINGSELECTIONSROOT'] = '/home/khurewat/cmtuser/DaVinciDev_v36r4p1/Phys/StrippingSelections'
sys.path.insert(0, os.environ['STRIPPINGSELECTIONSROOT']+'/python')

import pytest  # Sadly, there's no fixture in 2.2.4, too old!!


from StrippingSelections.Utils import buildStreams
from StrippingSelections import buildersConf
allconf = buildersConf() 


def clean():
  """Because the singleton are... everywhere..."""
  from GaudiKernel import Configurable
  Configurable.purge()
  from StrippingConf import StrippingLine
  StrippingLine._stripping_lines__ = []
  from SelPy.utils import UniquelyNamedObject
  UniquelyNamedObject._UniquelyNamedObject__used_names = []



#--------------

def test_buildStreams_from_bad():
  with pytest.raises(ValueError):
    buildStreams(None)
  with pytest.raises(ValueError):
    buildStreams({})


def test_buildStreams_from_string_good():
  buildStreams('stripping21')
  clean()

def test_buildStreams_from_string_bad():
  with pytest.raises(Exception):
    buildStreams('BADNAME')


def test_buildStreams_good_from_builderConf():
  buildStreams(allconf)  
  clean()


def test_buildStreams_filter_name():
  list_stream = buildStreams(allconf, 'Z02MuMu')
  assert len(list_stream)==1

  # Check stream 
  from StrippingConf.StrippingStream import StrippingStream
  stream = list_stream[0]
  assert isinstance(stream, StrippingStream)
  assert stream.name() == 'EW'

  # Check line member, after build
  assert len(stream.lines) == 1
  line = stream.lines[0]
  assert line.name() == 'StrippingZ02MuMuLine'

  clean()


def test_buildStreams_filter_name2():
  list_stream = buildStreams(allconf, ['Z02MuMu', 'B2ppipiSigmacmm_Lcpi'])
  assert len(list_stream)==2

  # Check stream 
  from StrippingConf.StrippingStream import StrippingStream
  for stream in list_stream:
    assert isinstance(stream, StrippingStream)
    assert stream.name() in ('EW', 'Charm')
    if stream.name()=='EW':
      assert len(stream.lines) == 1
      assert stream.lines[0].name() == 'StrippingZ02MuMuLine'
    if stream.name()=='Charm':
      assert len(stream.lines) == 7

  clean()



def test_buildStreams_badname():
  with pytest.raises(ValueError):
    buildStreams(allconf, 'BADNAME')


def test_buildStreams_badconf():
  allconf = { 'GOODNAME': { 'NOSTREAM': None } }
  with pytest.raises(KeyError):
    buildStreams(allconf, 'GOODNAME')

