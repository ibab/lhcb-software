#from Configurables import DaVinci
from Gaudi.Configuration import VERBOSE
def setupTypeTrackSelector(tsname,selector,trackCuts):
         from Configurables import TrackSelector
         selector.addTool(TrackSelector,name=tsname)
         ts = getattr(selector,tsname)
         # Set Cuts
         ts.TrackTypes = [tsname]
         if tsname in trackCuts :
             for name,cut in trackCuts[tsname].iteritems() :
                 if name.find("Min")>-0.5 or name.find("Max")>-0.5 or name.find("AcceptClones")>-0.5:
                     ts.setProp(name,cut)
                 else:
                     ts.setProp("Min"+name,cut[0])
                     ts.setProp("Max"+name,cut[1])
from Configurables import ParticleFlow4Jets,DelegatingTrackSelector

PPF = ParticleFlow4Jets("PF")#,OutputLevel = VERBOSE)
PPF.PFOutputLocation = "Phys/PFParticles/Particles"
PPF.ParticleLocations = [
    "Phys/StdLooseMergedPi0/Particles",
    "Phys/StdLooseResolvedPi0/Particles",
    "Phys/StdLoosePhotons/Particles"  ]
PPF.CatchBremFromElectrons = False
PPF.MaxChi2NoTT = 2.5

TrackCuts = {  "Long"       : {  "MinPtCut" : 0.2 , "AcceptClones" : False,  "MinCloneDistCut" : 5000. }
               ,"Downstream" : {  "MinPtCut" : 0.2 , "Chi2Cut" : [0,3]  } }
PPF.addTool( DelegatingTrackSelector, name="TrackSelector" )
tracktypes = TrackCuts.keys()
PPF.TrackSelector.TrackTypes = ["Long","Downstream"]
for type in tracktypes : setupTypeTrackSelector( type, PPF.TrackSelector,TrackCuts )
ParticleFlow=PPF
#DaVinci().UserAlgorithms +=[ParticleFlow]
