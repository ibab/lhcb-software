# =============================================================================
## @file
#  Configuration of BeamGas Lines
#  @author Jaap Panman jaap.panman@cern.ch
#  @date 2009-02-25
# =============================================================================
"""
 script to configure BeamGas trigger lines
"""
# =============================================================================
__author__  = "Jaap Panman jaap.panman@cern.ch"
__author__  = "Plamen Hopchev phopchev@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.7 $"
# =============================================================================

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt1BeamGasLinesConf(HltLinesConfigurableUser) :

    # steering variables
    __slots__ = { 'L0ChannelBeam1'          : "B1gas"
                , 'L0ChannelBeam2'          : "B2gas"
                , 'L0ChannelBeamCrossing'   : [ "SPD" , "PU" ]
                , 'Beam1VtxRangeLow'        : -2000.
                , 'Beam1VtxRangeUp'         :   600.
                , 'Beam2VtxRangeLow'        :     0.
                , 'Beam2VtxRangeUp'         :  2000.
                , 'BGVtxExclRangeMin'       :  -350.      # These 2 options take effect
                , 'BGVtxExclRangeMax'       :   250.      # only for the Lines for bb BX
                , 'MaxBinValueCut'          :     4
                , 'HistoBinWidth'           :    14
                , 'ForcedRZVeloFraction'    :     0.1
                , 'Prescale'                : { 'Hlt1BeamGasBeam1' :                0.5
                                              , 'Hlt1BeamGasBeam2' :                0.5
                                              , 'Hlt1BeamGasCrossing' :             0.05
                                              , 'Hlt1BeamGasCrossingForcedRZReco' : 0.001
                                              }
                , 'Postscale'               : { 'Hlt1BeamGasBeam1' :                'RATE(25)'
                                              , 'Hlt1BeamGasBeam2' :                'RATE(25)'
                                              , 'Hlt1BeamGasCrossing' :             'RATE(25)'
                                              , 'Hlt1BeamGasCrossingForcedRZReco' : 'RATE(25)'
                                              }
                } 


    def __performRZVelo( self ) :
        from HltLine.HltReco import RZVelo
        from Configurables import DeterministicPrescaler as Scaler

        return [ GaudiSequencer( 'BeamGasRZVeloSequencer'
                               , Members = [ Scaler('BeamGasRZVeloPrescaler', AcceptFraction = self.getProp('ForcedRZVeloFraction') )
                                           ] + RZVelo.members()
                               , IgnoreFilterPassed = True )            
              ]


    def __create_beam_empty_line__(self, whichBeam) :
        ''' Configure the Hlt Line for the cases of b1-empty or
            empty-b2 crossings. Run RZTracking and VtxCut algo. 
        '''
        
        name = 'BeamGas'+whichBeam

        from Configurables import Tf__PatVeloRTracking
        algRZTracking = Tf__PatVeloRTracking( 'Hlt1RZVeloBeamGas'+whichBeam 
                                            , OutputTracksName = "Hlt/Track/RZVeloBeamGas"
                                            , ZVertexMin  = self.getProp(whichBeam+"VtxRangeLow")
                                            , ZVertexMax  = self.getProp(whichBeam+"VtxRangeUp")
                                            )

        ## last algorithm should have name of line, plus 'Decision'
        from Configurables import BeamGasTrigVertexCut
        algVtxCut = BeamGasTrigVertexCut( 'Hlt1%sDecision' % name
                                        , RZTracksInputLocation = algRZTracking.OutputTracksName
                                        , MaxBinValueCut     = self.getProp("MaxBinValueCut")
                                        , HistoBinWidth      = self.getProp("HistoBinWidth")
                                        , HistoZRangeLow     = self.getProp(whichBeam+"VtxRangeLow")
                                        , HistoZRangeUp      = self.getProp(whichBeam+"VtxRangeUp")
                                        , ZExclusionRangeLow = 0.
                                        , ZExclusionRangeUp  = 0.  
                                        , MinCandidates      = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
                                        )           
 
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltReco import DecodeVELO
        channel = self.getProp('L0Channel' + whichBeam)
        ##  Only create an Hlt1 line if the corresponding L0 channel exists...
        from Hlt1Lines.HltL0Candidates import L0Channels
        if channel not in L0Channels() : return None
        from Hlt1Lines.HltL0Candidates import L0Mask, L0Mask2ODINPredicate
        mask = L0Mask(channel)
        return Line( name
                   , priority = 200
                   , prescale = self.prescale
                   , ODIN  = L0Mask2ODINPredicate(mask) 
                   , L0DU  = "L0_CHANNEL('%s')" % channel
                   , algos = [ DecodeVELO, algRZTracking, algVtxCut ]
                   , postscale = self.postscale )

   
    def __create_beam_crossing_lines__(self) :
        ''' Configure the Hlt Lines for the case of beam-gas interaction 
            overlapped with a pp collision. The first line runs only if 
            the RZVelo tracks are already reconstructed. The second line
            (ForcedRZReco) has RZVelo in its sequence, so runs it if needed.
        '''

        lineName = "BeamGasCrossing"

        ### To check the existence of tracks !!!
        #First need to have a 'TrEXISTS' predicate...
        #algCheckRZVelo = VoidFilter('RequireRZVelo' , Code = "TrSOURCE('Hlt/Track/RZVelo') >> (TrSIZE > 0)"

        from Configurables import BeamGasTrigCheckL0TracksBXType
        algCheckTracks = BeamGasTrigCheckL0TracksBXType( ChechBXType     = False
                                                       , CheckL0Decision = False
                                                       , CheckTracks     = True
                                                       , OutputLevel     = FATAL
                                                       )  
        
        from Configurables import BeamGasTrigClusterCut
        algClusterCut = BeamGasTrigClusterCut( SensorsBegin = 22
                                             , SensorsEnd   = 39
                                             , FracUnusedClustersCut = 0.27 )

        from Configurables import BeamGasTrigExtractClusters
        algExtractClust = BeamGasTrigExtractClusters( OutputLocation = "Raw/Velo/RLiteClustersBeamGas" )

        ### To be checked if these two instantiations are needed ???
        from Configurables import Tf__DefaultVeloRHitManager
        defmgr = Tf__DefaultVeloRHitManager( "UnusedRClustersDefaultVeloRHitManager"
                                           , LiteClusterLocation = algExtractClust.OutputLocation )

        from Configurables import Tf__PatVeloRHitManager
        hitmgr = Tf__PatVeloRHitManager( "UnusedRCluserPatVeloRHitManager"
                                       , DefaultHitManagerName= defmgr.name()  )

        from Configurables import Tf__PatVeloRTracking
        algRTracking2 = Tf__PatVeloRTracking(   'Hlt1BeamGasRZVeloSecondPass'
                                            , HitManagerName = hitmgr.name()
                                            , OutputTracksName = "Hlt/Track/RZVeloBeamGas"
                                            , ZVertexMin  = self.getProp("Beam1VtxRangeLow")
                                            , ZVertexMax  = self.getProp("Beam2VtxRangeUp")
                                            )

        from Configurables import BeamGasTrigVertexCut
        algVtxCut = BeamGasTrigVertexCut( 'Hlt1%sDecision' % lineName 
                                        , RZTracksInputLocation = algRTracking2.OutputTracksName
                                        , MaxBinValueCut     = self.getProp("MaxBinValueCut")
                                        , HistoBinWidth      = self.getProp("HistoBinWidth")
                                        , HistoZRangeLow     = self.getProp("Beam1VtxRangeLow")
                                        , HistoZRangeUp      = self.getProp("Beam2VtxRangeUp")
                                        , ZExclusionRangeLow = self.getProp("BGVtxExclRangeMin")
                                        , ZExclusionRangeUp  = self.getProp("BGVtxExclRangeMax")
                                        , MinCandidates      = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
                                        )

        bgTrigAlgos = [ algClusterCut, algExtractClust, algRTracking2, algVtxCut ]

        from HltLine.HltLine import Hlt1Line as Line
        from Hlt1Lines.HltL0Candidates import L0Channels
        channels = [ channel for channel in self.getProp('L0ChannelBeamCrossing') if channel in L0Channels() ]
        if not channels  : return None
        line_beamCrossing = Line( lineName
                                , priority = 200 
                                , prescale = self.prescale
                                , L0DU  = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in channels ] )
                                , algos = [ algCheckTracks ] + bgTrigAlgos
                                , postscale = self.postscale )

        line_beamCrossingForcedRZReco = line_beamCrossing.clone( lineName+"ForcedRZReco"
                                                               , algos = self.__performRZVelo() + bgTrigAlgos )
        return line_beamCrossing, line_beamCrossingForcedRZReco
        
    def __apply_configuration__(self) : 
        self.__create_beam_empty_line__('Beam1')
        self.__create_beam_empty_line__('Beam2')
        self.__create_beam_crossing_lines__()
