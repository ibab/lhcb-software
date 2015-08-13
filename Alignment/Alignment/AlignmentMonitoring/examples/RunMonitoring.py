#!/bin/python

from GaudiPython import gbl
AlMon = gbl.Alignment.AlignmentMonitoring

#hfile = '/afs/cern.ch/work/m/mamartin/public/Tests/TrackingPlots_2015_Velov5_Trackerv8-5000ev-histos.root'
#hfile = 'monitoring/Align-r156916-Online-VELOv2ITv4TTv4OTv4-20000ev-histos.root'
#outputdir = "test/r156916/ITv4TTv4OTv4"
hfile = 'monitoring/Align-r156916-Online-20000ev-histos.root'
outputdir = "test/r156916/Velov5_Trackerv8"
aMon = AlMon.AlignmentMonitoring(hfile,outputdir)
#aMon = AlMon.AlignmentMonitoring(hfile)
aMon.SetVerbosity(1)
aMon.Run()
#raw_input("press Enter to continue")
