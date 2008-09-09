"""
High level configuration tools for Gauss
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-09-09 16:27:22 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import GeV, ns, ms
from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime )
from Configurables import GiGaGeo
import GaudiKernel.ProcessJobOptions
