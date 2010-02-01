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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *

class Hlt1L0LinesConf(HltLinesConfigurableUser) :
   __slots__ = { 'Postscale' : { 'Hlt1L0(?!Any).*' : 0.000001 } # set new default Postscale for these lines!
               , 'Prescale'  : { 'Hlt1L0Any'       : 0.000001 }
               , 'L0Channels' : []  # if empty, use all pre-defined channels
               }

   def __apply_configuration__(self):
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine     import hlt1Lines
        from Hlt1Lines.HltL0Candidates import convertL0Candidates,L0Channels
        channels = self.getProp('L0Channels')
        if not channels : channels = L0Channels()
        for channel in channels :
            converter = convertL0Candidates(channel) 
            Line ( 'L0' + channel 
                 , prescale = self.prescale
                 , L0DU  = "L0_CHANNEL('%s')" % channel
                 , algos = [ converter ] if converter else []
                 , postscale = self.postscale
                 )
        Line('L0Any' ,  L0DU = 'L0_DECISION' 
            , prescale = self.prescale
            , postscale = self.postscale
            )
        #Line('L0Forced', L0DU = 'L0_FORCEBIT'
        #    , prescale = self.prescale
        #    , postscale = self.postscale
        #    )
