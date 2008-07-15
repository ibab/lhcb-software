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
from  Configurables import LoKi__DCCheck            as DCCheck

importOptions( "$DAVINCIROOT/options/DaVinciCommon.opts" )
importOptions( "$COMMONPARTICLESROOT/options/StandardKaons.opts"   )
importOptions( "$COMMONPARTICLESROOT/options/StandardMuons.opts"   )
importOptions( "$COMMONPARTICLESROOT/options/StandardPions.opts"   )

## configure our own algorithm: 
alg = DCCheck('DCCheck_')
alg.addTool ( PhysDesktop ) 
alg.PhysDesktop.InputLocations = [ 'Phys/StdTightKaons'   ,
                                   'Phys/StdTightMuons'   ,
                                   'Phys/StdNoPIDPions'   ]
alg.DistanceCalculators = {
    "1" : "LoKi::DistanceCalculator/1" ,
    "2" : "LoKi::DistanceCalculator/2"
    }
alg.NTupleLUN = "DC"
alg.PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ]

alg2=alg.clone('DCCheck2')
alg2.DistanceCalculators = {
    "1" : "LoKi::DistanceCalculator/1"    ,
    "2" : "LoKi::DistanceCalculator/2"
    }

alg3=alg.clone('DCCheck3')
alg3.DistanceCalculators = {
    "1" : "LoKi::TrgDistanceCalculator/1" ,
    "2" : "LoKi::TrgDistanceCalculator/2"
    }

alg4=alg.clone('DCCheck4')
alg4.DistanceCalculators = {
    "1" : "LoKi::DistanceCalculator/1" ,
    "2" : "LoKi::TrgDistanceCalculator/2"
    }


## confgure the application itself:
appMgr = ApplicationMgr( EvtMax = 1000 )
#appMgr.TopAlg += [ alg  ]
#appMgr.TopAlg += [ alg2 ]
#appMgr.TopAlg += [ alg3 ]
appMgr.TopAlg += [ alg4 ]

## printout frequency
EventSelector ( PrintFreq = 10 ) 

## histograms:
HistogramPersistencySvc ( OutputFile = "DCCheck_Histos.root" )

## n-Tuples
NTupleSvc ( Output = [ "DC DATAFILE='DCCheck_Tuples.root' TYP='ROOT' OPT='NEW'" ] ) ;

## input data:
importOptions ( os.environ['LOKIEXAMPLEDATA'] )

# =============================================================================
# The END
# =============================================================================
