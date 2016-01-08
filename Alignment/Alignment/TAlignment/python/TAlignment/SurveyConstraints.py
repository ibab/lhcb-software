from Gaudi.Configuration  import *
from Configurables import LHCbConfigurableUser
#from Configurables import Al__AlignChisqConstraintTool as AlignChisqConstraintTool

# Dictionary with all the available XMLfiles
XmlFilesDict = { 'VP': {'latest': ['Modules']},
                 'Velo': {'latest': ['Modules'],
                          'Run1': ['Modules_run1', 'Detectors_run1']},
                 'OT': {'latest': ['ElementsBowingCorrected'],
                        '2010': [],
                        'old': ['Elements', 'ElementsBowingCorrected']},
                 'IT': {'latest': ['Detectors_ITSurvey_MagOn_2012ZSurvey2011YBoxBilayerCorrected','ITBoxSurvey20150413'],
                        '2015': ['Detectors_ITSurvey_MagOn_2012ZSurvey2011YBoxBilayerCorrected','ITBoxSurvey20150413'],
                        '2012-on': ['Detectors_ITSurvey_MagOn_2012ZSurvey2011YBoxBilayerCorrected'],
                        '2012-off': ['Detectors_ITSurvey_MagOff_2012ZSurvey2011YBoxBilayerCorrected'],
                        '2011': ['Detectors_ITSurvey2011YBoxBilayerCorrected'],
                        '2010': ['ITSurvey2010BoxYCorrected'],
                        '2009': ['ITBoxSurvey2009'],
                        '2008': ['ITSurvey2008'],
                        'metrology': ['metrology_onlysensors'],
                        'old': ['Detectors']},
                 'TT': {'latest': ['Detectors_TTSurvey2011SystemYCorrected', 'TTSensorSurvey2011', 'Modules'],
                        '2010': ['Detectors', 'TTSurvey2010YCorrected', 'Modules'],
                        'old': ['Detectors', 'Sensors', 'Modules'] } }

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

    def Velo( self, ver='latest' ) :
        if not ver in XmlFilesDict['Velo'].keys():
            print 'WARNING(SurveyConstraints): Survey constraint version '+ver+' not found for Velo. Using \'latest\''
            ver = 'latest'
        for f in XmlFilesDict['Velo'][ver]: self.XmlFiles += [ self.defaultSurveyDir() + "Velo/"+f+".xml" ]
        self.XmlUncertainties += ["Module(PU|).. : 0.02 0.02 0.02 0.0002 0.0002 0.0002",
 				  "Detector(PU|)..-.. : 0.005 0.005 0.005 0.0001 0.0001 0.0001" ]
        self.Constraints += [ "Velo      : 0 0 0 -0.0001 0 -0.0001 : 0.2 0.2 0.2 0.0001 0.0001 0.001",
                              "Velo/Velo(Right|Left) : 0 0 0 0 0 0 : 10 1 0.2 0.0001 0.0001 0.0001" ]
    def VP( self, ver='latest' ) :
        if not ver in XmlFilesDict['VP'].keys():
            print 'WARNING(SurveyConstraints): Survey constraint version '+ver+' not found for Velo. Using \'latest\''
            ver = 'latest'
        for f in XmlFilesDict['VP'][ver]: self.XmlFiles += [ self.defaultSurveyDir() + "VP/"+f+".xml" ]
        self.XmlUncertainties += ["Module(00|).. : 0.02 0.02 0.02 0.0002 0.0002 0.0002" ]
        self.Constraints += ["VP/VP(Right|Left) : 0. 0. 0. 0. 0. 0. : 0.200 0.200 0.2 0.0001 0.0001 0.0001" ]
    def OT( self, ver='latest' ) :
        # we don't load OT xml yet. there is no usefull info, since it doesn't have the right structures in geometry. (solved in 2011 - do not use old file)
        if not ver in XmlFilesDict['OT'].keys():
            print 'WARNING(SurveyConstraints): Survey constraint version '+ver+' not found for OT. Using \'latest\''
            ver = 'latest'
        for f in XmlFilesDict['OT'][ver]: self.XmlFiles += [ self.defaultSurveyDir() + "OT/"+f+".xml" ]
        self.XmlUncertainties += [ "OT/T.(X1U|VX2) : 0.5 0.5 0.5 0.0001 0.0001 0.0001",
                                   "OT/T.(X1U|VX2).Side : 0.5 0.5 0.5 0.0001 0.0001 0.0001",
                                   "OT : 0.5 0.5 0.5 0.001 0.001 0.001",
                                   "OT/T. : 0.5 0.0001 0.5 0.0001 0.0001 0.0001"]
                
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
        
    def TT( self, ver='latest' ) :
        if not ver in XmlFilesDict['TT'].keys():
            print 'WARNING(SurveyConstraints): Survey constraint version '+ver+' not found for TT. Using \'latest\''
            ver = 'latest'
        for f in XmlFilesDict['TT'][ver]: self.XmlFiles += [ self.defaultSurveyDir() + "TT/"+f+".xml" ]
        self.XmlUncertainties += [ "TTSystem             : 0.5 0.5 0.5 0.001 0.001 0.001",
                                   "TT.                  : 0.1 0.1 0.1 0.0005 0.0005 0.0005",
                                   "TT..Layer            : 0.1 0.1 0.1 0.0005 0.0005 0.0005",
                                   "TT..Layer.Side       : 0.1 0.1 0.1 0.0005 0.0005 0.0005",
                                   "TT..LayerR.Module.*? : 0.1 0.1 0.1 0.0005 0.0005 0.0005"]
#                                   "TT..LayerR.Module.{1,2}    : 0.1 0.1 0.1 0.0005 0.0005 0.0005", ## To match Wouter's new definition
#                                   "TT..LayerR.Module..Sensor. : 0.005 0.005 0.005 0.000005 0.000005 0.000005"]  
        self.XmlUncertainties += [ "TT.*?(Low|High)Z     : 0.1 0.1 1.0 0.0005 0.0005 0.0005"]
        
        # boxes
        self.Constraints += [ "TTASide : 0 0 0 0 0 0 : 0.2 0.2 0.2 0.001 0.001 0.001",
                              "TTCSide : 0 0 0 0 0 0 : 0.2 0.2 0.2 0.001 0.001 0.001" ]
 
    def IT( self, ver='latest' ) :
        if not ver in XmlFilesDict['IT'].keys():
            print 'WARNING(SurveyConstraints): Survey constraint version '+ver+' not found for IT. Using \'latest\''
            ver = 'latest'
        for f in XmlFilesDict['IT'][ver]: self.XmlFiles += [ self.defaultSurveyDir() + "IT/"+f+".xml" ]
        self.XmlUncertainties += [ "ITSystem                   : 1 1 1 0.01 0.01 0.01",
                                   "ITT.                       : 0.5 0.5 0.5 0.001 0.001 0.001",
                                   "ITT.*?Box                  : 2.0 2.0 2.0 0.001 0.001 0.001",
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

    def All( self, ver='latest' ) :
        self.Velo(ver)
        self.VP(ver)
        self.TT(ver)
        self.IT(ver)
        self.OT(ver)
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
