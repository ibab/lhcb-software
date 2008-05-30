from Gaudi.Configuration import *

appName            = "BoxesAlignment"
appVersion         = "test"
nIter              = 1
nEvents            = 20
minNumHits         = 100
useDrift           = False
useCorr            = True
applyMS            = True
runFullPatRec      = False
simplifiedGeom     = True
alignDoFs          = "TxTyRyRz"
constraintStrategy = 1
constraints        = [] # [ "Tx", "Ty", "Tz", "Rx", "Ry", "Rz", "Szx", "Szy", "Szz" ]
solver             = "DiagSolvTool"
Misaligned         = 'none' # 'none', 'xml' or 'sqlite'
CutStrategy        = 'none' # 'none', 'trackChi2' or 'fitMatchChi2'

from Alignables import *
elements = Alignables()
elements.ITBoxes( alignDoFs )

trackingOpts = "$TALIGNMENTROOT/python/ITOTTracking.py"

from tapes import *
data   = MinBiasDSTs
output = "MinBias"

# Go past this line only when you know what you are doing
###########################################################################
## File to setup Alignment

ApplicationMgr( AppName    = appName, AppVersion    = appVersion,
                AuditTools = True,    AuditServices = True,       AuditAlgorithms = True )

from Configurables import LbAppInit
ApplicationMgr().TopAlg.append( LbAppInit( ApplicationMgr().AppName ) )

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.printing_level += 1

from AlConfigurable import *

## AlternativeDB
if Misaligned == 'sqlite':
   AlConfigurable().AlternativeCondDB         = "/home/lnicolas/cmtuser/MyLHCbCond/AWWMISALHCBCOND2.db/LHCBCOND"
   AlConfigurable().AlternativeCondDBTag      = "AW2_IT_OT_03"
   AlConfigurable().AlternativeCondDBOverlays = [ "/Conditions/IT", "/Conditions/OT"]

## Patttern Recognition?
AlConfigurable().Pat                          = runFullPatRec
AlConfigurable().SimplifiedGeom               = simplifiedGeom

## Set output level
AlConfigurable().OutputLevel                  = INFO

## Configure alignment
AlConfigurable().ElementsToAlign              = elements
AlConfigurable().NumIterations                = nIter
if AlConfigurable().getProp( "Pat" ) == True:
   AlConfigurable().AlignInputTackCont        = "Alignment/FilteredTracks"
else:
   AlConfigurable().AlignInputTackCont        = "Alignment/AlignmentTracks"
AlConfigurable().UseCorrelations              = useCorr
AlConfigurable().ApplyMS                      = applyMS
AlConfigurable().CanonicalConstraintStrategy  = constraintStrategy
AlConfigurable().Constraints                  = constraints
AlConfigurable().UseWeightedAverageConstraint = False
AlConfigurable().MinNumberOfHits              = minNumHits
AlConfigurable().UsePreconditioning           = False
AlConfigurable().SolvTool                     = solver
AlConfigurable().WriteCondToXML               = True
AlConfigurable().WriteCondSubDetList          = [ "IT", "OT" ]
AlConfigurable().Precision                    = 16

## Call after all options are set
AlConfigurable().applyConf()

## Here we configure the track fitting/selection and what else?
importOptions( trackingOpts )

## Now lets run it
from GaudiPython import *
from GaudiPython import gbl

def update(algorithm, appMgr) :
   # get pointer to incident service
   incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
   # incident to trigger update of constants
   updateConstants = gbl.Incident( algorithm, 'UpdateConstants' )
   incSvc.fireIncident( updateConstants )

HistogramPersistencySvc().OutputFile = "./" + output + ".root"

if Misaligned == 'xml':
   DetectorDataSvc().DetDbLocation = "/home/lnicolas/cmtuser/DDDB/lhcb.xml"

## Instantiate application manager
appMgr = AppMgr()

## Print flow of application
AlConfigurable().printFlow( appMgr )

## Get pointer to event selector
evtSel = appMgr.evtSel()
## Set print frequency
evtSel.printfreq = 500
evtSel.FirstEvent = 1

## Open Files; Also initialises Application Manager
evtSel.open( data )

# get pointer to AlignSelTool instance for (fit match) chi2 selection
chi2Filter = appMgr.tool('CleanChi2.Selector')

## Get pointer to ot traj projector
otProjector = appMgr.tool( "ToolSvc.TrajOTProjector" )

chi2CutStrategy         = [100, 40, 20, 10]
fitMatchChi2CutStrategy = [100, 80, 50, 30]

print '==> Iterating ' + str(nIter) + ' times over ' + str(nEvents) + ' events'

for i in range( nIter ) :
    evtSel.rewind()

    if useDrift == True :
       if i < 4 :
          otProjector.UseDrift = False
       elif i >= 4 :
          otProjector.UseDrift = True
       print '==> Use Drift Time: ' + str(otProjector.UseDrift)
    elif useDrift == False:
       print '==> Use Drift Time: ' + str(useDrift)

    if CutStrategy == 'trackChi2':
       if i < 3:
          cut = chi2CutStrategy[i]
       elif i >= 3:
          cut = chi2CutStrategy[3]
       chi2Filter.Chi2PerDoFMaxCut = cut
       print '==> Chi2/dof cut = ' + str(chi2Filter.Chi2PerDoFMaxCut)
    elif CutStrategy == 'fitMatchChi2':
       if i < 3:
          cut = fitMatchChi2CutStrategy[i]
       elif i >= 3:
          cut = fitMatchChi2CutStrategy[3]
       chi2Filter.FitMatchChi2MaxCut = cut
       print '==> Fit match Chi2 cut = ' + str(chi2Filter.FitMatchChi2MaxCut)
    else :
       print '==> No Chi2 cut strategy defined'

    appMgr.run( nEvents )
    update( "Alignment", appMgr )

appMgr.finalize()
