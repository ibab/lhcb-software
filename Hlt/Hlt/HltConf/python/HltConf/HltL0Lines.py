# =============================================================================
# $Id: HltL0Lines.py,v 1.7 2009-03-25 08:38:54 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.7 $"
# =============================================================================

from HltConf.HltLinesConfigurableUser import *
from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltL0Candidates import *
from HltConf.HltLine     import hlt1Lines

class HltL0LinesConf(HltLinesConfigurableUser) :
   __slots__ = { 'Postscale' : { '.*' : 0.000001 } # set new default Postscale for these lines!
               , 'L0Channels' : []  # if empty, use all pre-defined channels
               }

   def __apply_configuration__(self):
        channels = self.getProp('L0Channels')
        if not channels : channels = L0Channels()
        for channel in channels :
            Line ( 'L0' + channel 
                 , prescale = self.prescale
                 , L0DU  = "L0_CHANNEL('"+channel+"')"
                 , algos = [ convertL0Candidates(channel) ]
                 , postscale = self.postscale
                 )
