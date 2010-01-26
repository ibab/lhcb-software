import types

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
        self.append("Velo      : 0 0 0 -0.0001 0 -0.0001 : 0.2 0.2 0.2 0.0001 0.0001 0.0001" )
        self.append("Velo/Velo(Right|Left) : 0 0 0 0 0 0 : 0.2 0.2 0.2 0.0001 0.0001 0.0001" ) 
        
    def OT( self ) :
        # modules
        self.append("OT/.*?M. : 0 0 0 0 0 0 : 0.05 0.05 0.05 0.00005 0.0005 0.00005")

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

        # this is for the average of 2 C-frames
        self.append("OT/T1X1U : -1.11  0.00  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T1VX2 : -0.41 -1.50 -0.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2X1U : -0.63 -0.33 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T2VX2 : -0.83 -1.28 -3.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3X1U : -0.78 -1.13  0.0  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        self.append("OT/T3VX2 : -0.65 -1.50 -2.5  0.0 0.0 0.0 : 0.5 0.5 0.5 0.0001 0.0001 0.0001")
        
    def TT( self ) :
        self += [ self.defaultSurveyDir() + "TT/Detectors.xml" ]
        self += [ self.defaultSurveyDir() + "TT/Modules.xml" ]
        self += [ self.defaultSurveyDir() + "TT/Sensors.xml" ]
        #self.append(".*?/TT/.*?/Module.{2} : Tx : 0.1")
        #self.append(".*?/TT/.*?/Module.{2} : Rz : 0.0001")
        # halflayers
        self.append("TTaXLayerASide : Tx : 0.1")
        self.append("TTaULayerASide : Tx : 0.1")
        self.append("TTaXLayerCSide : Tx : 0.1")
        self.append("TTaULayerCSide : Tx : 0.1")
        self.append("TTbVLayerASide : Tx : 0.1")
        self.append("TTbXLayerASide : Tx : 0.1")
        self.append("TTbVLayerCSide : Tx : 0.1")
        self.append("TTbXLayerCSide : Tx : 0.1")
        # boxes
        self.append("TTASide : Tx : 0.5")
        self.append("TTASide : Ty : 0.5")
        self.append("TTASide : Tz : 0.5")
        self.append("TTCSide : Tx : 0.5")
        self.append("TTCSide : Ty : 0.5")
        self.append("TTCSide : Tz : 0.5")
        # global
        self.append("TT : Tx : 0.5")
        self.append("TT : Ty : 0.5")
        self.append("TT : Tz : 0.5")

    def IT( self ) :
        self += [ self.defaultSurveyDir() + "IT/Detectors.xml" ]

    def All( self ) :
        self.Velo()
        self.TT()
        self.IT()
        self.OT()
