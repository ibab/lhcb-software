import types

class Alignables( list ):
    
    def __init__( self, elements = None, dofs = "" ) :        
        if elements and self.__validElements( elements ) :
            self.m_dofs     = [ "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" ]
            self.m_velo     = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
            self.m_tt       = "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
            self.m_it       = "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
            self.m_ot       = "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
            self.m_stations = [ "/T1", "/T2", "/T3" ]
            self.m_layers   = [ "/X1layer", "/Ulayer", "/Vlayer", "/X2layer" ]
            self.m_quarters = [ "/Quarter(0|2)", "/Quarter(1|3)" ]
            self.m_modules  = [ "/Module1", "/Module2", "/Module3", "/Module4", "/Module5", "/Module6", "/Module7", "/Module8", "/Module9" ]
            
            self.__append( elements, dofs )
        
    def __findDOF( self, dof, dofs ):
        valid = ""
        if not dofs.find( dof ) == -1:
            valid = dof

        return valid
    
    def __addDoFs( self, elements, dofs ) :
        valid   = ""
        invalid = dofs
        tmp = []

        for i in range( len( self.m_dofs ) ) :
            valid += self.__findDOF( self.m_dofs[i], invalid )
            invalid = invalid.replace( self.m_dofs[i] , "" )    

        if valid :
            if type( elements ) == types.ListType :
                for i in range( len( elements ) ) :
                    tmp.append( elements[i] + ":%s"%valid )
            elif type( elements ) == types.StringType :
                tmp.append( elements + ":%s"%valid )
                
        try: ## asserting whether invalid is not empty       
            assert( not invalid )
        except AssertionError :
            print "ERROR: Unknown degree of freedom(s) " + '"' + invalid + '"'
            print "       Valid degrees of freedom are Tx, Ty, Tz, Rx, Ry and Rz" 
            return []
        
        return tmp
    
    def __validElements( self, elements ) :
        try:
            assert( type( elements ) == types.ListType or type( elements ) == types.StringType )
        except AssertionError :
            self.__elementsError()
            return None
        
        ## Paranoia; extend also throws an exception
        if type( elements ) == types.ListType :
            for i in range( len( elements ) ) :
                try:
                    assert( type( elements[ i ] ) == types.StringType )
                except AssertionError :
                    self.__elementsError
                    return None
        return True
    
    def __elementsError( self ) :
        print "ERROR: Specify either a string representing a detector element"
        print "       or a list of strings representing a list of detector elements"
        
    def __append( self,  elements, dofs ) :          
        if dofs :
            tmp = self.__addDoFs( elements, dofs )
            self.extend( tmp )
        else :
            if type( elements ) == types.ListType :
                self.extend( elements )
            elif type( elements ) == types.StringType :
                self.append( elements )
            
    ## Some predefined alignables
    def Velo( self, dofs = "" ) :
        self.__append( self.m_velo, dofs )

    def TT( self, dofs = "" ) :
        self.__append( self.m_tt, dofs )
            
    def IT( self, dofs = "" ) :
        self.__append( self.m_it, element )

    def OT( self, dofs = "" ) :
        self.append( self.m_ot, dofs )
    
    def OTStations( self, dofs = "" ) :
        elements = []
        for i in range( len( self.m_stations ) ) :
            elements.append( self.m_ot + self.m_stations[ i ] )
            
        self.__append( elements, dofs )
        
                
    def OTLayers( self, dofs = "" ) :
        elements = []
        for i in range( len( self.m_stations ) ) :
            for j in range( len( self.m_layers ) ) :
                elements.append( self.m_ot + self.m_stations[ i ] + self.m_layers[j] )
           
        self.__append( elements, dofs )    

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
           
        self.__append( elements, dofs )    
        
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
            
        self.__append( elements, dofs )
            

    
