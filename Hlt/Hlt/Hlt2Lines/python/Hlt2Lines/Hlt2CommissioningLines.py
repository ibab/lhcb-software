# =============================================================================
# $Id: Hlt2CommissioningLines.py,v 1.1 2009-08-11 12:22:22 graven Exp $
# =============================================================================
## @file
#  Configuration of Hlt Lines for commissioning
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2009-08-11
# =============================================================================
"""
 script to configure Hlt2 lines for commissioning
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import *
from HltLine.HltLine import Hlt2Line   as Line


class Hlt2CommissioningLinesConf(HltLinesConfigurableUser):

   __slots__ = { 'Prescale' : { 'Hlt2Hlt1PassThrough'  : 0.0001 
                              }
               }
   def __apply_configuration__(self):
        Line('Hlt1PassThrough' ,  HLT = "HLT_PASS('Hlt1Global')"
            , prescale = self.prescale
            , postscale = self.postscale
            )
        Line('Transparant' ,  HLT = "HLT_PASS_RE('^Hlt1(Incident|Tell1Error|(Non)?RandomODIN|Velo.*|Align.*|L0.*|Lumi.*)Decision$')"
            , prescale = self.prescale
            , postscale = self.postscale
            )
