#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: HltL0Lines.py,v 1.5 2008-12-17 21:36:56 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.5 $"
# =============================================================================

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltL0Candidates import *

for i in L0Channels(): 
    Line ( 'L0' + i 
         , L0DU  = "L0_CHANNEL('"+i+"')"
         , prescale = 0.000001
         , algos = convertL0Candidates(i)
         )

