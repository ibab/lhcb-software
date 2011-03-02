# =============================================================================
## @file
#  Configuration of BeamGas Lines
#  @author Jaap Panman jaap.panman@cern.ch
#  @date 2009-02-25
#  @author Plamen Hopcheb phopchev@cern.ch
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
                  'TrackingConf'             : 'FastVeloFull' # 'FastVeloFull', 'FastVeloHLT1Only', 'PatVelo'
                , 'FastVeloBGHitManagerName' : 'FastVeloBeamGasHitManager'

                ### Track Container Names
                , 'TrackContName_NonBB'  : 'Hlt/Track/BeamGasAll'
                , 'TrackContName_BB'     : 'Hlt/Track/BeamGasAll'
                #, 'VertexContName_NonBB' : 'Hlt/Vertex/BeamGasNonBB'
                #, 'VertexContName_BB'    : 'Hlt/Vertex/BeamGasBB'
                , 'VertexContName_NonBB' : 'Hlt/Vertex/BeamGasAll' #!!! Review it : if we are going to use the same vertexing algo need to update the function 'GetPV3DAlg'
                , 'VertexContName_BB'    : 'Hlt/Vertex/BeamGasAll'

                ### Constituents of the names of the HLT Lines
                , 'LineNameParts' : { 'ee' : ['BeamGas','NoBeam']
                                    , 'be' : ['BeamGas','']
                                    , 'bblonely' : ['BeamGasCrossing','Lonely']
                                    , 'bbforced' : ['BeamGasCrossing','ForcedRZReco']
                                    , 'bbparasitic' : ['BeamGasCrossing','']
                                    }

                ### L0DU requirements
                , 'L0Channel' : { 'NoBeamBeam1'   : 'B1gas'
                                , 'NoBeamBeam2'   : 'B2gas'
                                , 'Beam1'         : 'B1gas'
                                , 'Beam2'         : 'B2gas'
                                , 'LonelyBeam1'   : 'B1gas'
                                , 'LonelyBeam2'   : 'B2gas'
                                }
                , 'L0FilterBB' : "|".join( [ "(L0_DATA('Spd(Mult)') > 5)" , "(L0_DATA('PUHits(Mult)') > 5)" ] )

                ### Rate Limits of events passing the L0DU requirements
                , 'L0RateLimit' : { 'NoBeamBeam1' : 1.e4 #'SCALE(0.1)' #!!! what to put here?
                                  , 'NoBeamBeam2' : 1.e4 #'SCALE(0.1)' #!!! what to put here?
                                  , 'Beam1'       : 5.e3
                                  , 'Beam2'       : 5.e3
                                  , 'LonelyBeam1' : 5.e3
                                  , 'LonelyBeam2' : 5.e3
                                  , 'ForcedRZReco': 1.e3
                                  #, 'Parasitic'  : 1.e6
                                  }

                ### z-ranges for Tracking and Vertexing  
                , 'Beam1VtxRangeLow'        : -1200.
                , 'Beam1VtxRangeUp'         :   400.
                , 'Beam2VtxRangeLow'        :     0.
                , 'Beam2VtxRangeUp'         :  1200.
                , 'BGVtxExclRangeMin'       :  -300.      # These 2 options take effect
                , 'BGVtxExclRangeMax'       :   300.      # only for the bb Lines

                ### Proto-Vertexing Settings  

                ### PV3D vertex cuts  
                , 'VertexMinNTracks'        :    '8'
                , 'VertexMaxChi2PerDoF'     : '9999.'
                  
                ### Input Prescales  
                , 'Prescale'                : { 'Hlt1BeamGasNoBeamBeam1'          : 1.
                                              , 'Hlt1BeamGasNoBeamBeam2'          : 1.
                                              , 'Hlt1BeamGasBeam1'                : 1.
                                              , 'Hlt1BeamGasBeam2'                : 1.
                                              , 'Hlt1BeamGasCrossingLonelyBeam1'  : 1.
                                              , 'Hlt1BeamGasCrossingLonelyBeam2'  : 1.
                                              , 'Hlt1BeamGasCrossingForcedRZReco' : 1.
                                              , 'Hlt1BeamGasCrossing'             : 1.
                                              }

                ### Output Postscales  
                , 'Postscale'               : { 'Hlt1BeamGasBeam1'                : 'RATE(20)'
                                              , 'Hlt1BeamGasBeam2'                : 'RATE(20)'
                                              , 'Hlt1BeamGasCrossing'             : 'RATE(20)'
                                              , 'Hlt1BeamGasCrossingForcedRZReco' : 'RATE(10)'
                                              , 'Hlt1BeamGasCrossingLonelyBeam1'  : 'RATE(20)'
                                              , 'Hlt1BeamGasCrossingLonelyBeam2'  : 'RATE(20)'
                                              }
                } 



    def GetTrackingAlg(self, OutTracksName):
        ''' creates common FastVelo instance '''
        from Configurables import FastVeloTracking
        algTr = FastVeloTracking('FastVeloBeamGasFullTracking', OutputTracksName = OutTracksName, HitManagerName = self.getProp("FastVeloBGHitManagerName") )
        if 'HLT1Only' in self.getProp("TrackingConf"): algTr.HLT1Only = True
        algTr.ZVertexMin = self.getProp("Beam1VtxRangeLow")
        algTr.ZVertexMax = self.getProp("Beam2VtxRangeUp")
        #!!! to check if I need to set "zVertexMaxBack" to a bigger value than 1200 mm
        return algTr 

    def GetPatVeloTrackingAlg(self, OutTracksName):
        #### Velo Tracking
        from Configurables import Tf__PatVeloGeneric, Tf__PatVeloRTracking
        from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool
        patVeloR = Tf__PatVeloRTracking('PatVeloBeamGas_RTracking')
        patVeloR.OutputTracksName = "Hlt/Track/MyRZVelo"
        patVeloR.ZVertexMin = self.getProp("Beam1VtxRangeLow")
        patVeloR.ZVertexMax = self.getProp("Beam2VtxRangeUp")

        patVeloSpace = Tf__PatVeloSpaceTracking('PatVeloBeamGas_SpaceTracking')
        patVeloSpace.InputTracksName  = patVeloR.OutputTracksName
        patVeloSpace.OutputTracksName = OutTracksName
        patVeloSpace.addTool(Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool")
        patVeloSpace.PatVeloSpaceTool.MarkClustersUsed=True

        return patVeloR, patVeloSpace 

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
        #!!! Make the algorithm apply a cut on the transverse position of the tracks
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
        from Configurables import PatPV3D, PVOfflineTool,PVSeed3DTool,LSAdaptPV3DFitter
        myPV3D = PatPV3D('PV3DHltBeamGas' + OutVerticesName[-3:])
        myPV3D.addTool(PVOfflineTool,'PVOfflineTool')
        myPV3D.PVOfflineTool.PVSeedingName = 'PVSeed3DTool'
        myPV3D.PVOfflineTool.addTool(LSAdaptPV3DFitter, 'LSAdaptPV3DFitter')
        myPV3D.PVOfflineTool.PVFitterName = 'LSAdaptPV3DFitter'
        myPV3D.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2. #!!! what is this doing
        myPV3D.PVOfflineTool.InputTracks = [InpTracksName]
        myPV3D.OutputVerticesName = OutVerticesName
        return myPV3D

    def GetVertexFilterAlg(self, lineName, InpVerticesName):
        ''' Create filters for the vertices produced by PV3D '''
        #!!! zMin, zMax = self.HelperBeamsZRange(whichBeam)
        zMin, zMax = -9999., 9999.
        from Configurables import LoKi__VoidFilter
        vtxSource             = "VSOURCE('%s')" % InpVerticesName
        vtxCut_maxChi2PerDoF  = "(VCHI2PDOF < %(VertexMaxChi2PerDoF)s)" % self.getProps()
        vtxCut_minNTracks     = "(NTRACKS > %(VertexMinNTracks)s)" % self.getProps()
        #!!!! Add a cut on the track asymmetry? --> this can help us in the case of pp collisions in ee, be or eb crossings
        #!!!! Add a cut on transverse positions - to get rid of secondary interction in the VELO
        #       - check the distribution of #vertices vs r to choose the cut
        #!!!! Add a cut on z_vtx to get rid of Offest Collisions at +/- 75 cm 
        vtxCut_zPosMin        = "(VZ > %s*mm)" % zMin
        vtxCut_zPosMax        = "(VZ < %s*mm)" % zMax
        vtxCut_excludeLumiReg = "((VZ < %s*mm) | (VZ > %s*mm))" %(self.getProp("BGVtxExclRangeMin"), self.getProp("BGVtxExclRangeMax"))
        vtxSink               = "~VEMPTY"

        listCuts = [ vtxSource, vtxCut_maxChi2PerDoF, vtxCut_minNTracks, vtxCut_zPosMin, vtxCut_zPosMax, vtxSink ]
        #in case of a bb line add the cut that vetoes the Lumi Region
        if "Cross" in lineName: listCuts.insert(-1, vtxCut_excludeLumiReg)

        ### The last algorithm should have name of line, plus 'Decision'
        return LoKi__VoidFilter( 'Hlt1%sDecision' % lineName
                               , Preambulo = ['from LoKiPhys.decorators import *']
                               , Code = " >> ".join( listCuts )
                               )
	


    #!!! GEC - only events with less than X velo tracks
    #!!! 1) Create a filter to process only events with more than X "BeamGas" RZ VELO tracks
    #    from Configurables import LoKi__VoidFilter as VoidFilter
    #    fltrInpTr = VoidFilter('BGFltr_InputTracks', Code = "CONTAINS('%(InputRZTracksLocation)s') > %(VertexMinNTracks)s" % self.getProps() )
	

    ############################################################################
    ### Function creating dummy Forced Line to check tracking
    ############################################################################
    def __create_dummyForcedLine__(self):
        from Configurables import Tf__PatVeloRTracking
        patVeloR = Tf__PatVeloRTracking('MyPatVeloRZ')

        from Configurables import FastVeloTracking
        fastVeloFull = FastVeloTracking('MyFastVeloTestFull')
        fastVeloFull.HitManagerName = 'MyFastVeloHitManagerName_Full'

        fastVeloHLT1 = FastVeloTracking('MyFastVeloTestHLT1')
        fastVeloHLT1.HLT1Only = True
        fastVeloHLT1.HitManagerName = 'MyFastVeloHitManagerName_HLT1'
        
        patVeloR.OutputTracksName     = 'MyTracks/PatVeloR'
        fastVeloFull.OutputTracksName = 'MyTracks/FastVeloFull'
        fastVeloHLT1.OutputTracksName = 'MyTracks/FastVeloHLT1'
        
        ### Make whatever alg to carry the name of the line 
        from Configurables import LoKi__VoidFilter as VoidFilter
        finalAlg = VoidFilter('Hlt1BeamGasCrossingForcedRZRecoDecision' , Code="CONTAINS('%s') > -999" %fastVeloHLT1.OutputTracksName)
    
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltDecodeRaw import DecodeVELO
        TheLine = Line( name='BeamGasCrossingForcedRZReco'
                      , priority = 1
                      , prescale = 1.
                      , algos = [DecodeVELO, patVeloR, fastVeloFull, fastVeloHLT1, finalAlg]
                      , postscale = 1.
                      )
        return TheLine

    ############################################################################
    ### Function creating the ee, be, lonely and forced lines
    ############################################################################
    def __create_nonParasitic_line__(self, whichLine, whichBeam) :
        ''' Function to create the Lines for 'ee'(2), 'be'(1), 'eb'(1), 
            'lonely bb'(2) and 'forced bb'(1) crossings. Algorithms:
            1) FastVeloTracking with enlarged z-window (the same algorithm for all lines)
            2) The Proto-vertexing selection alogrithm !!! this algo is not really needed -> saves time ??
            3) PV3D (the same algorithm for all lines)
            4) Loki-Filter the produced vertices (Algo name should be <name of line> + Decision)
            In Algos 2) and 4) need to exclude lumi region for the 'lonely bb' and 'forced bb' cases
        '''

        nameParts = self.getProp("LineNameParts")[whichLine]
        nameKey = nameParts[1]+whichBeam
        lineName = nameParts[0]+ nameKey

        ### Get the L0 Rate Limit
        L0RateLimit = self.getProp('L0RateLimit')[nameKey] if (nameKey in self.getProp('L0RateLimit').keys()) else None

        ### Create the L0DU and ODIN filters
        if 'ForcedRZReco' == nameParts[1]:
            l0du = self.getProp('L0FilterBB')
            odin = '(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'
        else:
            channel = self.getProp('L0Channel')[nameKey] if (nameKey in self.getProp('L0Channel').keys()) else None
            ## Only create an Hlt1 line if the corresponding L0 channel exists...
            from Hlt1Lines.HltL0Candidates import L0Channels
            if channel not in L0Channels() : return None
            from Hlt1Lines.HltL0Candidates import L0Mask, L0Mask2ODINPredicate
            mask = L0Mask(channel)
            l0du = "L0_CHANNEL('%s')" % channel
            odin = L0Mask2ODINPredicate(mask) #!!! we need to fix the definitions -> no more ee crossings for be and eb crossings
            ### In case of bb-lonely lines need to make the odin filter add-hoc (can't use the function L0Mask2ODINPredicate)
            if 'Lonely' == nameParts[1]: odin = '(ODIN_TRGTYP == LHCb.ODIN.BeamGasTrigger) & (ODIN_BXTYP == LHCb.ODIN.BeamCrossing)'

        ### Get the tracking algo(s) - Fast or Pat
        if   'FastVelo' in self.getProp("TrackingConf"): algTracking = [ self.GetTrackingAlg(self.getProp("TrackContName_NonBB")) ]
        elif 'PatVelo'  in self.getProp("TrackingConf"): algTracking = list(self.GetPatVeloTrackingAlg(self.getProp("TrackContName_NonBB")))
        else: print "\n\nERROR : Unknown Setting For 'TrackingConf'\n\n"
        
        ### Get the proto-vertexing algo
        ### (The last algorithm should have name of line, plus 'Decision')
        algProtoVertex = self.GetProtoVertexingAlg( lineName, algTracking[-1].OutputTracksName, whichBeam )

        ### Get the PV3D algo
        ### (The last algorithm should have name of line, plus 'Decision')
        algPV3D = self.GetPV3DAlg( algTracking[-1].OutputTracksName, self.getProp("VertexContName_NonBB") )

        ### Get the vertex filtering algo
        ### (The last algorithm should have name of line, plus 'Decision')
        algVertexFilter = self.GetVertexFilterAlg( lineName, algPV3D.OutputVerticesName )

        #log.setLevel(logging.DEBUG)
        #log.warning('#'*100)

        ''' DEBUG
        print "\n", "@"*100
        print "nameParts = ", nameParts
        print "nameKey = ", nameKey
        print "lineName = ", lineName
        print "L0 channel = ", channel if ('ForcedRZReco' != nameParts[1]) else l0du
        print "L0RateLim = ", L0RateLimit
        print "ODIN = ", odin
        print "@"*100, "\n"
        '''

        ### Finally, create the Hlt1Line
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltDecodeRaw import DecodeVELO

        TheLine = Line( lineName
                      , priority = None
                      , prescale = self.prescale
                      , ODIN  = odin
                      , L0DU  = 'scale( %s, RATE(%s) )' % (l0du, L0RateLimit) if L0RateLimit else l0du
                      , algos = [ DecodeVELO ] + algTracking + [ algPV3D, algVertexFilter ]
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
            3) Copy the Velo tracks to a different container
            4) Run second FastVeloTracking with HLT2Complement = True
            5) PV3D
            6) Filter the produced vertices
        '''
        
        nameParts = ['BeamGasCrossing', '']
        nameKey   = 'Parasitic'
        lineName  = nameParts[0]

        ### Get the L0 Rate Limit
        L0RateLimit = self.getProp('L0RateLimit')[nameKey] if (nameKey in self.getProp('L0RateLimit').keys()) else None

        ### Create the L0DU and ODIN filters
        l0du = self.getProp('L0FilterBB')
        odin = "(ODIN_BXTYP == LHCb.ODIN.BeamCrossing)"

        ### Algorithm to check for existing Velo tracks
        from HltTracking.HltReco import MinimalVelo
        from Configurables import LoKi__VoidFilter as VoidFilter
        algCheckTracks = VoidFilter('Hlt1BeamGasCheckVeloTracks' , Code = "CONTAINS('%s') > 0" % MinimalVelo.outputSelection() )

        ### The last algorithm should have name of line, plus 'Decision'
        algProtoVertex = self.GetProtoVertexingAlg( lineName, MinimalVelo.outputSelection() )

        ### Copy the Velo tracks to another container
        from Configurables import TrackContainerCopy
        algCopyTracks = TrackContainerCopy( "Hlt1BeamGasCopyTracks"
                                          , inputLocation  = MinimalVelo.outputSelection()
                                          , outputLocation = self.getProp('TrackContName_BB')
                                          )  

        ### Get the tracking algo(s) - Fast or Pat
        if   'FastVelo' in self.getProp("TrackingConf"): algTracking = [ self.GetTrackingAlg(self.getProp("TrackContName_BB")) ]
        elif 'PatVelo'  in self.getProp("TrackingConf"): algTracking = list(self.GetPatVeloTrackingAlg(self.getProp("TrackContName_BB")))
        else: print "\n\nERROR : Unknown Setting For 'TrackingConf'\n\n"

        ### Get the PV3D algo
        ### (The last algorithm should have name of line, plus 'Decision')
        algPV3D = self.GetPV3DAlg( algTracking[-1].OutputTracksName, self.getProp("VertexContName_BB") )

        ### Get the vertex filtering algo
        ### (The last algorithm should have name of line, plus 'Decision')
        algVertexFilter = self.GetVertexFilterAlg( lineName, algPV3D.OutputVerticesName )

        ''' DEBUG
        print "\n", "%"*100
        print "nameParts = ", nameParts
        print "nameKey = ", nameKey
        print "lineName = ", lineName
        print "L0 channel = ", l0du
        print "L0RateLim = ", L0RateLimit
        print "ODIN = ", odin
        print "%"*100, "\n"
        '''

        #Algos for the Line: 

        ### Finally, create the Hlt1Line
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltDecodeRaw import DecodeVELO
        TheLine = Line( lineName
                      , priority = 200 
                      , prescale = self.prescale
                      , ODIN  = odin
                      , L0DU  = 'scale( %s, RATE(%s) )' % (l0du, L0RateLimit) if L0RateLimit else l0du
                      , algos = [ DecodeVELO, algCheckTracks, algProtoVertex ] + algTracking + [ algPV3D, algVertexFilter ]
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
        self.__create_nonParasitic_line__('bblonely', 'Beam1') #line name = 'Hlt1BeamGasCrossingLonelyBeam1'
        self.__create_nonParasitic_line__('bblonely', 'Beam2')
        self.__create_nonParasitic_line__('bbforced', '') #line name = 'Hlt1BeamGasCrossingForcedRZReco'
        #instead of the forced line create a dummy one just to check the tracking versions
        #self.__create_dummyForcedLine__() #line name = 'Hlt1BeamGasCrossingForcedRZReco'
        self.__create_parasitic_line__() #line name = 'Hlt1BeamGasCrossing'
