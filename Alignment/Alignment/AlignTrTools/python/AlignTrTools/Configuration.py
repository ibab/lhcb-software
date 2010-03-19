## @package AlignTrTools
# High level configuration tools for AlignTrTools
# @author Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>
# @date   03/02/2009 


from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables import (LHCbConfigurableUser, LHCbApp, TAConfig )


__version__= "$Id: Configuration.py"
__author__= "Johan Blouw <Johan.Blouw@physi.uni-heidelberg.de>"

class AlignTrTools( LHCbConfigurableUser ):

     __slots__ = {
            "Tools" 		 	: [] # list of various tools 
	  , "Constraints" 	 	: [] # list of constraints
          , "Degrees_of_Freedom" 	: [0, 0, 0, 0, 0, 0] # degrees of freedom to align
          , "nTrackModelParameters" 	: 4 # nof of parameters of track model
          , "initial_residual_cut"  	: 100.0 # 1st outlier rejection
          , "Chi2Scale"           	: -1   # value = -1 : no chi2 cut
          , "minChi2"               	: 10    # value = -1 : no chi2 cut
          , "Outlier"             	: 4     # max outlier value for MP track fit
          , "Sim"                   	: False

          }
     
     
     def __apply_configuration__( self ):
         print "******* calling ", self.name()
	 print "Tools used: ", self.getProp("Tools")
         for tool in self.getProp("Tools"):
             print "Configuring tool ", tool
             if tool == "TAConfig":
 		self.TAConfigure()
 		TAConfig().Constrain_OT = self.getProp("Constraints")
                TAConfig().Degrees_of_Freedom = self.getProp("Degrees_of_Freedom")
		TAConfig().nTrackModelParameters = self.getProp("nTrackModelParameters")
		TAConfig().initial_residual_cut = self.getProp("initial_residual_cut")
		TAConfig().Chi2Scale = self.getProp("Chi2Scale")
		TAConfig().minChi2 = self.getProp("minChi2")
		TAConfig().Outlier = self.getProp("Outlier")
		TAConfig().Sim       = self.getProp("Sim")
                print "TAConfig degrees of freedom = ", TAConfig().getProp("Degrees_of_Freedom")
             if tool  == "ATrackSelector":
		self.TrackSelection()
             if tool == "WriteAlignmentConditions": 
		self.WriteCondConf()

     def TAConfigure( self ):
     	from Configurables import (TAConfig, Centipede, Derivatives)

	TAConfig().addTool(Centipede)
	TAConfig().MillepedeTool 	= "Centipede"
	TAConfig().addTool(Derivatives)
	TAConfig().DerivativeTool       = "Derivatives";

	TAConfig().OutputLevel 		= 3;
	TAConfig().Velo_left_modules  	= [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
	TAConfig().Velo_right_modules 	= [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]

	TAConfig().PenaltyTerms       	= [100000.0,300000.0,300000.0,300000.0,30000.0,30000.0]
	TAConfig().Degrees_of_Freedom 	= [1,0,0,0,0,0];
	TAConfig().nTrackModelParameters = 4; #4;
	TAConfig().Constrain_OT 	= []
	TAConfig().Chi_sq_factor        = 10.0;   #startfctr in Centipede/Millepede-->x*3sigma of chi2/ndf function
	TAConfig().initial_residual_cut = 100.0;  #1st outlier rejection
	TAConfig().n_l_stdev            = 3.0;
	TAConfig().Chi2Scale          = -1;    # value = -1 : no chi2 cut
	TAConfig().Outlier            = 10; #4;
	TAConfig().residual_cut         = 116.0; #not used...
	TAConfig().OT_system   		= False;
	TAConfig().OT_layer    		= False;
	TAConfig().OT_halflayer		= True;
	TAConfig().OT_quadrant 		= False;
	TAConfig().OT_module   		= False;

	TAConfig().IT_system 		= False;
	TAConfig().IT_layer 		= False;
	TAConfig().IT_box 		= False;
	TAConfig().IT_ladder 		= False;

	TAConfig().TT_system 		= False;
	TAConfig().TT_station 		= False;
	TAConfig().TT_layer 		= False;



     def TrackSelection( self ):
	from Configurables import ATrackSelector
	ATrackSelector().MinPCut = 0.0
	ATrackSelector().MinPtCut = 0.0
	ATrackSelector().MinChi2Cut = 0.0
	ATrackSelector().MaxChi2Cut = 15.0
	ATrackSelector().Charge = 0
	ATrackSelector().MaxPCut = -1
	ATrackSelector().MaxPtCut = -1
	ATrackSelector().MinITHitCut    = 0
	ATrackSelector().MinOTHitCut = 15
	ATrackSelector().MinTTHitCut = 0
	ATrackSelector().MinEnergyCut = 0.0
	ATrackSelector().OutputLevel = 1
	ATrackSelector().YCutMax_at_T1 = -3000.0
	ATrackSelector().YCutMin_at_T1 = 3000.0


     def WriteCondConf( self ):
	from Configurables import WriteAlignmentConditions
	WriteAlignmentConditions().topElement = ""
	WriteAlignmentConditions().footer = "</DDDB>"
	WriteAlignmentConditions().startTag = "<condition"
	WriteAlignmentConditions().outputFile = "alignment.xml"
	WriteAlignmentConditions().depths = []
	WriteAlignmentConditions().precision = 8



                
