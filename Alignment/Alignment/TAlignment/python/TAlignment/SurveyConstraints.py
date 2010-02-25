import types

from Configurables import Al__AlignChisqConstraintTool as AlignChisqConstraintTool

class SurveyConstraints( list ):

    def __init__( self, elements = None, dofs = "" ) :        
        self.m_dofs       = [ "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" ]
        
    def __findDOF( self, dof, dofs ):
        valid = ""
        if not dofs.find( dof ) == -1:
            valid = dof

        return valid

    def __append( self,  elements, dofs ) :      
        if dofs :
            tmp = self.__addDoFs( elements, dofs )
            self.extend( tmp )
        else :
            if type( elements ) == types.ListType :
                self.extend( elements )
            elif type( elements ) == types.StringType :
                self.append( elements )

    def defaultSurveyDir( self ) :
        import os
        return os.getenv("TALIGNMENTROOT") + "/surveyxml/"

    def Velo( self ) :
        self += [ self.defaultSurveyDir() + "Velo/Modules.xml" ]
        self += [ self.defaultSurveyDir() + "Velo/Detectors.xml" ]
        AlignChisqConstraintTool().XmlUncertainties += ["Module(PU|).. : 0.02 0.02 0.02 0.000001 0.000001 0.000001",
                                                        "Detector(PU|)..-.. : 0.005 0.005 0.005 0.000001 0.000001 0.000001" ]
        self.append("Velo      : 0 0 0 -0.0001 0 -0.0001 : 0.2 0.2 0.2 0.0001 0.0001 0.0001" )
        self.append("Velo/Velo(Right|Left) : 0 0 0 0 0 0 : 10 1 0.2 0.0001 0.0001 0.0001" ) 
        
    def OT( self ) :
        # we don't load OT xml yet. there is no usefull info, since it doesn't have the right structures in geometry.
                
        # modules
        self.append("OT/.*?M. : 0 0 0 0 0 0 : 0.05 0.05 0.05 0.00001 0.00001 0.00001")

        # C frames
        self.append("OT/T1X1UASide : -1.43  0.05  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T1X1UCSide : -0.79 -0.05  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T1VX2ASide : -0.47 -0.65 -0.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T1VX2CSide : -0.35 -1.50 -0.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2X1UASide : -0.27  0.05 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2X1UCSide : -0.98 -0.70 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2VX2ASide : -1.69 -0.85 -3.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2VX2CSide :  0.04 -1.70 -3.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3X1UASide : -1.52 -1.25  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3X1UCSide : -0.04 -1.00  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3VX2ASide : -1.49 -1.30 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3VX2CSide :  0.20 -1.70 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")

        # this is for the average of 2 C-frames.
        # corrected  for the bowing of the bridge (2010/01/26)
        self.append("OT/T1X1U : -1.11 -0.7  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T1VX2 : -0.41 -2.2 -0.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2X1U : -0.63 -1.2 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2VX2 : -0.83 -2.2 -3.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3X1U : -0.78 -2.5  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3VX2 : -0.65 -2.9 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")

        # OT global
        self.append("OTSystem : 0 0 0 0 0 0 : 1 1 1 0.001 0.001 0.001")
        
    def TT( self ) :
        self += [ self.defaultSurveyDir() + "TT/Detectors.xml" ]
        self += [ self.defaultSurveyDir() + "TT/Modules.xml" ]
        self += [ self.defaultSurveyDir() + "TT/Sensors.xml" ]
        AlignChisqConstraintTool().XmlUncertainties += [ "TTSystem             : 0.5 0.5 0.5 0.001 0.001 0.001",
                                                         "TT..LayerR.Module.*? : 0.1 0.1 0.1 0.0005 0.0005 0.0005"]
        
        #self.append(".*?/TT/.*?/Module.{2} : Tx : 0.1")
        #self.append(".*?/TT/.*?/Module.{2} : Rz : 0.0001")
        # halflayers
        self.append("TTaXLayerASide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        self.append("TTaULayerASide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        self.append("TTaXLayerCSide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        self.append("TTaULayerCSide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        self.append("TTbVLayerASide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        self.append("TTbXLayerASide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        self.append("TTbVLayerCSide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        self.append("TTbXLayerCSide : 0 0 0 0 0 0 : 0.1 0.1 0.1 0.0002 0.0002 0.0002")
        # boxes
        self.append("TTASide : 0 0 0 0 0 0 : 0.2 0.2 0.2 0.001 0.001 0.001")
        self.append("TTCSide : 0 0 0 0 0 0 : 0.2 0.2 0.2 0.001 0.001 0.001")
 
    def IT( self ) :
        self += [ self.defaultSurveyDir() + "IT/Detectors.xml" ]
        AlignChisqConstraintTool().XmlUncertainties += [ "ITSystem                   : 1 1 1 0.01 0.01 0.01",
                                                         "ITT.                       : 0.5 0.5 0.5 0.001 0.001 0.001",
                                                         "ITT.*?Box                  : 0.5 0.5 0.5 0.005 0.002 0.001",
                                                         "ITT.*?Layer.{1,2}          : 0.05 0.05 0.05 0.0001 0.0001 0.0001",
                                                         "ITT.*?Layer.{1,2}Ladder.*? : 0.02 0.02 0.02 0.0001 0.0001 0.0001" ] 

    def All( self ) :
        self.Velo()
        self.TT()
        self.IT()
        self.OT()
