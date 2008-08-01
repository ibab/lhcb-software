import types

class Alignables( list ):
    
    def __init__( self, elements = None, dofs = "" ) :        
        self.m_dofs       = [ "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" ]
        self.m_velo       = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
        self.m_veloLeft   = "/VeloLeft"
        self.m_veloRight  = "/VeloRight"
        
        self.m_tt         = "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
        
        self.m_it         = "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
        self.m_itStations = [ "/Station1", "/Station2", "/Station3" ]
        self.m_itBoxes    = [ "/TopBox", "/BottomBox", "/ASideBox", "/CSideBox" ]
        self.m_itLayers   = [ "/LayerX1", "/LayerU", "/LayerV", "/LayerX2" ]
        self.m_itLadders  = [ "/Ladder1", "/Ladder2", "/Ladder3", "/Ladder4",
                              "/Ladder5", "/Ladder6", "/Ladder7" ]

        self.m_ot         = "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        self.m_otStations = [ "/T1", "/T2", "/T3" ]
        self.m_otLayers   = [ "/X1layer", "/Ulayer", "/Vlayer", "/X2layer" ]
        self.m_otQuarters = [ "/Quarter(0|2)", "/Quarter(1|3)" ]
        self.m_otModules  = [ "/Module1", "/Module2", "/Module3", "/Module4", "/Module5",
                              "/Module6", "/Module7", "/Module8", "/Module9" ]
        if elements and self.__validElements( elements ) :    
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
            self.__elementsError( elements )
            return None
        
        ## Paranoia; extend also throws an exception
        if type( elements ) == types.ListType :
            for i in range( len( elements ) ) :
                try:
                    assert( type( elements[ i ] ) == types.StringType )
                except AssertionError :
                    self.__elementsError( elements )
                    return None
        return True
    
    def __elementsError( self, elements ) :
        print "ERROR: Specify either a string representing a detector element"
        print "       or a list of strings representing a list of detector elements"
        print "elements = ",elements
        
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

    ## VELO ###############################################################################
    def Velo( self, dofs = "" ) :
        self.__append( self.m_velo, dofs )

    ## TODO: Need to make the following more generic
    def VeloRSensors( self, dofs = "" ) :
        elements = []
        elements.append( self.m_velo + "/Velo(Left|Right)/Module(00|01|04|05|08|09|12|13|16|17|20|21|24|25|28|29|32|33|36|37|40|41)/RPhiPair.{1,2}/Detector-00" )
        elements.append( self.m_velo + "/Velo(Left|Right)/Module(02|03|06|07|10|11|14|15|18|19|22|23|26|27|30|31|34|35|38|39)/RPhiPair.{1,2}/Detector-01" )
        self.__append( elements, dofs )

    def VeloPhiSensors( self, dofs = "" ) :
        elements = []
        elements.append( self.m_velo + "/Velo(Left|Right)/Module(00|01|04|05|08|09|12|13|16|17|20|21|24|25|28|29|32|33|36|37|40|41)/RPhiPair.{1,2}/Detector-01" )
        elements.append( self.m_velo + "/Velo(Left|Right)/Module(02|03|06|07|10|11|14|15|18|19|22|23|26|27|30|31|34|35|38|39)/RPhiPair.{1,2}/Detector-00" )
        self.__append( elements, dofs )
        
    def VeloModules( self, dofs = "" ) :
        elements = []
        elements.append( self.m_velo + "/Velo(Left|Right)/Module.{1,2}" )
        self.__append( elements, dofs )
        
    def VeloPileUpSensors( self, dofs = "" ) :
        elements = []
        elements.append( self.m_velo + "/Velo(Left|Right)/Module.{1,2}/RPhiPair.{1,2}/DetectorPU.{1,5}" )
        self.__append( elements, dofs )
        
    def VeloRightRSensors( self, dofs = "" ) :
        self.__append( self.m_velo + self.m_veloRight + "/Module(00|01|04|05|08|09|12|13|16|17|20|21|24|25|28|29|32|33|36|37|40|41)/RPhiPair.{1,2}/Detector-00", dofs)

    def VeloRight( self, dofs = "" ) :
        self.__append( self.m_velo + self.m_veloRight, dofs)
        
    def VeloLeft( self, dofs = "" ) :
        self.__append( self.m_velo + self.m_veloLeft, dofs)

    ## TT #################################################################################
    def TT( self, dofs = "" ) :
        self.__append( self.m_tt, dofs )

    def TTLayers( self, dofs = "" ) :
        self.__append( self.m_tt + "/TT./.{4}Layer", dofs )

    def TTHalfModules( self, dofs = "" ) :
        self.__append( self.m_tt + "/TT./.{4}Layer/.{2}Module.{2}", dofs )

    ## IT #################################################################################
    def IT( self, dofs = "" ) :
        self.__append( self.m_it, dofs )

    def ITBoxes( self, dofs = "" ) :
        elements = []
        for i in self.m_itStations :
            for j in self.m_itBoxes :
                elements.append( self.m_it + i + j )
        self.__append( elements, dofs )
        
    def ITLayers( self, dofs = "" ) :
        elements = []
        for i in self.m_itStations :
            for j in self.m_itBoxes :
                for k in self.m_itLayers :
                    elements.append( self.m_it + i + j + k )
        self.__append( elements, dofs )
        
    def ITLadders( self, dofs = "" ) :
        elements = []
        for i in self.m_itStations :
            for j in self.m_itBoxes :
                for k in self.m_itLayers :
                    for l in self.m_itLadders :
                        elements.append( self.m_it + i + j + k + l )
        self.__append( elements, dofs )


    ## OT #################################################################################
    def OT( self, dofs = "" ) :
        self.__append( self.m_ot, dofs )
    
    def OTStations( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            elements.append( self.m_ot + i )
        self.__append( elements, dofs )
        
                
    def OTLayers( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            for j in self.m_otLayers :
                elements.append( self.m_ot + i + j )
        self.__append( elements, dofs )    

    def OTModules( self, dofs = "" ) :
        elements = []
        ## 3 Stations numbered from 1 to 3
        for i in self.m_otStations :
            ## There are four layers X1, U, V and X2
            for j in self.m_otLayers :
                ## Quarters 0 and 2 (C-Side) and Quarters 1 and 3 (A-side)
                for k in self.m_otQuarters :
                    ## Nine modules numbered from 1 to 9
                    for l in self.m_otModules :
                        elements.append( "Group : " + self.m_ot + i + j + k + l )
        self.__append( elements, dofs )    
        
    def OTCFramesCSide( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            ## C-side X1 and U
            elements.append( "Group : " + station + "/(X1|U)layer" + self.m_otQuarters[0] + "/Module." )
            ## C-side V and X2
            elements.append( "Group : " + station + "/(V|X2)layer" + self.m_otQuarters[0] + "/Module." )
        self.__append( elements, dofs )

    def OTLayersCSide( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            for j in self.m_otLayers :
                ## C-side layers
                elements.append( "Group : " + station + j + self.m_otQuarters[0] + "/Module." )
        self.__append( elements, dofs )
        
    def OTCFramesASide( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            ## A-side X1 and U
            elements.append( "Group : " + station + "/(X1|U)layer" + self.m_otQuarters[1] + "/Module." )
            ## A-side V and X2
            elements.append( "Group : " + station + "/(V|X2)layer" + self.m_otQuarters[1] + "/Module." )
        self.__append( elements, dofs )

    ## OTHER ##############################################################################
    def ITLaddersOTModules( self, dofs = "" ) :
        self.ITLadders( dofs )
        self.OTModules( dofs )
        
