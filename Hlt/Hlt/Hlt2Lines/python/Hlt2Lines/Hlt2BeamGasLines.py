###############################################################
# Hlt2BeamGasLines : Run Space tracking and PV3D reconstruction
# looking for beam-gas interactions. The input tracks for the space tracking are
# the ones produced in the Hlt1 BeamGas lines. The space tracks are used to
# reconstruct 3D vertices, which are further filtered to form the decision of the lines.
#
# Maintainer : Plamen Hopchev
# Date       : 26 Aug  2010
# Revised    : 28 Sept 2010
############################################################

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2BeamGasLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 'VertexMinNTracks'       : '9'
                , 'AbsZPosMin'             : '300.'
                , 'AbsZPosMax'             : '1300.'
                , 'VertexMaxChi2'          : '9999.'
                , 'InputRZTracksLocation'  : 'Hlt1/Track/RZVeloBeamGas'
                , 'Output3DTracksLocation' : 'Hlt/Track/BeamGasVelo3D'
                , 'OutputVerticesLocation' : 'Hlt/Vertex/PVBeamGas'
                #, 'Prescale'             : 1.0   
                #, 'Postscale'            : 1.0   
                }

    
    def __apply_configuration__(self) :
        
        from HltLine.HltLine import Hlt2Line
        from Configurables import HltANNSvc

        ## get the sequence which does the 3D track and PV reconstruction
        recoSeq = self.configureMyReco()

        ## Create filters on the produced vertices: 2 different filters for the non-beam-beam and the beam-beam crossings
        from Configurables import LoKi__VoidFilter
        vtxCut_MinNTracks = "(NTRACKS > "+self.getProp('VertexMinNTracks')+")"
        vtxCut_AbsZPosMax = "(abs(VZ) < "+self.getProp('AbsZPosMax')+" * mm)"
        vtxCut_AbsZPosMin = "(abs(VZ) > "+self.getProp('AbsZPosMin')+" * mm)"
        vtxCut_MaxChi2    = "(VCHI2 < "+self.getProp('VertexMaxChi2')+")"

        ## The vertex filters will be the last algorithms inside the Hlt2 lines so they need to have the names of the lines
        fltrVerticesBE = LoKi__VoidFilter( 'Hlt2BeamGasNonBBCrossingDecision',
                                           Preambulo = ['from LoKiPhys.decorators import *'],
                                           Code = "VSOURCE('"+self.getProp('OutputVerticesLocation')+"') >> "+vtxCut_MaxChi2+" >> "+vtxCut_MinNTracks+" >> "+vtxCut_AbsZPosMax+" >> ~VEMPTY"
                                         )

        fltrVerticesBB = LoKi__VoidFilter( 'Hlt2BeamGasBBCrossingDecision',
                                           Preambulo = ['from LoKiPhys.decorators import *'],
                                           Code = "VSOURCE('"+self.getProp('OutputVerticesLocation')+"') >> "+vtxCut_MaxChi2+" >> "+vtxCut_MinNTracks+" >> "+vtxCut_AbsZPosMin+" >> "+vtxCut_AbsZPosMax+" >> ~VEMPTY"
                                         )



        lineNonBB = Hlt2Line( 'BeamGasNonBBCrossing'
                            , prescale = self.prescale
                            , ODIN = 'ODIN_BXTYP != LHCb.ODIN.BeamCrossing'
                            , HLT = "HLT_PASS_SUBSTR('Hlt1BeamGas')"
                            , algos = recoSeq+[fltrVerticesBE]
                            , postscale = self.postscale
                            )

        lineBB = Hlt2Line( 'BeamGasBBCrossing'
                         , prescale = self.prescale
                         , ODIN = 'ODIN_BXTYP == LHCb.ODIN.BeamCrossing'
                         , HLT = "HLT_PASS_SUBSTR('Hlt1BeamGas')"
                         , algos = recoSeq+[fltrVerticesBB]
                         , postscale = self.postscale
                         )

        ###!!! temporary !!!
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BeamGasNonBBCrossingDecision" : 55555 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2BeamGasBBCrossingDecision"    : 55556 } )


    def configureMyReco(self):

        bgTracksLocation_RZ = self.getProp('InputRZTracksLocation')
        bgTracksLocation_3D = self.getProp('Output3DTracksLocation')

        ### 1) Create a filter to process only events with more than X "BeamGas" RZ VELO tracks
        from Configurables import LoKi__VoidFilter as VoidFilter
        fltrInpTr = VoidFilter('BGFltr_InputTracks', Code = "CONTAINS('%s') > %d" %(bgTracksLocation_RZ, int(self.getProp('VertexMinNTracks'))) )

        ## 2) Do Space Tracking on top of the RZ tracks produced in the Hlt1BeamGas lines
        from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloSpaceTool
        mySpaceTracking = Tf__PatVeloSpaceTracking( 'SpaceTrackingBeamGas', InputTracksName  = bgTracksLocation_RZ, OutputTracksName = bgTracksLocation_3D )
        mySpaceTracking.addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
        mySpaceTracking.PatVeloSpaceTool.MarkClustersUsed=False
        

        ### 3) Configure the vertex fitter
        from Configurables import PatPV3D, PVOfflineTool,PVSeed3DTool,LSAdaptPV3DFitter

        recoPV3D =  PatPV3D('HltPV3DBeamGas' )
        recoPV3D.addTool(PVOfflineTool,'PVOfflineTool')

        recoPV3D.PVOfflineTool.PVSeedingName = 'PVSeed3DTool'

        recoPV3D.PVOfflineTool.addTool(LSAdaptPV3DFitter, 'LSAdaptPV3DFitter')
        recoPV3D.PVOfflineTool.PVFitterName = 'LSAdaptPV3DFitter'
        recoPV3D.PVOfflineTool.LSAdaptPV3DFitter.TrackErrorScaleFactor = 2.

        recoPV3D.PVOfflineTool.InputTracks = [bgTracksLocation_3D]
        recoPV3D.OutputVerticesName = self.getProp('OutputVerticesLocation')

        ### 4) return a list of the reconstruction algorithms
        return [fltrInpTr, mySpaceTracking, recoPV3D]


        
