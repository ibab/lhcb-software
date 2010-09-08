#!/usr/bin/env python
## 
## Script to run VELO sensor alignment by histograms method
## to run the code
## gaudivelosens.py -n 3 -e 500000 FileOptions.py FileDataFile.py
## where 3 is the number of iteration and 50000 is the number of events
##
## from Gaudi.Configuration import *
from optparse import OptionParser
import GaudiPython
from GaudiPython import gbl

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel.SystemOfUnits import mm, m

from Configurables import IncidentSvc
from Configurables import  AlignSensors
from GaudiPython import  AppMgr

import os
import sys


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

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-n","--numiter",type="int", dest="numiter",help="number of iterations", default=3)
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=1000)
(opts, args) = parser.parse_args()

# Prepare the "configuration script" to parse (like this it is easier than
# having a list with files and python commands, with an if statements that
# decides to do importOptions or exec)
options = [ "importOptions(%r)" % f for f in args ]

    
# "execute" the configuration script generated (if any)
from Gaudi.Configuration import logging
if options:
    g = {}
    l = {}
    exec "from Gaudi.Configuration import *" in g, l
    for o in options:
        logging.debug(o)
        exec o in g, l
    
## Instantiate application manager
from GaudiPython.Bindings import AppMgr
appMgr = AppMgr()
evtSel = appMgr.evtSel()
evtSel.OutputLevel = 1
mainSeq = appMgr.algorithm( "EscherSequencer" )
print evtSel.Input


for i in range( opts.numiter ) :
    print "Iteration nr: ", i
    mainSeq.Enable = False
    evtSel.rewind()
    mainSeq.Enable = True
    # steer the monitor sequence depending on the iteration
    appMgr.algorithm('AlignMonitorSeq').Enable = ( i == 0 )
    appMgr.algorithm('Moni').Enable = ( i == 0 )
    if opts.numiter>1 :
        appMgr.algorithm('AlignPostMonitorSeq').Enable = ( i == opts.numiter-1 )
        
    resetHistos("AlignSensors")
    appMgr.run( opts.numevents )
    fitSensorResiduals("AlignSensors" )
    
#exit the appmgr for finalize
appMgr.exit()

