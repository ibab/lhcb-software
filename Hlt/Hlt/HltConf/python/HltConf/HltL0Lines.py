#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltL0Lines.py,v 1.2 2009-01-09 12:05:40 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines which are plain L0 lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-12-02
# =============================================================================
"""
 script to configure Hlt lines corresponding to L0 triggers
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.2 $"
# =============================================================================

from LHCbKernel.Configuration import *
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltL0Candidates import *

class HltL0LinesConf(LHCbConfigurableUser):
# we postscale (instead of prescale!) these lines because 
# TOS needs these candidates to exist
   __slots__ = { 'Prescale'  : 1
               , 'Postscale' : 0.000001
               , 'L0Channels' : []  # if empty, use all pre-defined channels
               }

   def __apply_configuration__(self):
        channels = self.getProp('L0Channels')
        if not channels : channels = L0Channels()
        for channel in channels :
            Line ( 'L0' + channel 
                 , L0DU  = "L0_CHANNEL('"+channel+"')"
                 , prescale = self.getProp('Prescale')
                 , algos = [ convertL0Candidates(channel) ]
                 , postscale = self.getProp('Postscale')
                 )
