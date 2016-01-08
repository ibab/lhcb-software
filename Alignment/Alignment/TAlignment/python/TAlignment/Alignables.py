import types

class Alignables( list ):
    
    def __init__( self, elements = None, dofs = "" ) :        
        self.m_dofs       = [ "Tx", "Ty", "Tz", "Rx", "Ry", "Rz", "None" ]
        self.m_velo       = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
        self.m_veloLeft   = "/VeloLeft"
        self.m_veloRight  = "/VeloRight"

        self.m_vp         = "/dd/Structure/LHCb/BeforeMagnetRegion/VP"
        self.m_vpLeft   = "/VPLeft"
        self.m_vpRight  = "/VPRight"

        self.m_tt         = "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
        self.m_ttStations = [ "TTa","TTb" ]
        self.m_ttLayers   = [ "TTa/TTaXLayer","TTa/TTaULayer","TTb/TTbVLayer","TTb/TTbXLayer", ]
        
        self.m_it         = "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
        self.m_itStations = [ "/Station1", "/Station2", "/Station3" ]
        self.m_itBoxes    = [ "/TopBox", "/BottomBox", "/ASideBox", "/CSideBox" ]
        self.m_itLayers   = [ "/LayerX1", "/LayerU", "/LayerV", "/LayerX2" ]
        self.m_itLadders  = [ "/Ladder1", "/Ladder2", "/Ladder3", "/Ladder4",
                              "/Ladder5", "/Ladder6", "/Ladder7" ]

        self.m_ot         = "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
        self.m_otStations = [ "/T1", "/T2", "/T3" ]
        self.m_otLayers   = [ "/X1", "/U", "/V", "/X2" ]
        self.m_otQuarters = [ "/Q(0|2)", "/Q(1|3)" ]
        self.m_otModules  = [ "/M1", "/M2", "/M3", "/M4", "/M5", "/M6", "/M7", "/M8", "/M9" ]
        self.m_otRealQuarters = [ "/Q0", "/Q1", "/Q2", "/Q3" ]
        
        self.m_muon            = "/dd/Structure/LHCb/DownstreamRegion/Muon"
        self.m_mustations      = ["/M1","/M2","/M3","/M4","/M5"] ###cosmici
        self.m_muhalfstationsAC= ["/M.ASide", "/M.CSide"] 
        self.m_muhalfstations  = ["/M.{1,2}Side"]
        self.m_muchambers      = ["/R.{1,2}Side/Cham.{1,3}"] 

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

    ## Global ###############################################################################         
    def Tracker( self, dofs = "" ) :
        elements = []
        elements.append( "Tracker : " + self.m_velo )
        elements.append( "Tracker : " + self.m_tt )
        elements.append( "Tracker : " + self.m_it )
        elements.append( "Tracker : " + self.m_ot )
        #elements.append( "Tracker : " + self.m_muon )
        self.__append( elements, dofs )

    def TStations( self, dofs = "" ) :
        elements = [ "TStations : /dd/Structure/LHCb/AfterMagnetRegion/T/(I|O)T" ]
        self.__append( elements, dofs )

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

    def VeloLeftModules( self, dofs = "" ) :
        elements = []
        elements.append( self.m_velo + "/VeloLeft/Module.{1,2}" )
        self.__append( elements, dofs )

    def VeloRightModules( self, dofs = "" ) :
        elements = []
        elements.append( self.m_velo + "/VeloRight/Module.{1,2}" )
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

    ## VP #################################################################################
    def VPModules( self, dofs = "" ) :
        elements = []
        elements.append( self.m_vp + "/VP(Right|Left)/Module.{1,2}WithSupport" )
        self.__append( elements, dofs )
        
    def VPRight( self, dofs = "" ) :
        self.__append( self.m_vp + self.m_vpRight, dofs)
        
    def VPLeft( self, dofs = "" ) :
        self.__append( self.m_vp + self.m_vpLeft, dofs)
    ## TT #################################################################################
    def TT( self, dofs = "" ) :
        self.__append( self.m_tt, dofs )

    def TTLayers( self, dofs = "" ) :
        self.__append( self.m_tt + "/TT./.{4}Layer", dofs )

    def TTHalfLayers( self, dofs = "" ) :
        elements = []
        # we need to do TTa and TTb seperately since they are actually different
        ttalayers = ['TTaXLayer','TTaULayer']
        for layername in ttalayers:
            # A-side
            elements.append( layername + "ASide : "  +
                             self.m_tt + "/TTa/" + layername + "/R(3Module.{2}|2Module(3.|2T))" )
            # C-side
            elements.append( layername + "CSide : "  +
                             self.m_tt + "/TTa/" + layername + "/R(1Module.{2}|2Module(1.|2B))" )
        ttblayers = ['TTbVLayer','TTbXLayer']
        for layername in ttblayers:
            # A-side
            elements.append( layername + "ASide : "  +
                             self.m_tt + "/TTb/" + layername + "/R(3Module.{2}|2Module(3B|4.|5.))" )
            # C-side
            elements.append( layername + "CSide : "  +
                             self.m_tt + "/TTb/" + layername + "/R(1Module.{2}|2Module(1.|2.|3T))" )
        self.__append( elements, dofs )

    def TTSplitLayers( self, dofs = "" ) :
        elements = []
        ttalayers = ['TTaXLayer','TTaULayer']
        for layername in ttalayers:
            elements.append( layername + "LowZ : "  +
                             self.m_tt + "/TTa/" + layername + "/R(1Module(1|3|5)|2Module(1|3)|3Module(2|4|6)).")
            elements.append( layername + "HighZ : "  +
                             self.m_tt + "/TTa/" + layername + "/R(1Module(2|4|6)|2Module2|3Module(1|3|5))." )
        ttblayers = ['TTbVLayer','TTbXLayer']
        for layername in ttblayers:
            elements.append( layername + "LowZ : "  +
                             self.m_tt + "/TTb/" + layername + "/R(1Module(1|3|5)|2Module(1|3|5)|3Module(2|4|6)).")
            elements.append( layername + "HighZ : "  +
                             self.m_tt + "/TTb/" + layername + "/R(1Module(2|4|6)|2Module(2|4)|3Module(1|3|5))." )
        self.__append( elements, dofs )
      
    def TTBoxes( self, dofs = "" ) :
        elements = []
        elements.append( "TTASide : "  +
                         self.m_tt + "/TTa/TTa.Layer/R(3Module.{2}|2Module(3.|2T))" )
        elements.append( "TTCSide : "  +
                         self.m_tt + "/TTa/TTa.Layer/R(1Module.{2}|2Module(1.|2B))" )
        elements.append( "TTASide : "  +
                         self.m_tt + "/TTb/TTb.Layer/R(3Module.{2}|2Module(3B|4.|5.))")
        elements.append( "TTCSide : "  +
                         self.m_tt + "/TTb/TTb.Layer/R(1Module.{2}|2Module(1.|2.|3T))" )
        self.__append( elements, dofs )

    def TTHalfModules( self, dofs = "" ) :
        self.__append( self.m_tt + "/TT./.{4}Layer/.{2}Module.{2}", dofs )

    def TTSensors( self, dofs = "" ) :
        self.__append( self.m_tt + "/TT./.{4}Layer/.{2}Module.{2}/Ladder./Sensor.{2}", dofs )

    def TTLongModules( self, dofs = "" ) :
        elements = []
        for layer in ['TTaXLayer','TTaULayer']:
            # first R1 and R3
            for i in range(1,7):
                elements.append( layer + "R1Module" + str(i) + " : " + self.m_tt + "/TTa/"+layer+"/R1Module"+str(i)+"." )
                elements.append( layer + "R3Module" + str(i) + " : " + self.m_tt + "/TTa/"+layer+"/R3Module"+str(i)+"." )
            # now R2
            for i in [1,3]:
                elements.append( layer + "R2Module" + str(i) + " : " + self.m_tt + "/TTa/"+layer+"/R2Module"+str(i)+"." )
        for layer in ['TTbVLayer','TTbXLayer']:
            # first R1 and R3
            for i in range(1,7):
                elements.append( layer + "R1Module" + str(i) + " : " + self.m_tt + "/TTb/"+layer+"/R1Module"+str(i)+"." )
                elements.append( layer + "R3Module" + str(i) + " : " + self.m_tt + "/TTb/"+layer+"/R3Module"+str(i)+"." )
            # now R2 
            for i in [1,2,4,5]:
                elements.append( layer + "R2Module" + str(i) + " : " + self.m_tt + "/TTb/"+layer+"/R2Module"+str(i)+"." )
        self.__append( sorted(elements), dofs )
            
    def TTShortModules( self, dofs = "" ) :
        elements = []
        self.__append( self.m_tt + "/TTa/.{4}Layer/R2Module2.", dofs )
        self.__append( self.m_tt + "/TTb/.{4}Layer/R2Module3.", dofs )
        self.__append( sorted(elements), dofs )
            
    def TTModules( self, dofs = "" ) :
        self.TTLongModules( dofs )
        self.TTShortModules( dofs )
                    
    ## IT #################################################################################
    def IT( self, dofs = "" ) :
        self.__append( self.m_it, dofs )

    def ITStations( self, dofs = "" ) :
        elements = []
        for i in self.m_itStations :
            elements.append( self.m_it + i )
        self.__append( elements, dofs )
        
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
        
    def ITDoubleLayers( self, dofs = "" ) :
        elements = []
        for i in self.m_itStations :
            for j in self.m_itBoxes :
                elements.append( "IT" + i + j + "/LayerX1U" + " : "
                                 + self.m_it + i + j + "/Layer(X1|U)" )
                elements.append( "IT" + i + j + "/LayerVX2" + " : "
                                 + self.m_it + i + j + "/Layer(X2|V)" )
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

    def OTModules( self, dofs = "", modules = None ) :
        if modules == None: modules = self.m_otModules
        elements = []
        ## 3 Stations numbered from 1 to 3
        for i in self.m_otStations :
            ## There are four layers X1, U, V and X2
            for j in self.m_otLayers :
                ## Quarters 0 and 2 (C-Side) and Quarters 1 and 3 (A-side)
                for k in self.m_otQuarters :
                    ## Nine modules numbered from 1 to 9
                    for l in modules :
                        elements.append( 'OT/' + i.strip('/') + j.strip('/') + k.strip('/') + l.strip('/') + " : " + self.m_ot + i + j + k + l )
        self.__append( elements, dofs )    

    def OTHalfModules( self, dofs = "", modules = None ) :
        if modules == None: modules = self.m_otModules
        elements = []
        ## 3 Stations numbered from 1 to 3
        for i in self.m_otStations :
            ## There are four layers X1, U, V and X2
            for j in self.m_otLayers :
                ## Quarters 0 and 2 (C-Side) and Quarters 1 and 3 (A-side)
                for k in self.m_otRealQuarters :
                    ## Nine modules numbered from 1 to 9
                    for l in modules :
                        elements.append( 'OT/' + i.strip('/') + j.strip('/') + k.strip('/') + l.strip('/') + " : " + self.m_ot + i + j + k + l )
        self.__append( elements, dofs )

    def OTShortHalfModules( self, dofs = "" ) :
        self.OTHalfModules( dofs = dofs, modules = [ '/M8', '/M9' ] )
                
    def OTLongModules( self, dofs = "" ) :
        self.OTModules( dofs = dofs, modules = [ '/M1','/M2','/M3','/M4','/M5','/M6','/M7' ] )
                
    def XOTModules( self, dofs = '' ) :
        elements = []
        for i in self.m_otStations :
            ## There are four layers X1, U, V and X2
            for j in [ '/X1', '/X2' ] :
                ## Quarters 0 and 2 (C-Side) and Quarters 1 and 3 (A-side)
                for k in self.m_otQuarters :
                    ## Nine modules numbered from 1 to 9
                    for l in self.m_otModules :
                        elements.append( "Group : " + self.m_ot + i + j + k + l )
        self.__append( elements, dofs )
    
    def StereoOTModules( self, dofs = '' ) :
        elements = []
        for i in self.m_otStations :
            ## There are four layers X1, U, V and X2
            for j in [ '/U', '/V' ] :
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
            # elements.append( "Group : " + station + "/(X1|U)" + self.m_otQuarters[0] + "/M." )
            elements.append( "OT/" + i.strip( '/' ) + "X1UCSide : " + station + "/(X1|U)" + self.m_otQuarters[0] )
            ## C-side V and X2
            # elements.append( "Group : " + station + "/(V|X2)" + self.m_otQuarters[0] + "/M." )
            elements.append( "OT/" + i.strip( '/' ) + "VX2CSide : " + station + "/(V|X2)" + self.m_otQuarters[0] )
        self.__append( elements, dofs )

    def OTCFramesASide( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            ## A-side X1 and U
            # elements.append( "Group : " + station + "/(X1|U)" + self.m_otQuarters[1] + "/M." )
            elements.append( "OT/" + i.strip( '/' ) + "X1UASide : " + station + "/(X1|U)" + self.m_otQuarters[1] )
            ## A-side V and X2
            # elements.append( "Group : " + station + "/(V|X2)" + self.m_otQuarters[1] + "/M." )
            elements.append( "OT/" + i.strip( '/' ) + "VX2ASide : " + station + "/(V|X2)" + self.m_otQuarters[1] )
        self.__append( elements, dofs )

    def OTCFrames( self, dofs = "" ) :
        self.OTCFramesASide( dofs )
        self.OTCFramesCSide( dofs )

    def OTCFrameLayers( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            # we could also make the c-frame pairs groups of 2
            # layers. however, to be a bit consistent, I prefer to
            # put everything at quarter level.
            elements.append( "OT/" + i.strip( '/' ) + "X1U : " + station + "/(X1|U)/Q." )
            elements.append( "OT/" + i.strip( '/' ) + "VX2 : " + station + "/(V|X2)/Q." )
        self.__append( elements, dofs )
    
    def OTLayersCSide( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            for j in self.m_otLayers :
                ## C-side layers
                elements.append( "Group : " + station + j + self.m_otQuarters[0] + "/M." )
        self.__append( elements, dofs )
        
    def OTLayersASide( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            for j in self.m_otLayers :
                ## C-side layers
                elements.append( "Group : " + station + j + self.m_otQuarters[1] + "/M." )
        self.__append( elements, dofs )

    def OTLayersPP( self, dofs = "" ) :
        elements = []
        for i in self.m_otStations :
            station = self.m_ot + i
            for j in self.m_otLayers :
                ## C-side layers
                elements.append( "Group : " + station + j + "/Q(0|2|1|3)" + "/M." )
        self.__append( elements, dofs )

    ## MUON  ##############################################################################
    def MuonStations( self, dofs = "" ) :
        elements = []
        for i in self.m_mustations :
            elements.append( self.m_muon + i )
        self.__append( elements, dofs )
            
    def MuonHalfStations( self, dofs = "" ) :
        elements = []
        for i in self.m_mustations :
            for j in self.m_muhalfstations :
                    elements.append( self.m_muon +  i + j )
        self.__append( elements, dofs )            

    def MuonChambers( self, dofs = "" ) :
        elements = []
        ## 5 Stations numbered from 1 to 5
        for i in self.m_mustations :
            ## There are 2 halves in each station
            for j in self.m_muhalfstations :
                ## Chambers
                for l in self.m_muchambers :
                    elements.append( self.m_muon + i + j + l )
        self.__append( elements, dofs )

    def MuonHalfStationsCside( self, dofs = "" ) :
        elements = []
        for i in self.m_mustations :
            elements.append( self.m_muon +  i + self.m_muhalfstationsAC[1] )
        self.__append( elements, dofs )

    def MuonHalfStationsAside( self, dofs = "" ) :
        elements = []
        for i in self.m_mustations :
            elements.append( self.m_muon +  i + self.m_muhalfstationsAC[0] )
        self.__append( elements, dofs ) 

      
    ## OTHER ##############################################################################
    def ITLaddersOTModules( self, dofs = "" ) :
        self.ITLadders( dofs )
        self.OTModules( dofs )
        
