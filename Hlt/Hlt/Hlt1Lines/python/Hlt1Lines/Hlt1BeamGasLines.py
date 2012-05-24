# =============================================================================
## @file
#  Configuration of BeamGas Lines
#  @author Jaap Panman jaap.panman@cern.ch
#  @date 2009-02-25
#  @author Plamen Hopchev phopchev@cern.ch
#  @date 2011-03-01
# =============================================================================
"""
 script to configure BeamGas trigger lines
"""
# =============================================================================
__author__  = "Jaap Panman jaap.panman@cern.ch"
__author__  = "Plamen Hopchev phopchev@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.25 $"
# =============================================================================

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser



class Hlt1BeamGasLinesConf(HltLinesConfigurableUser) :

    # steering variables
    __slots__ = {

                ### Set the tracking alg and hit manager
                  'TrackingConf'             : 'FastVelo' # can be 'FastVelo' or 'PatVelo'
                , 'FastVeloBGHitManagerName' : 'FastVeloBeamGasHitManager'

                ### Track Container Names
                , 'RZTrackContName': 'Hlt/Track/BeamGasRZ'
                , 'TrackContName'  : 'Hlt/Track/BeamGasAll'
                , 'VertexContName' : 'Hlt/Vertex/BeamGasAll'

                ### Constituents of the names of the HLT Lines
                , 'LineNameParts' : { 'ee' : ['BeamGas','NoBeam']
                                    , 'be' : ['BeamGas','']
                                    , 'bbenhanced'  : ['BeamGasCrossing','Enhanced']
                                    , 'bbforced'    : ['BeamGasCrossing','ForcedReco']
                                    , 'bbparasitic' : ['BeamGasCrossing','Parasitic']
                                    , 'highrho'     : ['BeamGas', 'HighRhoVertices']
                                    }

                ### ODIN rate limits for the high-rho lines
                , 'ODINLimitHighRho' : { 'HighRhoBB'    : 'SCALE(0.)'  #e.g. 'SCALE(0.1)' or 'RATE(50000.0)'
                                       , 'HighRhoNonBB' : 'SCALE(1.0)'
                                       }

                ### L0DU requirements
                , 'L0Filter' : { 'NoBeamBeam1'   : "L0_CHANNEL('B1gas')"
                               , 'NoBeamBeam2'   : "L0_CHANNEL('B2gas')"
                               , 'Beam1'         : "L0_CHANNEL('B1gas')"
                               , 'Beam2'         : "L0_CHANNEL('B2gas')"
                               , 'EnhancedBeam1' : "L0_CHANNEL('B1gas')"
                               , 'EnhancedBeam2' : "L0_CHANNEL('B2gas')"
                               , 'BB'            : "|".join( [ "(L0_DATA('Spd(Mult)') > 5)" , "(L0_DATA('PUHits(Mult)') > 5)" ] )
                               }

                ### Rate Limits of events passing the L0DU requirements
                , 'L0RateLimit' : { 'NoBeamBeam1'    : 1.e4 #'SCALE(0.1)'
                                  , 'NoBeamBeam2'    : 1.e4 #'SCALE(0.1)'
                                  , 'Beam1'          : 5.e3
                                  , 'Beam2'          : 5.e3
                                  , 'EnhancedBeam1'  : 5.e3
                                  , 'EnhancedBeam2'  : 5.e3
                                  , 'ForcedReco'     : 1.e3
                                  , 'ForcedRecoFullZ': 1.e3
                                  , 'Parasitic'      : 5.e4
                                  }

                ### z-ranges for Tracking
                , 'TrackingZMin'            : -2000.
                , 'TrackingZMax'            :  2000.

                ### z-ranges for Vertexing
                , 'Beam1VtxRangeLow'        : -2000.
                , 'Beam1VtxRangeUp'         :   400.
                , 'Beam2VtxRangeLow'        :     0.
                , 'Beam2VtxRangeUp'         :  2000.
                , 'BGVtxExclRangeMin'       :  -300.      # These 2 options take effect
                , 'BGVtxExclRangeMax'       :   300.      # only for the bb Lines

                ### Proto-Vertexing Settings  

                ### PV3D vertex cuts  
                , 'VertexMinNTracks'        :    '9'
                , 'HighRhoVerticesMinNTracks':   '9'
                , 'VertexMaxChi2PerDoF'     : '9999.'
                , 'VertexCutSmallRho'       : '(VX_BEAMSPOTRHO( 6*mm ) < 4*mm)'
                , 'VertexCutHighRho'        : '(VX_BEAMSPOTRHO( 6*mm ) > 4*mm)'

                ### Global Event Cuts
                , 'UseGEC'                  : 'Loose' # 'None' / 'Loose' / 'Tight' (defined in Hlt1GECs.py)

                ### Input Prescales
                , 'Prescale'                : { 'Hlt1BeamGasNoBeamBeam1'             : 1.
                                              , 'Hlt1BeamGasNoBeamBeam2'             : 1.
                                              , 'Hlt1BeamGasBeam1'                   : 1.
                                              , 'Hlt1BeamGasBeam2'                   : 1.
                                              , 'Hlt1BeamGasCrossingEnhancedBeam1'   : 1.
                                              , 'Hlt1BeamGasCrossingEnhancedBeam2'   : 1.
                                              , 'Hlt1BeamGasCrossingForcedReco'      : 1.
                                              , 'Hlt1BeamGasCrossingForcedRecoFullZ' : 1.
                                              , 'Hlt1BeamGasCrossingParasitic'       : 1.
                                              , 'Hlt1BeamGasHighRhoVertices'         : 1.
                                              }
                ### Output Postscales  
                , 'Postscale'               : { 'Hlt1BeamGasNoBeamBeam1'             : 'RATE(5)'
                                              , 'Hlt1BeamGasNoBeamBeam2'             : 'RATE(5)'
                                              , 'Hlt1BeamGasBeam1'                   : 'RATE(20)'
                                              , 'Hlt1BeamGasBeam2'                   : 'RATE(20)'
                                              , 'Hlt1BeamGasCrossingEnhancedBeam1'   : 'RATE(20)'
                                              , 'Hlt1BeamGasCrossingEnhancedBeam2'   : 'RATE(20)'
                                              , 'Hlt1BeamGasCrossingForcedReco'      : 'RATE(5)'
                                              , 'Hlt1BeamGasCrossingForcedRecoFullZ' : 'RATE(5)'
                                              , 'Hlt1BeamGasCrossingParasitic'       : 'RATE(5)'
                                              , 'Hlt1BeamGasHighRhoVertices'         : 'RATE(5)'
                                              }
                } 


    def GetGEC(self):
        ''' Get the GEC Algorithm. Need to return a list! '''
        reqGEC = self.getProp('UseGEC')
        if reqGEC in ['Loose', 'Tight']:
            from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
            return [ Hlt1GECUnit( gec=reqGEC ) ]
        else:
            if reqGEC != 'None': print "\nError : Unknown Value of property 'UseGEC'. No GEC will be set ...\n"
            return []

    def GetVeloTrackingAlg(self, TrackingAlgType, OutTracksName):
        ''' Creates common Velo tracking instance. TrackingAlgType can be FastVelo or PatVelo (R+Space+General). '''
        trackingAlgs = []
        if TrackingAlgType == 'FastVelo':
            from Configurables import FastVeloTracking
            algTr = FastVeloTracking('FastVeloBeamGasFullTracking', OutputTracksName = OutTracksName, HitManagerName = self.getProp("FastVeloBGHitManagerName") )
            algTr.ZVertexMin = self.getProp("TrackingZMin")
            algTr.ZVertexMax = self.getProp("TrackingZMax")
            trackingAlgs.append(algTr)
        elif TrackingAlgType == 'PatVelo':
            from Configurables import Tf__PatVeloRTracking, Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool, Tf__PatVeloGeneralTracking
            patVeloR = Tf__PatVeloRTracking('PatVeloBeamGas_RTracking')
            patVeloR.OutputTracksName = self.getProp('RZTrackContName')
            patVeloR.ZVertexMin = self.getProp("TrackingZMin")
            patVeloR.ZVertexMax = self.getProp("TrackingZMax")
            patVeloSpace = Tf__PatVeloSpaceTracking('PatVeloBeamGas_SpaceTracking')
            patVeloSpace.InputTracksName  = patVeloR.OutputTracksName
            patVeloSpace.OutputTracksName = OutTracksName
            patVeloSpace.addTool(Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool")
            patVeloSpace.PatVeloSpaceTool.MarkClustersUsed=True
            patVeloGeneral = Tf__PatVeloGeneralTracking('PatVeloBeamGas_GeneralTracking')
            patVeloGeneral.OutputTracksLocation = OutTracksName
            patVeloGeneral.PointErrorMin = 2
            trackingAlgs.extend( [patVeloR, patVeloSpace, patVeloGeneral] )
        else:
            print "FATAL ERROR : unknown TrackingAlgType!!!"
        return trackingAlgs

    def HelperBeamsZRange(self, whichBeam):
        ''' Function returning the z-range limits to be
            used by the [Proto]Vertexing alogirthms '''
        if not whichBeam:
            zMin = self.getProp('Beam1VtxRangeLow')
            zMax = self.getProp('Beam2VtxRangeUp')
        elif whichBeam in ['Beam1', 'Beam2']:
            zMin = self.getProp(whichBeam+'VtxRangeLow')
            zMax = self.getProp(whichBeam+'VtxRangeUp')
        else:
            print "\nError : Please specify correct argument for function 'HelperBeamsZRange'\n"
            zMin, zMax = -9999., -9999.
        return zMin, zMax

    def GetProtoVertexingAlg( self, lineName, InpTracksName, whichBeam=0 ):
        zMin, zMax = self.HelperBeamsZRange(whichBeam)
        from Configurables import BeamGasProtoVertex
        protoVtxAlgo = BeamGasProtoVertex( 'BGProtoVertex_' + lineName
                       , InputSelection     = 'TES:%s' % InpTracksName
                       , zTracksMin         = zMin
                       , zTracksMax         = zMax
                       , zTracksExcludeLumiRegionLow = self.getProp("BGVtxExclRangeMin") if ("Cross" in lineName) else 0.
                       , zTracksExcludeLumiRegionUp  = self.getProp("BGVtxExclRangeMax") if ("Cross" in lineName) else 0.
                       , MinimumNumberOfTracks = self.getProp("VertexMinNTracks")
                       )           
        return protoVtxAlgo

    def GetPV3DAlg(self, InpTracksName, OutVerticesName):
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter
        myPV3D = PatPV3D('PV3DHltBeamGas' + OutVerticesName[-3:])
        myPV3D.addTool(PVOfflineTool,'PVOfflineTool')
        ## seed tool
        myPV3D.PVOfflineTool.PVSeedingName = 'PVSeed3DTool'
        myPV3D.PVOfflineTool.addTool( PVSeed3DTool, name = 'PVSeed3DTool')
        myPV3D.PVOfflineTool.PVSeed3DTool.TrackPairMaxDistance = 2. # dflt 0.3
        myPV3D.PVOfflineTool.PVSeed3DTool.zMaxSpread = 10. # dflt 3.
        myPV3D.PVOfflineTool.PVSeed3DTool.MinCloseTracks = 1 # dflt 4
        ## fitter tool
        myPV3D.PVOfflineTool.PVFitterName = 'LSAdaptPV3DFitter'
        myPV3D.PVOfflineTool.addTool(LSAdaptPV3DFitter, 'LSAdaptPV3DFitter')
        myPV3D.PVOfflineTool.LSAdaptPV3DFitter.maxIP2PV = 4. # dflt 2.
        myPV3D.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2. #dflt 1.
        ##
        myPV3D.PVOfflineTool.InputTracks = [InpTracksName]
        myPV3D.OutputVerticesName = OutVerticesName
        return myPV3D

    def GetVertexFilterAlg(self, lineName, InpVerticesName):
        ''' Create filters for the vertices produced by PV3D '''
        ### To Do:
        #!!! Add a cut on the track asymmetry? --> this can help us in the case of pp collisions in ee, be or eb crossings
        #!!! Add a cut on z_vtx to get rid of Offest Collisions at +/- 75 cm 

        if   'Beam1' in lineName: zMin, zMax = self.HelperBeamsZRange('Beam1')
        elif 'Beam2' in lineName: zMin, zMax = self.HelperBeamsZRange('Beam2')
        else:                     zMin, zMax = self.HelperBeamsZRange('')

        ### One object cuts ###
        vtxCut_zPos           = "in_range(%s*mm, VZ, %s*mm)" %(zMin, zMax)
        vtxCut_maxChi2PerDoF  = "(VCHI2PDOF < %s)"% self.getProp("VertexMaxChi2PerDoF") if self.getProp("VertexMaxChi2PerDoF") < 998 else ""
        vtxCut_minNTracks     = "(NTRACKS > %s)"  % self.getProp("VertexMinNTracks") if self.getProp("VertexMinNTracks") > 0 else ""
        vtxCut_smallRho = self.getProp('VertexCutSmallRho')
        listVertexCuts = [ vtxCut_zPos, vtxCut_maxChi2PerDoF, vtxCut_minNTracks, vtxCut_smallRho ]

        #in case of a bb line add the cut that vetoes the Lumi Region
        vtxCut_excludeLumiReg = "~in_range(%s*mm, VZ, %s*mm)" %(self.getProp("BGVtxExclRangeMin"), self.getProp("BGVtxExclRangeMax"))
        if "Cross" in lineName:
            if "FullZ" not in lineName: listVertexCuts.append(vtxCut_excludeLumiReg)

        ### The special case for the high-rho vertices Line
        if "HighRho" in lineName:
            vtxCut_minNTracks = "(NTRACKS > %s)"  % self.getProp("HighRhoVerticesMinNTracks") if self.getProp("HighRhoVerticesMinNTracks") > 0 else ""
            vtxCut_highRho    = self.getProp('VertexCutHighRho')
            listVertexCuts = [ vtxCut_zPos, vtxCut_maxChi2PerDoF, vtxCut_minNTracks, vtxCut_highRho ]

        ### The LoKi vertex filtering code
        codeLoKiFilter = "VSOURCE('%s', %s) >> ~VEMPTY" %(InpVerticesName, " & ".join([ c for c in listVertexCuts if c ]))
        
        ### The last algorithm should have name of line, plus 'Decision'
        from Configurables import LoKi__VoidFilter
        return LoKi__VoidFilter( 'Hlt1%sDecision' % lineName
                               , Preambulo = ['from LoKiPhys.decorators import *','from LoKiCore.functions import *']
                               , Code = codeLoKiFilter
                               )


    ############################################################################
    ### Function creating the ee, be, lonely and forced lines
    ############################################################################
    def __create_nonParasitic_line__(self, whichLine, whichBeam) :
        ''' Function to create the Lines for 'ee'(2), 'be'(1), 'eb'(1), 
            'lonely bb'(2) and 'forced bb'(2) crossings. Algorithms:
            1) FastVeloTracking with enlarged z-window (the same algorithm for all lines)
            2) PV3D (the same algorithm for all lines)
            3) Loki-Filter the produced vertices (Algo name should be <name of line> + Decision)
            In Algos 2) and 3) need to exclude lumi region for the 'lonely bb' and 'forced bb' cases
        '''

        nameParts = self.getProp("LineNameParts")[whichLine]
        nameKey   = nameParts[1]+whichBeam
        lineName  = nameParts[0]+nameKey
            
        ### Get the L0 Rate Limit
        L0RateLimit = self.getProp('L0RateLimit')[nameKey] if (nameKey in self.getProp('L0RateLimit').keys()) else None

        ### Create the L0DU and ODIN filters
        if 'ForcedReco' in nameParts[1]:
            l0du = self.getProp('L0Filter')['BB']
            odin = '(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'
            #reject lumi events for the FullZ line
            if whichBeam=='FullZ': odin += ' & (ODIN_TRGTYP != LHCb.ODIN.LumiTrigger)'
        elif 'HighRho' in nameParts[1]:
            l0du = self.getProp('L0Filter')['BB']
            reqBB    = '(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'
            reqNonBB = '((ODIN_BXTYP == LHCb.ODIN.Beam1) | (ODIN_BXTYP == LHCb.ODIN.Beam2))'
            rateLimBB    = self.getProp('ODINLimitHighRho')['HighRhoBB']
            rateLimNonBB = self.getProp('ODINLimitHighRho')['HighRhoNonBB']
            odin = '(scale(%s, %s) | scale(%s, %s))' %(reqBB, rateLimBB, reqNonBB, rateLimNonBB)
        else:
            l0du = self.getProp('L0Filter')[nameKey] if (nameKey in self.getProp('L0Filter').keys()) else None
            if 'NoBeam' == nameParts[1]:
                odin = '(ODIN_BXTYP == LHCb.ODIN.NoBeam)'
            elif '' == nameParts[1]:
                odin = '(ODIN_BXTYP == LHCb.ODIN.'+whichBeam+')'
            elif 'Enhanced' == nameParts[1]:
                odin = '(ODIN_TRGTYP == LHCb.ODIN.BeamGasTrigger) & (ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'
            else:
                print "\n\nFATAL ERROR - Can't set ODIN Filter\n\n"

        ### Get the tracking algo(s)
        algTracking = self.GetVeloTrackingAlg(self.getProp("TrackingConf"), self.getProp("TrackContName"))
        
        ### Get the PV3D algo
        algPV3D = self.GetPV3DAlg( self.getProp("TrackContName"), self.getProp("VertexContName") )

        ### Get the vertex filtering algo
        ### This is the last algorithm - should have name of the line, plus 'Decision'
        algVertexFilter = self.GetVertexFilterAlg( lineName, algPV3D.OutputVerticesName )

        ''' DEBUG
        print "\n", "@"*100
        print "nameParts = ", nameParts
        print "nameKey = ", nameKey
        print "lineName = ", lineName
        print "L0 requirement = ", l0du
        print "L0RateLim = ", L0RateLimit
        print "ODIN = ", odin
        print "@"*100, "\n"
        '''

        ### Get the Global Event Cuts
        algGEC = self.GetGEC()

        ### Finally, create the Hlt1Line
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltDecodeRaw import DecodeVELO

        TheLine = Line( lineName
                      , priority = None
                      , prescale = self.prescale
                      , ODIN  = odin
                      , L0DU  = 'scale( %s, RATE(%s) )' % (l0du, L0RateLimit) if L0RateLimit else l0du
                      , algos = algGEC + [ DecodeVELO ] + algTracking + [ algPV3D, algVertexFilter ]
                      , postscale = self.postscale
                      )
 
        return TheLine

    ############################################################################
    ### Function creating the parastic bb line
    ############################################################################
    def __create_parasitic_line__(self):
        ''' Function to create the Parasitic line for bb crossings. Algorithms:
            1) Check if Velo tracks have already been reconstructed
            2) Run the proto-vertexing algorithm (excluding the lumi region)
            3) Run Full Fast Velo tracking (the same instance as in the non-parasitic lines)
            4) PV3D (the same instance as in the non-parasitic lines)
            5) Filter the produced vertices
        '''
        
        nameParts = ['BeamGasCrossing', 'Parasitic']
        nameKey   = nameParts[1]
        lineName  = nameParts[0]+nameKey

        ### Get the L0 Rate Limit
        L0RateLimit = self.getProp('L0RateLimit')[nameKey] if (nameKey in self.getProp('L0RateLimit').keys()) else None

        ### Create the L0DU and ODIN filters
        l0du = self.getProp('L0Filter')['BB']
        odin = "(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)"

        ### Algorithm to check for existing Velo tracks
        from HltTracking.HltReco import MinimalVelo
        from Configurables import LoKi__VoidFilter as VoidFilter
        algCheckTracks = VoidFilter('Hlt1BeamGasCheckVeloTracks' , Code = "CONTAINS('%s') > 0" % MinimalVelo.outputSelection() )

        ### ProtoVertexing algorithm
        algProtoVertex = self.GetProtoVertexingAlg( lineName, MinimalVelo.outputSelection() )

        ### Get the tracking algo(s)
        algTracking = self.GetVeloTrackingAlg(self.getProp("TrackingConf"), self.getProp("TrackContName"))

        ### Get the PV3D algo
        algPV3D = self.GetPV3DAlg( self.getProp("TrackContName"), self.getProp("VertexContName") )

        ### Get the vertex filtering algo
        ### This is the last algorithm - should have name of the line, plus 'Decision'
        algVertexFilter = self.GetVertexFilterAlg( lineName, algPV3D.OutputVerticesName )

        ''' DEBUG
        print "\n", "%"*100
        print "nameParts = ", nameParts
        print "nameKey = ", nameKey
        print "lineName = ", lineName
        print "L0 requirement = ", l0du
        print "L0RateLim = ", L0RateLimit
        print "ODIN = ", odin
        print "%"*100, "\n"
        '''

        ### Get the Global Event Cuts
        algGEC = self.GetGEC()

        ### Finally, create the Hlt1Line
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltDecodeRaw import DecodeVELO

        TheLine = Line( lineName
                      , priority = 200 
                      , prescale = self.prescale
                      , ODIN  = odin
                      , L0DU  = 'scale( %s, RATE(%s) )' % (l0du, L0RateLimit) if L0RateLimit else l0du
                      , algos = algGEC + [ DecodeVELO, algCheckTracks, algProtoVertex ] + algTracking + [ algPV3D, algVertexFilter ]
                      , postscale = self.postscale
                      )
 
        return TheLine

    ############################################################################
    ### Create the BeamGas Hlt Lines
    ############################################################################
    def __apply_configuration__(self) : 
        self.__create_nonParasitic_line__('ee', 'Beam1') #line name = 'Hlt1BeamGasNoBeamBeam1'
        self.__create_nonParasitic_line__('ee', 'Beam2')
        self.__create_nonParasitic_line__('be', 'Beam1') #line name = 'Hlt1BeamGasBeam1'
        self.__create_nonParasitic_line__('be', 'Beam2')
        self.__create_nonParasitic_line__('bbenhanced', 'Beam1') #line name = 'Hlt1BeamGasCrossingEnhancedBeam1'
        self.__create_nonParasitic_line__('bbenhanced', 'Beam2')
        self.__create_nonParasitic_line__('bbforced', '') #line name = 'Hlt1BeamGasCrossingForcedReco'
        self.__create_nonParasitic_line__('bbforced', 'FullZ') #line name = 'Hlt1BeamGasCrossingForcedRecoFullZ'
        self.__create_nonParasitic_line__('highrho', '') #line name = Hlt1BeamGasHighRhoVertices
        self.__create_parasitic_line__() #line name = 'Hlt1BeamGasCrossingParasitic'
