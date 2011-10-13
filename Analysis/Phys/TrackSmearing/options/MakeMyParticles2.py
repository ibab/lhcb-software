from Configurables import ChargedProtoParticleMaker, CombinedParticleMaker, NoPIDsParticleMaker, BestPIDParticleMaker, DataOnDemandSvc, UnpackTrack, DelegatingTrackSelector, TrackSelector, GaudiSequencer, TrackToDST, ChargedPP2MC, TrackAssociator, EventNodeKiller, DaVinci, TrackSmeared
from Configurables import ChargedProtoParticleAddMuonInfo, MuonIDAlg,ChargedProtoParticleAddRichInfo
from MuonID import ConfiguredMuonIDs
from Configurables import ProtoParticleMUONFilter, ChargedProtoCombineDLLsAlg,ProtoParticleCALOFilter


# kill all links
initseq=GaudiSequencer("AnalysisInitSeq")
evtnodekiller = EventNodeKiller("KillLinks")
evtnodekiller.Nodes = ["Link/Rec"]
initseq.Members += [ evtnodekiller ];
#TrackSmeared().OutputLevel = 0
#ChargedProtoParticleMaker().OutputLevel = 0
#NoPIDsParticleMaker().OutputLevel = 0

#MyOptions = [ "MC" ]
print "OPTIONS OF MAKEMYPARTICLES"
#print MyOptions

def makeMyProtoP(trackcont):
	unpacker = UnpackTrack(trackcont+"UnpackTrack")
	unpacker.InputName="pRec/Track/"+trackcont
	unpacker.OutputName="Rec/Track/"+trackcont
	refitter = TrackSmeared(trackcont+"TrackSmeared")
        refitter.InputLocation = "Rec/Track/"+trackcont
        refitter.OutputLocation = "Refit"
        refitter.smear = 1
        #refitter.makePlots = 1
        #refitter.OutputLevel = 2        
        #idalg = MuonIDAlg("BestIDalg")
	#idalg.OutputLevel = 0
        #cm=ConfiguredMuonIDs.ConfiguredMuonIDs(data=DaVinci().getProp("DataType"))
        #cm.configureMuonIDAlg(idalg)
        #idalg.TrackLocation = "Rec/Track/Refit"
        #idalg.MuonIDLocation = "Rec/Muon/MuonPID/Refit"
	downprotoseq = GaudiSequencer(trackcont+"ProtoPSeq")
	downprotos = ChargedProtoParticleMaker(trackcont+"ProtoPMaker")
	downprotos.InputTrackLocation = ["Rec/Track/Refit"]
	downprotos.OutputProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
	downprotos.addTool( DelegatingTrackSelector, name="TrackSelector" )
	#downprotos.OutputLevel = 0
	tracktypes = [ "Long","Upstream","Downstream","Ttrack","Velo","VeloR" ]
	downprotos.TrackSelector.TrackTypes = tracktypes
	selector = downprotos.TrackSelector
	for tsname in tracktypes:
		selector.addTool(TrackSelector,name=tsname)
		ts = getattr(selector,tsname)
		# Set Cuts
		ts.TrackTypes = [tsname]
	#set up associators
	#addmuonpid = ChargedProtoParticleAddMuonInfo("Bestaddmuoninfo")
        #addmuonpid.InputMuonPIDLocation = "Rec/Muon/MuonPID/Refit"
        #addmuonpid.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
	#addmuonpid.OutputLevel = 0
        addrichpid = ChargedProtoParticleAddRichInfo("Bestaddrichinfo")
        addrichpid.InputRichPIDLocation = "Rec/Rich/Refit"
        #addrichpid.InputRichPIDLocation = "Rec/Rich/"+trackcont 
        addrichpid.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #addcalopid = ChargedProtoParticleAddCaloInfo("Bestaddcaloinfo")
        #addcalopid.InputRichCALOLocation = "Rec/Rich/Refit"
        #addcalopid.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        combinedll = ChargedProtoCombineDLLsAlg("BestCombineDLL")
        combinedll.ProtoParticleLocation ="Rec/ProtoP/"+trackcont+"ProtoPMaker"
	assoctr = TrackAssociator(trackcont+"AssocTr")
	#assoctr.TracksInContainer = "Rec/Track/"+trackcont
        assoctr.TracksInContainer = "Rec/Track/Refit"      
	assocpp=ChargedPP2MC(trackcont+"AssocPP")
	#assocpp.TrackLocations = [ "Rec/Track/"+trackcont ]
        assocpp.TrackLocations = [ "Rec/Track/Refit" ]
	assocpp.InputData = [ "Rec/ProtoP/"+trackcont+"ProtoPMaker" ]
        #assocpp.InputData = [ "Rec/ProtoP/Refit" ]
	assocpp.OutputTable = "Relations/Rec/ProtoP/"+trackcont+"ProtoPMaker"
	# DST post treatment        
        TrackToDST(trackcont+"TrackToDST").TracksInContainer = "Rec/Track/"+trackcont
        #downprotoseq.Members += [ TrackToDST(trackcont+"TrackToDST"), assoctr, downprotos, assocpp,addrichpid,combinedll ]
        
        if not DaVinci().Simulation:
                downprotoseq.Members += [ TrackToDST(trackcont+"TrackToDST"), downprotos, addrichpid,combinedll ]
                DataOnDemandSvc().AlgMap.update( {      
                        "/Event/Rec/Track/"+trackcont : unpacker.getFullName(),
                        "/Event/Rec/Track/Refit" : refitter.getFullName(),
                        "/Event/Rec/Rich/Refit": downprotoseq.getFullName(),
                        "/Event/Rec/ProtoP/"+trackcont+"ProtoPMaker" : downprotoseq.getFullName(),
                        "/Event/Relations/Rec/ProtoP/"+trackcont+"ProtoPMaker" : downprotoseq.getFullName()
                        } )
        else:
                if ( DaVinci().Simulation ) :
                        downprotoseq.Members += [ TrackToDST(trackcont+"TrackToDST"), assoctr, downprotos, addrichpid,combinedll,assocpp ]
                        DataOnDemandSvc().AlgMap.update( {      
                                "/Event/Rec/Track/"+trackcont : unpacker.getFullName(),
                                "/Event/Rec/Track/Refit" : refitter.getFullName(),
                                "/Event/Rec/Rich/Refit": downprotoseq.getFullName(),
                                "/Event/Rec/ProtoP/"+trackcont+"ProtoPMaker" : downprotoseq.getFullName(),
                                "/Event/Relations/Rec/ProtoP/"+trackcont+"ProtoPMaker" : downprotoseq.getFullName()
                                } )
        
