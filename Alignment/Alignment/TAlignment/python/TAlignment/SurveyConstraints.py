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
