## $Id: DVTestInitialise.py,v 1.4 2010-02-23 10:33:21 jpalac Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $
## ============================================================================
"""
@file $Name: not supported by cvs2svn $

Test for DaVinci initialisation

@author P. Koppenburg
@date 2009-01-07
"""
from Gaudi.Configuration import *
from Configurables import DaVinci
DaVinci().MainOptions = ""
DaVinci().UserAlgorithms = [ ]

DaVinci().EvtMax = 0 # No running 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 
DaVinci().DataType = '2009'
DaVinci().Lumi = True
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

