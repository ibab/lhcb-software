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

    def OTModules( self, dofs = "TxTyTzRxRyRz" ) :
        if dofs.find( "Tx" ) >=0 :
            self.append("/.*?/OT/.*?/M. : Tx : 0.1")
        if dofs.find( "Rz" ) >=0 :
            self.append("/.*?/OT/.*?/M. : Rz : 0.0001")

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
            self.append("OT/T.*?Side : Rz : 0.0001")

    def ITBoxes( self ) :
        self += [
            ".*?/IT/Station1/TopBox : Tx : -2.145    +/- 0.5",
            ".*?/IT/Station1/TopBox : Ty :  4.308    +/- 0.5",
            ".*?/IT/Station1/TopBox : Tz : -2.580    +/- 0.5",
            ".*?/IT/Station1/TopBox : Rx :  0.005909 +/- 0.0005",
            ".*?/IT/Station1/TopBox : Ry :  0.001813 +/- 0.0005",
            ".*?/IT/Station1/TopBox : Rz :  0.000841 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station1/BottomBox : Tx : -1.8729  +/- 0.5",
            ".*?/IT/Station1/BottomBox : Ty :  1.3320  +/- 0.5",
            ".*?/IT/Station1/BottomBox : Tz :  0.2386  +/- 0.5",
            ".*?/IT/Station1/BottomBox : Rx : -0.004733 +/- 0.0005",
            ".*?/IT/Station1/BottomBox : Ry : -0.001014 +/- 0.0005",
            ".*?/IT/Station1/BottomBox : Rz :  0.000993 +/- 0.0005" ]

        self += [
            ".*?/IT/Station1/ASideBox : Tx : -1.90288  +/- 0.5",
            ".*?/IT/Station1/ASideBox : Ty :  2.11402  +/- 0.5",
            ".*?/IT/Station1/ASideBox : Tz : -4.00045  +/- 0.5",
            ".*?/IT/Station1/ASideBox : Rx :  0.000041 +/- 0.0005",
            ".*?/IT/Station1/ASideBox : Ry : -0.002059 +/- 0.0005",
            ".*?/IT/Station1/ASideBox : Rz :  0.000720 +/- 0.0005" ]

        self += [
            ".*?/IT/Station1/CSideBox : Tx :  1.2936   +/- 0.5",
            ".*?/IT/Station1/CSideBox : Ty : -5.55219  +/- 0.5",
            ".*?/IT/Station1/CSideBox : Tz :  5.43572  +/- 0.5",
            ".*?/IT/Station1/CSideBox : Rx : -0.005500 +/- 0.0005",
            ".*?/IT/Station1/CSideBox : Ry :  0.004248 +/- 0.0005",
            ".*?/IT/Station1/CSideBox : Rz :  0.002658 +/- 0.0005" ]

        self += [
            ".*?/IT/Station2/TopBox : Tx : -0.08482 +/- 0.5",
            ".*?/IT/Station2/TopBox : Ty :  2.20381 +/- 0.5",
            ".*?/IT/Station2/TopBox : Tz :  0.16064 +/- 0.5",
            ".*?/IT/Station2/TopBox : Rx :  0.011685 +/- 0.0005",
            ".*?/IT/Station2/TopBox : Ry :  0.001257 +/- 0.0005",
            ".*?/IT/Station2/TopBox : Rz : -0.002605 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station2/BottomBox : Tx : -0.99362 +/- 0.5",
            ".*?/IT/Station2/BottomBox : Ty :  3.422166 +/- 0.5",
            ".*?/IT/Station2/BottomBox : Tz :  1.19502 +/- 0.5",
            ".*?/IT/Station2/BottomBox : Rx : -0.00293721 +/- 0.0005",
            ".*?/IT/Station2/BottomBox : Ry :  0.00111782 +/- 0.0005",
            ".*?/IT/Station2/BottomBox : Rz :  0.00151636 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station2/ASideBox : Tx : -2.69929 +/- 0.5",
            ".*?/IT/Station2/ASideBox : Ty :  0.97438 +/- 0.5",
            ".*?/IT/Station2/ASideBox : Tz : -1.70793 +/- 0.5",
            ".*?/IT/Station2/ASideBox : Rx :  0.009677 +/- 0.0005",
            ".*?/IT/Station2/ASideBox : Ry : -0.004317 +/- 0.0005",
            ".*?/IT/Station2/ASideBox : Rz : -0.000251 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station2/CSideBox : Tx : -0.2541   +/- 0.5",
            ".*?/IT/Station2/CSideBox : Ty : -3.1283   +/- 0.5",
            ".*?/IT/Station2/CSideBox : Tz :  3.37044  +/- 0.5",
            ".*?/IT/Station2/CSideBox : Rx : -0.001755 +/- 0.0005",
            ".*?/IT/Station2/CSideBox : Ry :  0.003820 +/- 0.0005",
            ".*?/IT/Station2/CSideBox : Rz : -0.000383 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/TopBox : Tx : -1.3687 +/- 0.5",
            ".*?/IT/Station3/TopBox : Ty :  2.1091 +/- 0.5",
            ".*?/IT/Station3/TopBox : Tz : -3.7869 +/- 0.5",
            ".*?/IT/Station3/TopBox : Rx : -0.006849 +/- 0.0005",
            ".*?/IT/Station3/TopBox : Ry : -0.000077 +/- 0.0005",
            ".*?/IT/Station3/TopBox : Rz :  0.000590 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/BottomBox : Tx : -3.2069 +/- 0.5",
            ".*?/IT/Station3/BottomBox : Ty :  1.5801 +/- 0.5",
            ".*?/IT/Station3/BottomBox : Tz : -4.6517 +/- 0.5",
            ".*?/IT/Station3/BottomBox : Rx :  0.001111 +/- 0.0005",
            ".*?/IT/Station3/BottomBox : Ry :  0.002706 +/- 0.0005",
            ".*?/IT/Station3/BottomBox : Rz : -0.002938 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/ASideBox : Tx : -2.3649  +/- 0.5",
            ".*?/IT/Station3/ASideBox : Ty : -0.6294 +/- 0.5",
            ".*?/IT/Station3/ASideBox : Tz : -6.4968 +/- 0.5",
            ".*?/IT/Station3/ASideBox : Rx :  0.006865 +/- 0.0005",
            ".*?/IT/Station3/ASideBox : Ry :  0.004058 +/- 0.0005",
            ".*?/IT/Station3/ASideBox : Rz :  0.002164 +/- 0.0005" ]
        
        self += [
            ".*?/IT/Station3/CSideBox : Tx :  0.6910 +/- 0.5",
            ".*?/IT/Station3/CSideBox : Ty : -6.1691 +/- 0.5",
            ".*?/IT/Station3/CSideBox : Tz :  9.7834 +/- 0.5",
            ".*?/IT/Station3/CSideBox : Rx : -0.015896 +/- 0.0005",
            ".*?/IT/Station3/CSideBox : Ry : -0.000636 +/- 0.0005",
            ".*?/IT/Station3/CSideBox : Rz : -0.001525 +/- 0.0005" ]
        
