"""
Configuration file for Standard Loose Kstar2Kpi
"""

__author__ = 'Diego Martinez Santos, P. Koppenburg'
__date__ = '29/06/2009'


from Gaudi.Configuration import *
from Configurables import FilterDesktop
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdLooseDetachetKst2Kpi #
###########################

StdLooseDetachedKst2Kpi = FilterDesktop("StdLooseDetachedKst2Kpi")
StdLooseDetachedKst2Kpi.Inputs = ["Phys/StdVeryLooseDetachedKst2Kpi/Particles"]
StdLooseDetachedKst2Kpi.Code = "(MIPCHI2DV(PRIMARY)> 2.25)"
locations = updateDoD(StdLooseDetachedKst2Kpi)


