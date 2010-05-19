from GaudiConf.Configuration import *
from AnalysisPython import Dir, Functors
from GaudiPython.Bindings import gbl, AppMgr, Helper

#locationRoot = '/Event/MicroDST/' # MicroDST
#locationRoot = '/Event/CocktailB/'
#locationRoot = '/Event/Sel/' # MicroDST
locationRoot = '/Event/'
#filename = 'test_pions_killer.dst'
filename = 'test_kaons_killer.dst'

locations = ['Phys/StdTightPions',
             'Phys/StdTightKaons']

#rawEventLocation = '/Event/DAQ/RawEvent'
rawEventLocation = '/Event/pRec/Vertex/Primary'

fullLocations = [locationRoot+loc+'/Particles' for loc in locations]


#filename = 'Test.Cocktail.mdst'
#filename = 'TestJpsi.Cocktail.mdst'
#filename = 'TestRawEvent.SeqKaons.dst'
#filename = 'Test.Cocktail.dst'
#filename='TestRawEventJpsi.SeqJpsi.dst'
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
'''
   try :
       rawEvt = evtSvc[rawEventLocation]
       if rawEvt :
           nRawEvt +=1
   except :
       print 'Found no', rawEventLocation
'''
print "Analysed ", nEvents, " events"

for loc in fullLocations :
    print loc, ': ', nParticles[loc], ' Particles in ', nSelEvents[loc],' Selected Events. '
#print rawEventLocation, 'found in', nRawEvt, 'events'
