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
        
    def OTCFrames( self, dofs = "TxTyTzRxRyRz" ) :
        # Q0 is C-side
        if dofs.find( "Tx" ) >=0 :
            self.append("OT/T1X1UCSide : Tx : -0.79 +/- 0.5")
            self.append("OT/T1X1UASide : Tx : -1.43 +/- 0.5")
            self.append("OT/T1VX2CSide : Tx : -0.35 +/- 0.5")
            self.append("OT/T1VX2ASide : Tx : -0.47 +/- 0.5")
            self.append("OT/T2X1UCSide : Tx : -0.98 +/- 0.5")
            self.append("OT/T2X1UASide : Tx : -0.27 +/- 0.5")
            self.append("OT/T2VX2CSide : Tx :  0.04 +/- 0.5")
            self.append("OT/T2VX2ASide : Tx : -1.69 +/- 0.5")
            self.append("OT/T3X1UCSide : Tx : -0.04 +/- 0.5")
            self.append("OT/T3X1UASide : Tx : -1.52 +/- 0.5")
            self.append("OT/T3VX2CSide : Tx :  0.20 +/- 0.5")
            self.append("OT/T3VX2ASide : Tx : -1.49 +/- 0.5")

        if dofs.find( "Ty" ) >=0 :
            self.append("OT/T1X1UCSide : Ty : -0.05 +/- 0.5")
            self.append("OT/T1X1UASide : Ty :  0.05 +/- 0.5")
            self.append("OT/T1VX2CSide : Ty : -1.50 +/- 0.5")
            self.append("OT/T1VX2ASide : Ty : -0.65 +/- 0.5")
            self.append("OT/T2X1UCSide : Ty : -0.70 +/- 0.5")
            self.append("OT/T2X1UASide : Ty :  0.05 +/- 0.5")
            self.append("OT/T2VX2CSide : Ty : -1.70 +/- 0.5")
            self.append("OT/T2VX2ASide : Ty : -0.85 +/- 0.5")
            self.append("OT/T3X1UCSide : Ty : -1.00 +/- 0.5")
            self.append("OT/T3X1UASide : Ty : -1.25 +/- 0.5")
            self.append("OT/T3VX2CSide : Ty : -1.70 +/- 0.5")
            self.append("OT/T3VX2ASide : Ty : -1.30 +/- 0.5")

        if dofs.find( "Tz" ) >=0 :
            self.append("OT/T1X1UCSide : Tz :  0    +/- 0.5")
            self.append("OT/T1X1UASide : Tz :  0    +/- 0.5")
            self.append("OT/T1VX2CSide : Tz : -0.5  +/- 0.5")
            self.append("OT/T1VX2ASide : Tz : -0.5  +/- 0.5")
            self.append("OT/T2X1UCSide : Tz : -2.50 +/- 0.5")
            self.append("OT/T2X1UASide : Tz : -2.50 +/- 0.5")
            self.append("OT/T2VX2CSide : Tz : -3.50 +/- 0.5")
            self.append("OT/T2VX2ASide : Tz : -3.50 +/- 0.5")
            self.append("OT/T3X1UCSide : Tz :  0.00 +/- 0.5")
            self.append("OT/T3X1UASide : Tz :  0.00 +/- 0.5")
            self.append("OT/T3VX2CSide : Tz : -2.50 +/- 0.5")
            self.append("OT/T3VX2ASide : Tz : -2.50 +/- 0.5")
            
        if dofs.find( "Rz" ) >=0 :
            self.append("OT/T1X1UCSide : Rz : 0.0001")
            self.append("OT/T1X1UASide : Rz : 0.0001")
            self.append("OT/T1VX2CSide : Rz : 0.0001")
            self.append("OT/T1VX2ASide : Rz : 0.0001")
            self.append("OT/T2X1UCSide : Rz : 0.0001")
            self.append("OT/T2X1UASide : Rz : 0.0001")
            self.append("OT/T2VX2CSide : Rz : 0.0001")
            self.append("OT/T2VX2ASide : Rz : 0.0001")
            self.append("OT/T3X1UCSide : Rz : 0.0001")
            self.append("OT/T3X1UASide : Rz : 0.0001")
            self.append("OT/T3VX2CSide : Rz : 0.0001")
            self.append("OT/T3VX2ASide : Rz : 0.0001")

    def OT( self ) :
        self.append("OT/.*?M. : 0 0 0 0 0 0 : 0.05 0.05 0.05 0.00005 0.0005 0.00005")
        self.OTCFrames() 

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
        
    def ITBoxes( self ) :
        self += [
            ".*?/IT/Station1/TopBox : Tx : -2.683    +/- 0.5",
            ".*?/IT/Station1/TopBox : Ty :  3.582    +/- 0.5",
            ".*?/IT/Station1/TopBox : Tz : -2.407    +/- 0.5",
            ".*?/IT/Station1/TopBox : Rx :  0.004537 +/- 0.0005",
            ".*?/IT/Station1/TopBox : Ry :  0.001436 +/- 0.0005",
            ".*?/IT/Station1/TopBox : Rz :  0.000667 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station1/BottomBox : Tx : -2.5600   +/- 0.5",
            ".*?/IT/Station1/BottomBox : Ty :  2.4144   +/- 0.5",
            ".*?/IT/Station1/BottomBox : Tz : -0.8528   +/- 0.5",
            ".*?/IT/Station1/BottomBox : Rx : -0.007903 +/- 0.0005",
            ".*?/IT/Station1/BottomBox : Ry : -0.005081 +/- 0.0005",
            ".*?/IT/Station1/BottomBox : Rz :  0.001245 +/- 0.0005" ]

        self += [
            ".*?/IT/Station1/ASideBox : Tx : -2.9199   +/- 0.5",
            ".*?/IT/Station1/ASideBox : Ty :  3.00609  +/- 0.5",
            ".*?/IT/Station1/ASideBox : Tz : -4.70069  +/- 0.5",
            ".*?/IT/Station1/ASideBox : Rx :  0.000525 +/- 0.0005",
            ".*?/IT/Station1/ASideBox : Ry : -0.002353 +/- 0.0005",
            ".*?/IT/Station1/ASideBox : Rz :  0.001249 +/- 0.0005" ]

        self += [
            ".*?/IT/Station1/CSideBox : Tx :  0.23095  +/- 0.5",
            ".*?/IT/Station1/CSideBox : Ty : -4.90356  +/- 0.5",
            ".*?/IT/Station1/CSideBox : Tz :  5.73112  +/- 0.5",
            ".*?/IT/Station1/CSideBox : Rx : -0.000841 +/- 0.0005",
            ".*?/IT/Station1/CSideBox : Ry :  0.002909 +/- 0.0005",
            ".*?/IT/Station1/CSideBox : Rz :  0.002909 +/- 0.0005" ]

        self += [
            ".*?/IT/Station2/TopBox : Tx : -1.30598  +/- 0.5",
            ".*?/IT/Station2/TopBox : Ty :  1.59455  +/- 0.5",
            ".*?/IT/Station2/TopBox : Tz :  0.13497  +/- 0.5",
            ".*?/IT/Station2/TopBox : Rx :  0.008177 +/- 0.0005",
            ".*?/IT/Station2/TopBox : Ry :  0.000932 +/- 0.0005",
            ".*?/IT/Station2/TopBox : Rz : -0.001863 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station2/BottomBox : Tx : -4.1925   +/- 0.5",
            ".*?/IT/Station2/BottomBox : Ty :  3.96063  +/- 0.5",
            ".*?/IT/Station2/BottomBox : Tz :  1.37977  +/- 0.5",
            ".*?/IT/Station2/BottomBox : Rx : -0.005788 +/- 0.0005",
            ".*?/IT/Station2/BottomBox : Ry :  0.001327 +/- 0.0005",
            ".*?/IT/Station2/BottomBox : Rz :  0.001875 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station2/ASideBox : Tx : -5.31114  +/- 0.5",
            ".*?/IT/Station2/ASideBox : Ty :  1.74568  +/- 0.5",
            ".*?/IT/Station2/ASideBox : Tz : -1.74338  +/- 0.5",
            ".*?/IT/Station2/ASideBox : Rx :  0.008217 +/- 0.0005",
            ".*?/IT/Station2/ASideBox : Ry : -0.005128 +/- 0.0005",
            ".*?/IT/Station2/ASideBox : Rz : -0.001121 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station2/CSideBox : Tx : -1.22504  +/- 0.5",
            ".*?/IT/Station2/CSideBox : Ty : -1.82756  +/- 0.5",
            ".*?/IT/Station2/CSideBox : Tz :  2.45954  +/- 0.5",
            ".*?/IT/Station2/CSideBox : Rx : -0.000179 +/- 0.0005",
            ".*?/IT/Station2/CSideBox : Ry :  0.007642 +/- 0.0005",
            ".*?/IT/Station2/CSideBox : Rz : -0.000011 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/TopBox : Tx : -1.15984  +/- 0.5",
            ".*?/IT/Station3/TopBox : Ty :  0.96979  +/- 0.5",
            ".*?/IT/Station3/TopBox : Tz : -4.18076  +/- 0.5",
            ".*?/IT/Station3/TopBox : Rx : -0.002836 +/- 0.0005",
            ".*?/IT/Station3/TopBox : Ry :  0.002701 +/- 0.0005",
            ".*?/IT/Station3/TopBox : Rz :  0.000806 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/BottomBox : Tx : -3.28965  +/- 0.5",
            ".*?/IT/Station3/BottomBox : Ty :  3.39952  +/- 0.5",
            ".*?/IT/Station3/BottomBox : Tz : -3.65342  +/- 0.5",
            ".*?/IT/Station3/BottomBox : Rx :  0.003166 +/- 0.0005",
            ".*?/IT/Station3/BottomBox : Ry :  0.006002 +/- 0.0005",
            ".*?/IT/Station3/BottomBox : Rz : -0.0028   +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/ASideBox : Tx : -2.66769  +/- 0.5",
            ".*?/IT/Station3/ASideBox : Ty :  0.880385 +/- 0.5",
            ".*?/IT/Station3/ASideBox : Tz : -5.98421  +/- 0.5",
            ".*?/IT/Station3/ASideBox : Rx :  0.006741 +/- 0.0005",
            ".*?/IT/Station3/ASideBox : Ry :  0.003694 +/- 0.0005",
            ".*?/IT/Station3/ASideBox : Rz :  0.001242 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/CSideBox : Tx :  0.395279 +/- 0.5",
            ".*?/IT/Station3/CSideBox : Ty : -4.67969  +/- 0.5",
            ".*?/IT/Station3/CSideBox : Tz :  9.81678  +/- 0.5",
            ".*?/IT/Station3/CSideBox : Rx : -0.015437 +/- 0.0005",
            ".*?/IT/Station3/CSideBox : Ry :  0.000479 +/- 0.0005",
            ".*?/IT/Station3/CSideBox : Rz : -0.002224 +/- 0.0005" ]
        
