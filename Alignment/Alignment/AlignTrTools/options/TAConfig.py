from Configurables import AlignTrTools
from AlignmentTools.AlignmentToolsConf import Centipede
from AlignTrTools.AlignTrToolsConf import ( TAConfig, Derivatives )
import AlignTrTools.Configuration

########################################################
TAConfig().addTool(Centipede)
TAConfig().MillepedeTool = "Centipede"

TAConfig().properties()


####### below from MD ############

TAConfig().OutputLevel = 3;
TAConfig().Velo_left_modules  = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
TAConfig().Velo_right_modules = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]

TAConfig().PenaltyTerms       = [100000.0,300000.0,300000.0,300000.0,30000.0,30000.0]
TAConfig().Degrees_of_Freedom = [1,0,0,0,0,0];
TAConfig().nTrackModelParameters = 4; #4;
#//TAConfig().Fix_alignable_objects = true;
TAConfig().Constrain_OT = [
    
    #"X_S1_L1_Q0_M0",
    #"X_S1_L2_Q0_M0",
    #"X_S3_L1_Q0_M0",
    #"X_S3_L2_Q0_M0",
        
    #"Z_S1_L1_Q0_M0",
    #"Z_S1_L2_Q0_M0",
    #"Z_S3_L1_Q0_M0",
    #"Z_S3_L2_Q0_M0"

    
    #"A_S1_L1_Q0_M0",
    #"A_S1_L2_Q0_M0",
    #"A_S3_L1_Q0_M0",
    #"A_S3_L2_Q0_M0",

    #"B_S1_L1_Q0_M0",
    #"B_S1_L2_Q0_M0",
    #"B_S3_L1_Q0_M0",
    #"B_S3_L2_Q0_M0",
    
    #"C_S1_L1_Q0_M0",
    #"C_S1_L2_Q0_M0",
    #"C_S3_L1_Q0_M0",
    #"C_S3_L2_Q0_M0"
    
    ]

#TAConfig().MillepedeTool        = "Centipede";
TAConfig().Chi_sq_factor        = 10.0;   #startfctr in Centipede/Millepede-->x*3sigma of chi2/ndf function
TAConfig().initial_residual_cut = 100.0;  #1st outlier rejection
TAConfig().n_l_stdev            = 3.0; 
TAConfig().myChi2Scale          = -1;    # value = -1 : no chi2 cut
#TAConfig().minChi2              = 10;    # value = -1 : no chi2 cut
TAConfig().myOutlier            = 10; #4;
#TAConfig().ConstrainMovement    = ["X=0.06"];    

TAConfig().residual_cut         = 116.0; #not used...
TAConfig().DerivativeTool       = "Derivatives";

#TAConfig().CutonTrackSlope      = 1; # 0 - no slope cut, +-1 - cut up/downstream tracks

#TAConfig().Centipede.OutputLevel = 3;
#Centipede.Iteration = false;// inherited from Viret's Millepede, not used by OT
#Centipede.ModuleFixed = -2; // inherited from Viret's Millepede, not used by OT

# Outer Tracker
TAConfig().OT_system   = False;
TAConfig().OT_layer    = False;
TAConfig().OT_halflayer= True;
TAConfig().OT_quadrant = False;
TAConfig().OT_module   = False;
# Inner Tracker
TAConfig().IT_system = False;
TAConfig().IT_layer = False;
TAConfig().IT_box = False;
TAConfig().IT_ladder = False;
# TT
TAConfig().TT_system = False;
TAConfig().TT_station = False;
TAConfig().TT_layer = False;
