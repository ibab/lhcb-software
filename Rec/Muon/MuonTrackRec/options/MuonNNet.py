# MuonNNetRec tool default specific options
from Configurables import MuonNNetRec

muonNNet = MuonNNetRec(OutputLevel = INFO)

// choice of basic muon tools:
muonNNet.DecodingTool = "MuonHitDecode" # interface to MuonRawBuffer
                                        # alternative is "MuonMonHitDecode" from Muon/MuonMonKernel
muonNNet.PadRecTool   = "MuonPadRec";
muonNNet.PosTool      = "MuonDetPosTool" # use the default tool for using standard MuonDet methods
                                         # alternative is  much faster "MuonPosTool" from Hlt/HltMuon or Muon/MuonMonKernel  
muonNNet.ClusterTool  = "MuonFakeClustering" # default is no clustering
                                         # alternative is "MuonClusterRec"

muonNNet.PosScaleFactor  =  10.0 # head-tail weight scale factor                  
muonNNet.NegScaleFactorB =   1.0 # head-head weight scale factor                  
muonNNet.NegScaleFactorC =   1.0 # tail-tail weight scale factor                  
muonNNet.SlamFactorB     =   5.0 # penalty for TT connections                     
muonNNet.SlamFactorC     =   5.0 # penalty for HH connections                     
muonNNet.ExponentXZ      =  10.0 # exponent for (1-sin(thxz)) weight factor       
muonNNet.ExponentYZ      =  10.0 # exponent for (1-sin(thyz)) weight factor       
muonNNet.Stimulus        =   0.0 # stimulation weight term                        
muonNNet.Temperature     =   1.0 # temperature                                    
muonNNet.Convergence     =1.0e-5 # convergence factor                             
muonNNet.NeuronThreshold =   0.7 # neuron activation cut                          
muonNNet.DoubleKillAngCut=   0.1 # angular cut for double length neurons killing  

muonNNet.TrackSpanCut    =   2.0 # cut on span for selected tracks                
muonNNet.StationFiringCut=   2.0 # cut on  # of stations firing in the track (strict >)
muonNNet.MaxNeurons      =  400 # (def. 3000) max number of possible track segments      
muonNNet.MaxIterations   =   30 # (def. 100) max number of NN iterations
muonNNet.SkipStation     =   -1 # for efficiency studies (-1 for none)
muonNNet.AllowHoles      =  True # allow holes in tracks


# this is to decide how to deal with TOF corrections
# mandatory for (runs 33878-33883, runs 34063- 34120). 
muonNNet.PhysicsTiming = True; # should be False only for runs < 33000
muonNNet.AssumeCosmics = True; # if True, track direction inferred from y slope and not from speed fit
muonNNet.AssumePhysics = False # if True assume that tracks come from IP (only forward)


muonNNet.AddXTalk      = False # add Xtalk hits to tracks
muonNNet.XtalkRadius   = 1.5;  # radius of cone around tracks (in pad size units) where hits will be added
muonNNet.TracksOutputLocation = "Rec/Track/MuonStandalone"
 
