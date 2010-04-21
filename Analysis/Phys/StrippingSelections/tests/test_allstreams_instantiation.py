#!/usr/bin/env python
'''
 Test that Stripping Streams can be instantiated.
 This is the most basic test that should be run when modifying or
 adding Stripping Streams to StrippingSelections.Streams.allStreams.
'''
__author__ = 'Juan Palacios juan.palacios@nikhef.nl'

from StrippingConf.Configuration import StrippingConf
from StrippingSelections.Streams import allStreams

sc = StrippingConf (Streams = allStreams)