from CommonParticles.Utils import *

def makeMyParticles(name, trackcont, particle):
        # set up particle maker
        #particleMaker =  NoPIDsParticleMaker(name , Particle = particle)
        particleMaker =  CombinedParticleMaker(name , Particle = particle)
#       particleMaker.OutputLevel = 0
        particleMaker.Input = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        selector = trackSelector ( particleMaker )
        # protoparticle filter:
        fltr = protoFilter ( particleMaker , ProtoParticleCALOFilter, 'Pion' )
        fltr.Selection = [ "" ]     
        DataOnDemandSvc().AlgMap.update( {
                "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
                "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() } )

def makeMyKaons(name, trackcont, particle):
        # set up particle maker
        #particleMaker =  NoPIDsParticleMaker(name , Particle = particle)
        particleMaker =  CombinedParticleMaker(name , Particle = particle)
        #particleMaker =  BestPIDParticleMaker(name , Particle = particle)      
        #particleMaker.addTool(ProtoParticleCALOFilter(Selection = ["RequiresDet='RICH' CombDLL(k-pi)>'-5.0'"],name="muon"))
        #particleMaker.Particles = [ "kaon" ]
        particleMaker.Input = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #particleMaker.OutputLevel = 0     
        selector = trackSelector ( particleMaker )
        # protoparticle filter:
        fltr =protoFilter ( particleMaker , ProtoParticleCALOFilter, 'Kaon' )
        fltr.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ] 
        #fltr = protoFilter ( particleMaker , ProtoParticleCALOFilter, 'Kaon' )
        #fltr.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ]     
        DataOnDemandSvc().AlgMap.update( {
                "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
                "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() } )

def makeAllParticles(name,trackcont,particle):
        particleMaker =  NoPIDsParticleMaker(name , Particle = particle)
        particleMaker.Input = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        DataOnDemandSvc().AlgMap.update( {
                "/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(),
                "/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() } )

#def makeMyParticles(name, trackcont, particle):
#	# set up particle maker
#	particleMaker =  BestPIDParticleMaker(name , Particle = particle)
#	particleMaker.addTool(ProtoParticleMUONFilter(Selection = ["RequiresDet='MUON' IsMuon=True"]),name="muon")
#        particleMaker.Particles = [ "muon" ]
#	particleMaker.Input = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
	#particleMaker.OutputLevel = 0
#	DataOnDemandSvc().AlgMap.update( {
#		"/Event/Phys/" + particleMaker.name() + '/Particles' : particleMaker.getFullName(), 
#		"/Event/Phys/" + particleMaker.name() + '/Vertices'  : particleMaker.getFullName() } )

# define protoparticle containers and register them with DataOnDemandSvc
#makeMyProtoP("Best")
#makeMyProtoP("Downstream")
#makeMyProtoP("Forward")
#makeMyProtoP("Match")
#makeMyProtoP("VeloTT")
#makeMyProtoP("Seed")
#makeMyProtoP("Velo")

# define particlemakers and register them with DataOnDemandSvc
#makeMyKaons("MyLooseKaons", "Best", "kaon")
#makeMyParticles("MyLoosePions", "Best", "pion")
#makeAllParticles("MyBestPions", "Best", "pion")
#makeMyParticles("MyNoPIDBestProtons", "Best", "proton")
#makeMyParticles("MyNoPIDDownstreamPions", "Downstream", "pion")
#makeMyParticles("MyNoPIDDownstreamProtons", "Downstream", "proton")

#makeMyParticles("MyNoPIDForwardPions", "Forward", "pion")
#makeMyParticles("MyNoPIDMatchPions", "Match", "pion")
#makeMyParticles("MyNoPIDForwardProtons", "Forward", "proton")
#makeMyParticles("MyNoPIDMatchProtons", "Match", "proton")

#makeMyParticles("MyNoPIDUpstreamPions", "VeloTT", "pion")
#makeMyParticles("MyNoPIDTtrackPions", "Seed", "pion")
#makeMyParticles("MyNoPIDVeloPions", "Velo", "pion")
