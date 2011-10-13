from Configurables import ChargedProtoParticleMaker, CombinedParticleMaker, NoPIDsParticleMaker, BestPIDParticleMaker, DataOnDemandSvc, UnpackTrack, DelegatingTrackSelector, TrackSelector, GaudiSequencer, TrackToDST, ChargedPP2MC, TrackAssociator, EventNodeKiller, DaVinci, TrackSmeared
from Configurables import MuonIDAlg
from MuonID import ConfiguredMuonIDs
from Configurables import ProtoParticleMUONFilter,ProtoParticleCALOFilter

###########
##This file is useless when using smearBest
###########



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
	smeartracks = TrackSmeared(trackcont+"TrackSmeared")
        smeartracks.InputLocation = "Rec/Track/"+trackcont
        outputLocation = "Smeared"
        smeartracks.OutputLocation = outputLocation
        smeartracks.smearCopied = True
        smeartracks.smear = 1
        #smeartracks.makePlots = 1
        #smeartracks.OutputLevel = 2        
        protoseq = GaudiSequencer(trackcont+"ProtoPSeq")
	protos = ChargedProtoParticleMaker(trackcont+"ProtoPMaker")
	protos.InputTrackLocation = ["Rec/Track/"+outputLocation]
	protos.OutputProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
	protos.addTool( DelegatingTrackSelector, name="TrackSelector" )
	#protos.OutputLevel = 0
        tracktypes = [ "Long","Upstream","Downstream","Ttrack","Velo","VeloR" ]
	protos.TrackSelector.TrackTypes = tracktypes
	selector = protos.TrackSelector
	for tsname in tracktypes:
		selector.addTool(TrackSelector,name=tsname)
		ts = getattr(selector,tsname)
		# Set Cuts
		ts.TrackTypes = [tsname]

        # Add PID information	
        idalg = MuonIDAlg("BestIDalg")
	#idalg.OutputLevel = 5
        cm=ConfiguredMuonIDs.ConfiguredMuonIDs(data=DaVinci().getProp("DataType"))
        cm.configureMuonIDAlg(idalg)
        idalg.TrackLocation = "Rec/Track/"+outputLocation
        idalg.MuonIDLocation = "Rec/Muon/MuonPID/"+outputLocation



        from Configurables import ( ChargedProtoParticleAddRichInfo,
                                    ChargedProtoParticleAddMuonInfo,
                                    ChargedProtoParticleAddEcalInfo,
                                    ChargedProtoParticleAddBremInfo,
                                    ChargedProtoParticleAddHcalInfo,
                                    ChargedProtoParticleAddPrsInfo,
                                    ChargedProtoParticleAddSpdInfo,
                                    ChargedProtoParticleAddVeloInfo,
                                    ChargedProtoCombineDLLsAlg
                                    )
	addmuonpid = ChargedProtoParticleAddMuonInfo("Bestaddmuoninfo")
        addmuonpid.InputMuonPIDLocation = "Rec/Muon/MuonPID/"+outputLocation
        addmuonpid.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        addrichpid = ChargedProtoParticleAddRichInfo("Bestaddrichinfo")
        addrichpid.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        # The rich pid is added to the track in TrackSmeared, thus only kaon pion pid is consistent
        # the others are not implemented. The modes smearBest and smearProto don't have the problems
        #addcalopid = ChargedProtoParticleAddCaloInfo("Bestaddcaloinfo")
        #addcalopid.InputRichCALOLocation = "Rec/Rich/"+outputLocation
        #addcalopid.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #ecal = ChargedProtoParticleAddEcalInfo("ChargedProtoPAddEcal")
        #ecal.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #brem = ChargedProtoParticleAddBremInfo("ChargedProtoPAddBrem")
        #brem.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #hcal = ChargedProtoParticleAddHcalInfo("ChargedProtoPAddHcal")
        #hcal.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #prs  = ChargedProtoParticleAddPrsInfo("ChargedProtoPAddPrs")
        #prs.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #spd  = ChargedProtoParticleAddSpdInfo("ChargedProtoPAddSpd")
        #spd.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        #velo = ChargedProtoParticleAddVeloInfo("ChargedProtoPAddVeloDEDX")
        #velo.ProtoParticleLocation = "Rec/ProtoP/"+trackcont+"ProtoPMaker"
        combinedll = ChargedProtoCombineDLLsAlg("BestCombineDLL")
        combinedll.ProtoParticleLocation ="Rec/ProtoP/"+trackcont+"ProtoPMaker"     

        #set up associators
	assoctr = TrackAssociator(trackcont+"AssocTr")
        assoctr.TracksInContainer = "Rec/Track/"+outputLocation     
	assocpp=ChargedPP2MC(trackcont+"AssocPP")
        assocpp.TrackLocations = [ "Rec/Track/"+outputLocation ]
	assocpp.InputData = [ "Rec/ProtoP/"+trackcont+"ProtoPMaker" ]
       	assocpp.OutputTable = "Relations/Rec/ProtoP/"+trackcont+"ProtoPMaker"
	# DST post treatment        
        TrackToDST(trackcont+"TrackToDST").TracksInContainer = "Rec/Track/"+trackcont
        if not DaVinci().Simulation:
                protoseq.Members += [ TrackToDST(trackcont+"TrackToDST"), protos]
                protoseq.Members += [ ecal,brem,hcal,prs,spd,velo,addrichpid,addmuonpid,combinedll ]
                DataOnDemandSvc().AlgMap.update( {      
                        "/Event/Rec/Track/"+trackcont : unpacker.getFullName(),
                        "/Event/Rec/Track/"+outputLocation : smeartracks.getFullName(),
                        "/Event/Rec/Rich/"+outputLocation: protoseq.getFullName(),
                        "/Event/Rec/Muon/MuonPID/"+outputLocation : idalg.getFullName(),
                        "/Event/Rec/ProtoP/"+trackcont+"ProtoPMaker" : protoseq.getFullName(),
                        "/Event/Relations/Rec/ProtoP/"+trackcont+"ProtoPMaker" : protoseq.getFullName()
                        } )
        else:
                if ( DaVinci().Simulation ) :
                        protoseq.Members += [ TrackToDST(trackcont+"TrackToDST"), assoctr, protos]
                        #protoseq.Members += [ ecal,brem,hcal,prs,spd,velo,addrichpid,addmuonpid,combinedll,assocpp ]
                        protoseq.Members += [ idalg,addmuonpid,addrichpid,combinedll,assocpp ]
                        DataOnDemandSvc().AlgMap.update( {      
                                "/Event/Rec/Track/"+trackcont : unpacker.getFullName(),
                                "/Event/Rec/Track/"+outputLocation : smeartracks.getFullName(),
                                "/Event/Rec/Rich/"+outputLocation: protoseq.getFullName(),
                                "/Event/Rec/Muon/MuonPID/"+outputLocation : idalg.getFullName(),
                                "/Event/Rec/ProtoP/"+trackcont+"ProtoPMaker" : protoseq.getFullName(),
                                "/Event/Relations/Rec/ProtoP/"+trackcont+"ProtoPMaker" : protoseq.getFullName()
                                } )
        
from CommonParticles.Utils import *


#Here the particles are defined. For examples look also in $COMMONPARTICLESROOT
#The important thing is the input location.
#Can also be used with the option smearProto
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
makeMyProtoP("Best")
#makeMyProtoP("Downstream")
#makeMyProtoP("Forward")
#makeMyProtoP("Match")
#makeMyProtoP("VeloTT")
#makeMyProtoP("Seed")
#makeMyProtoP("Velo")

# define particlemakers and register them with DataOnDemandSvc
makeMyKaons("SmrdLooseKaons", "Best", "kaon")
makeMyParticles("SmrdLoosePions", "Best", "pion")
makeAllParticles("SmrdBestPions", "Best", "pion")
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
