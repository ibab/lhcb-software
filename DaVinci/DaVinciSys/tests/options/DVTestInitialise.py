## $Id: DVTestInitialise.py,v 1.2 2009-01-09 13:29:54 pkoppenb Exp $
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


