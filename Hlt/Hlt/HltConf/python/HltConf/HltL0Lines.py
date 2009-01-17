# =============================================================================
# $Id: HltL0Lines.py,v 1.6 2009-01-17 08:46:30 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.6 $"
# =============================================================================

from LHCbKernel.Configuration import *
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltL0Candidates import *
from HltConf.HltLine     import hlt1Lines

class HltL0LinesConf(LHCbConfigurableUser) :
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
