""" Test for DaVinci HLT initialisation

@author P. Koppenburg
@date 2010-10-11
"""
from Gaudi.Configuration import *
from Configurables import DaVinci
DaVinci().MainOptions = ""
DaVinci().UserAlgorithms = [ ]

DaVinci().EvtMax = 0 # No running 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 
DaVinci().DataType = '2010'
DaVinci().Hlt = True
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

