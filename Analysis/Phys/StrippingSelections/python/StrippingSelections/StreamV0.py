#!/usr/bin/env
# =============================================================================
# @file StrippingSelections/StreamV0.py
#
# Stripping selections for V0-stripping
#
# @date : 2010-01-16
# =============================================================================
"""
Stripping selections for V0-stripping
"""
# =============================================================================

from Gaudi.Configuration             import *
from StrippingConf.StrippingStream   import StrippingStream
from StrippingSelections.StrippingV0 import StrippingV0Conf

stream = StrippingStream("V0")

v0 = StrippingV0Conf()

stream.appendLines( v0.lines() )

# =============================================================================
# The END 
# =============================================================================
