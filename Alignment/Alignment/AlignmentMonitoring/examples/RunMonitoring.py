#!/bin/python

from GaudiPython import gbl
AlMon = gbl.Alignment.AlignmentMonitoring

hfile = '/afs/cern.ch/work/m/mamartin/public/Tests/TrackingPlots_2015_Velov5_Trackerv8-5000ev-histos.root'
aMon = AlMon.AlignmentMonitoring(hfile)
#aMon.SetVerbosity(1)
aMon.Run()

