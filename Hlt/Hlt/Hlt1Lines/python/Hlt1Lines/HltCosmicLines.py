# =============================================================================
# $Id: HltCosmicLines.py,v 1.1 2009-09-10 15:12:35 snies Exp $
# =============================================================================
## @file
#  Cosmics 
#  @author Stephan Nies & Jesko Merkel 
#  @date 2009-09-02
# =============================================================================
"""
 script to configure Hlt lines for commissioning
"""
# =============================================================================
__author__  = "Stephan Nies & Jesko Merkel " 
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *


class HltCosmicLinesConf(HltLinesConfigurableUser):
 
    __slots__ = { 'Prescale' : { 'Hlt1OTCosmic'  : 1.0
                               }
                }
    def __apply_configuration__(self):

        from HltLine.HltLine import Hlt1Member as Member 
        from HltLine.HltDecodeRaw import DecodeOT
        from HltLine.HltLine import Hlt1Line as Line
        Line('OTCosmic'  
            , prescale = self.prescale
            , postscale = self.postscale
            , L0DU = "L0_ALL"
            , algos = [ DecodeOT, Member('CosmicsOT', 'CosmicsOT', OutputSelection = '%Decision') ]
            )
        #from Configurables import FilterByBankType
        #Line('Tell1Error'
        #    , ODIN='ODIN_ALL'
        #    , algos = [ FilterByBankType('Hlt1Tell1ErrorDecision' 
        #                                , PassSelectedEvents = True
        #                                , BankNames = [ ".*Error" ] 
        #                                )
        #              ]
        #    , prescale = self.prescale
        #    , postscale = self.postscale
        #    )
