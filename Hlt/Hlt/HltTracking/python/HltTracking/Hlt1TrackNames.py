HltGlobalEventPrefix			= ""
HltGlobalTrackLocation 			= "Track"

def _baseTrackLocation(prefix,tracks) :
    return prefix + "/" + HltGlobalTrackLocation + "/" + tracks 

Hlt1TracksPrefix 			= HltGlobalEventPrefix + "Hlt1"
Hlt1TrackRoot               = Hlt1TracksPrefix + "/" + HltGlobalTrackLocation + "/"

TrackName = { "Velo"         : "Velo"          # full Velo recoonstruction
              ,"FittedVelo"   : "FittedVelo"
              ,"VeloSelection": "VeloSelection" # filtered velo tracks
              ,"VeloTTHPT"   : "VeloTTHPT"     # VeloTT for high pt and p thresholds (HLT1-like)
              ,"VeloTTComp"  : "VeloTTComp"    # VeloTT complement (HLT2 relaxed pt & p cuts) 
              ,"VeloTT"      : "VeloTT"        # Full VeloTT (HLT2)
              ,"ForwardHPT"  : "ForwardHPT"    # Forward for high pt and p thresholds (HLT1-like)
              ,"ForwardComp" : "ForwardComp"   # Forward complement
              ,"ForwardCompLPT"  : "ForwardCompLPT"    # low PT complement (aka secondLoop)
              ,"Forward"     : "Forward"       # Full Forward
              ,"Seeding"     : "Seeding"       # Seeding
              ,"ForwardSecondLoop" : "ForwardSecondLoop"
              ,"Match"       : "Match"
              ,"Long"        : "Long"
              ,"Downstream"  : "SeedTT"
              ,"MuonTT"      : "MuonTT"		#tag track for tracking efficiency from Muon and TT hits
              ,"VeloMuon"    : "VeloMuon"	#tag track for tracking efficiency from Velo and Muon hits
              ,"FullDownstream"  : "FullDownstream"	#tag track for tracking efficiency, needs full downstream reco.
              ,"FullSeeding"  : "FullSeeding"	#full seeding for tracking efficiency
              }    
Hlt1TrackLoc = { name : _baseTrackLocation(Hlt1TracksPrefix,loc) for name,loc in TrackName.iteritems() }

