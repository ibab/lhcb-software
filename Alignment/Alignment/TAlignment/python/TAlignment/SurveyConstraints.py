from Gaudi.Configuration  import *
from Configurables import LHCbConfigurableUser
#from Configurables import Al__AlignChisqConstraintTool as AlignChisqConstraintTool

class SurveyConstraints( LHCbConfigurableUser ):
    
    __slots__ = {
        "Constraints"        : [] # List of survey constraints for particular alignment element
        , "XmlFiles"         : [] # List of xml files with alignment constants from survey
        , "XmlUncertainties" : [] # List of uncertainties for survey from xml
        , "DefaultSurveyDir" : "" # Directory from where to take survey constants
        }
    
    def defaultSurveyDir( self ) :
        rc = self.getProp("DefaultSurveyDir")
        if len(rc) is 0:
            import os
            rc = os.getenv("TALIGNMENTROOT") + "/surveyxml/"
        return rc

    def Velo( self ) :
        self.XmlFiles  += [ self.defaultSurveyDir() + "Velo/Modules.xml",
                            self.defaultSurveyDir() + "Velo/Detectors.xml" ]
        self.XmlUncertainties += ["Module(PU|).. : 0.02 0.02 0.02 0.0002 0.0002 0.0002",
                                  "Detector(PU|)..-.. : 0.005 0.005 0.005 0.0001 0.0001 0.0001" ]
        self.Constraints += [ "Velo      : 0 0 0 -0.0001 0 -0.0001 : 0.2 0.2 0.2 0.0001 0.0001 0.001",
                              "Velo/Velo(Right|Left) : 0 0 0 0 0 0 : 10 1 0.2 0.0001 0.0001 0.0001" ]
        
    def OT( self ) :
        # we don't load OT xml yet. there is no usefull info, since it doesn't have the right structures in geometry.
                
        # modules
        self.Constraints += [ "OT/.*?M. : 0 0 0 0 0 0 : 0.05 0.05 0.05 0.00001 0.001 0.00001" ]
        # OT global
        self.Constraints += [ "OTSystem : 0 0 0 0 0 0 : 1 1 1 0.001 0.001 0.001" ]

        # C frames
        #self.append("OT/T1X1UASide : -1.43  0.05  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T1X1UCSide : -0.79 -0.05  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T1VX2ASide : -0.47 -0.65 -0.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T1VX2CSide : -0.35 -1.50 -0.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T2X1UASide : -0.27  0.05 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T2X1UCSide : -0.98 -0.70 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T2VX2ASide : -1.69 -0.85 -3.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T2VX2CSide :  0.04 -1.70 -3.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T3X1UASide : -1.52 -1.25  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T3X1UCSide : -0.04 -1.00  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T3VX2ASide : -1.49 -1.30 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T3VX2CSide :  0.20 -1.70 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")

        # this is for the average of 2 C-frames.
        # corrected  for the bowing of the bridge (2010/01/26)
        #self.append("OT/T1X1U : -1.11 -0.7  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T1VX2 : -0.41 -2.2 -0.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T2X1U : -0.63 -1.2 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T2VX2 : -0.83 -2.2 -3.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T3X1U : -0.78 -2.5  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        #self.append("OT/T3VX2 : -0.65 -2.9 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")

        self.XmlFiles += [ self.defaultSurveyDir() + "OT/Elements_OTSurvey2011StationYCorrected.xml" ]
        self.XmlUncertainties += [ "OT/T.(X1U|VX2) : 0.5 0.5 0.5 0.0001 0.0001 0.0001",
                                   "OT/T.(X1U|VX2).Side : 0.5 0.5 0.5 0.0001 0.0001 0.0001",
                                   "OT : 0.5 0.5 0.5 0.001 0.001 0.001",
                                   "OT/T. : 0.5 0.0001 0.5 0.0001 0.0001 0.0001"]
        
    def TT( self ) :
        self.XmlFiles += [ self.defaultSurveyDir() + "TT/Detectors_TTSurvey2011SystemYCorrected.xml",
                           self.defaultSurveyDir() + "TT/Modules.xml",
                           self.defaultSurveyDir() + "TT/TTSensorSurvey2011.xml" ]
        self.XmlUncertainties += [ "TTSystem             : 0.5 0.5 0.5 0.001 0.001 0.001",
                                   "TT.                  : 0.1 0.1 0.1 0.0005 0.0005 0.0005",
                                   "TT..Layer            : 0.1 0.1 0.1 0.0005 0.0005 0.0005",
                                   "TT..Layer.Side       : 0.1 0.1 0.1 0.0005 0.0005 0.0005",
                                   "TT..LayerR.Module.*? : 0.1 0.1 0.1 0.0005 0.0005 0.0005"]
        self.XmlUncertainties += [ "TT.*?(Low|High)Z     : 0.1 0.1 1.0 0.0005 0.0005 0.0005"]
        
        # boxes
        self.Constraints += [ "TTASide : 0 0 0 0 0 0 : 0.2 0.2 0.2 0.001 0.001 0.001",
                              "TTCSide : 0 0 0 0 0 0 : 0.2 0.2 0.2 0.001 0.001 0.001" ]
 
    def IT( self ) :
        #        self.XmlFiles += [ self.defaultSurveyDir() + "IT/Detectors_ITSurvey2011YBoxBilayerCorrected.xml" ]
        self.XmlFiles += [ self.defaultSurveyDir() + "IT/Detectors_ITSurvey_MagOn_2012ZSurvey2011YBoxBilayerCorrected.xml" ]
        self.XmlUncertainties += [ "ITSystem                   : 1 1 1 0.01 0.01 0.01",
                                   "ITT.                       : 0.5 0.5 0.5 0.001 0.001 0.001",
                                   "ITT.*?Box                  : 0.5 0.5 0.5 0.001 0.001 0.001",
                                   "ITT.*?Layer.{1,2}          : 0.2 0.05 0.05 0.0001 0.0001 0.001",
                                   "ITT.*?Layer(X1U|VX2)       : 0.2 0.05 0.05 0.0001 0.0001 0.001",
                                   "ITT.*?Layer.{1,2}Ladder.*? : 0.1 0.02 0.02 0.0001 0.0001 0.001" ] 
    
    def MUON( self ) :
        self.Constraints += [ 
            "Muon/M1/M1ASide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M1/M1CSide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M2/M2ASide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M2/M2CSide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M3/M3ASide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M3/M3CSide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M4/M4ASide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M4/M4CSide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M5/M5ASide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001",
            "Muon/M5/M5CSide : 0 0 0 0 0 0 : 1 1 3 0.001 0.005 0.001" ]

    def All( self ) :
        self.Velo()
        self.TT()
        self.IT()
        self.OT()
        self.MUON()
        self.Constraints += [ "Tracker : 0 0 0 0 0 0 : 10 10 10 0.1 0.1 0.1",
                              "TStations : 0 0 0 0 0 0 : 10 10 10 0.1 0.1 0.1" ]

    def configureTool( self, surveyConstraintTool ) :
        surveyConstraintTool.Constraints      = self.Constraints
        surveyConstraintTool.XmlFiles         = self.XmlFiles
        surveyConstraintTool.XmlUncertainties = self.XmlUncertainties
        
    def __apply_configuration__(self):
        log.info( self )
    #    surveyConstraintTool = AlignChisqConstraintTool( self.name() )
    #    configureTool( surveyConstraintTool )
