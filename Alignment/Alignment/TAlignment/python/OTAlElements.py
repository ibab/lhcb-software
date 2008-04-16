class OTAlElements:

    def __init__( self ):
        self.m_dofs     = [ "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" ]
        self.m_ot       = "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        self.m_stations = [ "/T1", "/T2", "/T3" ]
        self.m_layers   = [ "/X1layer", "/Ulayer", "/Vlayer", "/X2layer" ]
        self.m_quarters = [ "/Quarter(0|2)", "/Quarter(1|3)" ]
        self.m_modules  = [ "/Module1", "/Module2", "/Module3", "/Module4", "/Module5", "/Module6", "/Module7", "/Module8", "/Module9" ]

    def __findDOF( self, dof, dofs ):
        valid = ""
        if not dofs.find( dof ) == -1:
            valid = dof

        return valid
    
    def __addDoFs( self, elements, dofs ) :
        valid   = ""
        invalid = dofs

        for i in range( len( self.m_dofs ) ) :
            valid += self.__findDOF( self.m_dofs[i], invalid )
            invalid = invalid.replace( self.m_dofs[i] , "" )
            
        if valid :
            for i in range( len( elements ) ) :
                elements[ i ] += ":%s"%valid
                
        if invalid :
            print "ERROR: Unknown degree of freedom(s) " + '"' + invalid + '"'
            print "       Valid degrees of freedom are Tx, Ty, Tz, Rx, Ry and Rz" 
            
    def OTStations( self, dofs = "" ) :
        elements = []
        for i in range( len( self.m_stations ) ) :
            elements.append( self.m_ot + self.m_stations[ i ] )

        if dofs :
           self.__addDoFs( elements, dofs )

        return elements
                
    def OTLayers( self, dofs = "" ) :
        elements = []
        for i in range( len( self.m_stations ) ) :
            for j in range( len( self.m_layers ) ) :
                elements.append( self.m_ot + self.m_stations[ i ] + self.m_layers[j] )
                
        if dofs :
           self.__addDoFs( elements, dofs )
           
        return elements

    def OTModules( self, dofs = "" ) :
        elements = []
        ## 3 Stations numbered from 1 to 3
        for i in range( len( self.m_stations ) ) :
            ## There are four layers X1, U, V and X2
            for j in range( len( self.m_layers ) ) :
                ## Quarters 0 and 2 (C-Side) and Quarters 1 and 3 (A-side)
                for k in range( len( self.m_quarters ) ) :
                    ## Nine modules numbered from 1 to 9
                    for l in range( len( self.m_modules ) ) :
                        elements.append( self.m_ot + self.m_stations[ i ] + self.m_layers[ j ] + self.m_quarters[ k ] + self.m_modules[ l ] )

        if dofs :
           self.__addDoFs( elements, dofs )
           
        return elements
        
    def OTCFrames( self, dofs = "" ) :
        elements = []
        for i in range( len( self.m_stations ) ) :
            station = self.m_ot + self.m_stations[ i ]
            ## C-side X1 and U
            elements.append( station + "/(X1|U)layer" + self.m_quarters[0] + "/Module.")
            ## C-side V and X2
            elements.append( station + "/(V|X2)layer" + self.m_quarters[0] + "/Module.")
            ## A-side X1 and U
            elements.append( station + "/(X1|U)layer" + self.m_quarters[1] + "/Module.")
            ## A-side V and X2
            elements.append( station + "/(V|X2)layer" + self.m_quarters[1] + "/Module.")
            
        if dofs :
           self.__addDoFs( elements, dofs )
                
        return elements
            

    
