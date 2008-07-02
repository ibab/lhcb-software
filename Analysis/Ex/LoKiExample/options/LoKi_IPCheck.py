#!/usr/bin/env gaudirun.py
# =============================================================================
## @file
#  The configuration file to run LoKi_Bs2PsiPhi example
#
#   This file is a part of LoKi project - 
#     "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-06-07
# =============================================================================
import os 
from  Gaudi.Configuration import * 
from  Configurables import PhysDesktop
from  Configurables import LoKi__IPCheck as IPCheck

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
importOptions( "$COMMONPARTICLESROOT/options/StandardKaons.opts"   )
importOptions( "$COMMONPARTICLESROOT/options/StandardKaons.opts"   )
importOptions( "$COMMONPARTICLESROOT/options/StandardPions.opts"   )
importOptions( "$COMMONPARTICLESROOT/options/StandardProtons.opts" )

## configure our own algorithm: 
alg = IPCheck('IPCheck')
alg.addTool ( PhysDesktop() )
alg.PhysDesktop.InputLocations = [ 'Phys/StdTightKaons'   ,
                                   'Phys/StdTightMuons'   ,
                                   'Phys/StdTightPions'   ,
                                   'Phys/StdTightProtons' ]
alg.NTupleLUN = "IP"
alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]

## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 1000 )
appMgr.TopAlg += [ alg ]

## printout frequency
EventSelector ( PrintFreq = 10 ) 

## histograms:
HistogramPersistencySvc ( OutputFile = "IPCheck_Histos.root" )

## n-Tuples
NTupleSvc ( Output = [ "IP DATAFILE='IPCheck_Tuples.root' TYP='ROOT' OPT='NEW'" ] ) ;

## input data:
importOptions ( os.environ['LOKIEXAMPLEDATA'] )

# =============================================================================
# The END
# =============================================================================
