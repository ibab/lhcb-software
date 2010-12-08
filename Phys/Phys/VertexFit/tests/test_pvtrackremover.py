
from Configurables import LHCbApp

from AnalysisPython.Functors import NextEvent
from GaudiPython.Bindings   import AppMgr, gbl

LHCb = gbl.LHCb
std = gbl.std

LHCbApp()

appMgr = AppMgr(outpurlevel=3)
appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])

#appMgr.ExtSvc += ['LHCb::ParticlePropertySvc']
appMgr.HistogramPersistency = "ROOT"

appMgr.initialize()

evtSvc = appMgr.evtSvc()
toolSvc = appMgr.toolsvc()
evtSel = appMgr.evtSel()
nextEvent = NextEvent(appMgr, EvtMax=10)

# get an instance of IPVReFitter
pvReFitter = toolSvc.create('PVTrackRemover',
                            interface='IPVReFitter')


while ( nextEvent() ) :
    nEvents += 1
    PVs = evtSvc['Rec/Vertex/Primary']
    dummyParticle = LHCb.Particle()
    for pv in PVs :
        pvReFitter.removeTracks(dummyParticle, pv)
