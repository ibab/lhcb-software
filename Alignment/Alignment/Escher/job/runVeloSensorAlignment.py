import gaudimodule
from gaudimodule import gbl

import os
import sys

ESCHEROPTS = os.environ[ 'ESCHEROPTS' ]

OPTIONS = ESCHEROPTS + '/v200601.opts'

appMgr = gaudimodule.AppMgr( outputlevel = 3, joboptions = OPTIONS )

EVT = appMgr.evtSvc()

SEL=appMgr.evtSel()

def fitSensorResiduals( algorithm ) :
    # get pointer to incident service
    incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
    # incident to trigger fit of residual distributions
    fitIt = gbl.Incident( algorithm, 'FitSensorResiduals' )
    incSvc.fireIncident( fitIt )

def resetHistos( algorithm ) :
    # get pointer to incident service
    incSvc = appMgr.service( 'IncidentSvc', 'IIncidentSvc' )
    # incident to trigger fit of residual distributions
    resetIt = gbl.Incident( algorithm, 'ResetHistos' )
    incSvc.fireIncident( resetIt )

alg = appMgr.algorithm('AlignSensors')
alg.KalmanResidual = False
alg.OutputLevel = 3
SEL.PrintFreq = 100

for i in range( 5 ): 
        resetHistos( alg.name() )
	appMgr.run( 20000 )
        alg.OutputLevel = 2
        fitSensorResiduals( alg.name() )
        alg.OutputLevel = 3
	SEL.rewind()
