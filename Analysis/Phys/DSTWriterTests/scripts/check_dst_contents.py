#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --input <some file name> --location <TES root of selected particles>
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

from GaudiConf.Configuration import *
from AnalysisPython import Dir, Functors
from GaudiPython.Bindings import gbl, AppMgr, Helper

import sys, getopt

filename = ''
locationRoot = ''
opts, args = getopt.getopt(sys.argv[1:], "l:i", ["input=", "location="])

for o, a in opts:
    if o in ("-i", "--input"):
        filename = a
    elif o in ('-l', '--location') :
        locationRoot = a

assert(filename != '')


locationRoot = '/Event/' + locationRoot + '/'

locations = ['Phys/StdTightPions',
             'Phys/StdTightKaons',
             'Phys/StdTightMuons',
             'Phys/StdLooseJpsi2MuMu']

rawEventLocation = '/Event/DAQ/RawEvent'

fullLocations = [locationRoot+loc+'/Particles' for loc in locations]

lhcbApp = LHCbApp(DDDBtag = 'default',
                  CondDBtag = 'default',
                  DataType = '2010',
                  Simulation = True)

appMgr = AppMgr(outputlevel=4)
appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
appMgr.initialize()

import atexit
atexit.register(appMgr.exit)

evtSvc = appMgr.evtSvc()

evtSel = appMgr.evtSel()

nextEvent = Functors.NextEvent(appMgr)

evtSel.open(filename)

nEvents=0
nSelEvents = {}
nParticles = {}
nRawEvt=0
for loc in fullLocations :
    nParticles[loc] = 0
    nSelEvents[loc] = 0
    
while ( nextEvent() ) :
   nEvents+=1
   for loc in fullLocations :
       particles = evtSvc[loc]
       if particles :
           nSelEvents[loc] += 1
           nParticles[loc] += particles.size()
           
   try :
       rawEvt = evtSvc[rawEventLocation]
       if rawEvt :
           nRawEvt +=1
   except :
       print 'Found no', rawEventLocation

print '----------------------------------------------------------------------------------'
print "Analysed ", nEvents, " events"

for loc in fullLocations :
    if nParticles[loc] > 0 :
        print loc, ': ', nParticles[loc], ' Particles in ', nSelEvents[loc],' Selected Events. '
print rawEventLocation, 'found in', nRawEvt, 'events'
print '----------------------------------------------------------------------------------'

