## $Id: DVTestInitialise.py,v 1.1 2009-01-07 18:26:22 pkoppenb Exp $
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
##
## Truth checking 
##
from Configurables import AlgorithmCorrelationsAlg
##
## Correlations
##
from Configurables import AlgorithmCorrelationsAlg, AlgorithmCorrelations
TestCorrelations = AlgorithmCorrelationsAlg("TestCorrelations")
TestCorrelations.addTool(AlgorithmCorrelations())
TestCorrelations.AlgorithmCorrelations.OnlyNonZero = False
TestCorrelations.Algorithms = [ "Bu2LLK09_DiLepton", "Presel09Bu2LLK", "Sel09Bu2LLK" ]
###
# Selection
###
from Configurables import DaVinci
DaVinci().MainOptions = "$B2DILEPTONROOT/options/Do09selBu2LLK.py"
DaVinci().UserAlgorithms = [ TestCorrelations ]

DaVinci().EvtMax = 0 # No running 
DaVinci().SkipEvents = 0 
DaVinci().PrintFreq = 1 


