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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.19 $"
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
                , 'ForcedInputRateLimit'    :  1000.
                , 'Prescale'                : { 'Hlt1BeamGasBeam1' :                1.0
                                              , 'Hlt1BeamGasBeam2' :                1.0
                                              , 'Hlt1BeamGasCrossing' :             1.0
                                              , 'Hlt1BeamGasCrossingForcedRZReco' : 0.001
                                              }
                , 'Postscale'               : { 'Hlt1BeamGasBeam1' :                'RATE(25)'
                                              , 'Hlt1BeamGasBeam2' :                'RATE(25)'
                                              , 'Hlt1BeamGasCrossing' :             'RATE(25)'
                                              , 'Hlt1BeamGasCrossingForcedRZReco' : 'RATE(25)'
                                              }
                } 


    def __create_beam_empty_line__(self, whichBeam) :
        ''' Configure the Hlt Line for the cases of b1-empty or
            empty-b2 crossings. Run RZTracking and VtxCut algo. 
        '''
        
        name = 'BeamGas'+whichBeam

        from Configurables import ( Tf__PatVeloRTracking,
                                    Tf__PatVeloRHitManager )
            
        rm = Tf__PatVeloRHitManager( name + 'RHitManager',
                                     DefaultHitManagerName = name + 'DefaultVeloRHitManager' )

        algRZTracking = Tf__PatVeloRTracking( 'Hlt1RZVeloBeamGas'+whichBeam,
                                              OutputTracksName = "Hlt1/Track/RZVeloBeamGas",
                                              ZVertexMin  = self.getProp(whichBeam+"VtxRangeLow"),
                                              ZVertexMax  = self.getProp(whichBeam+"VtxRangeUp"),
                                              HitManagerName = rm.splitName()[ -1 ] )

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
        from HltTracking.HltReco import DecodeVELO
        channel = self.getProp('L0Channel' + whichBeam)
        ##  Only create an Hlt1 line if the corresponding L0 channel exists...
        from Hlt1Lines.HltL0Candidates import L0Channels
        if channel not in L0Channels() : return None
        from Hlt1Lines.HltL0Candidates import L0Mask, L0Mask2ODINPredicate
        mask = L0Mask(channel)
        return Line( name
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
        from HltTracking.HltReco import MinimalRZVelo
        from Configurables import LoKi__VoidFilter as VoidFilter
        algCheckTracks = VoidFilter('Hlt1BeamGasRequireRZVelo' , Code = "CONTAINS('%s') > 0" % MinimalRZVelo.outputSelection() )
        
        from Configurables import BeamGasTrigClusterCut
        algClusterCut = BeamGasTrigClusterCut( 'Hlt1BeamGasTrigClusterCut'
                                             , SensorsBegin = 22
                                             , SensorsEnd   = 39
                                             , FracUnusedClustersCut = 0.27 )

        from Configurables import BeamGasTrigExtractClusters
        algExtractClust = BeamGasTrigExtractClusters( 'Hlt1BeamGasTrigExtractClusters'
                                                    , OutputLocation = "Raw/Velo/RLiteClustersBeamGas"
                                                    )

        ### To be checked if these two instantiations are needed ???
        from Configurables import Tf__DefaultVeloRHitManager
        defmgr = Tf__DefaultVeloRHitManager( "Hlt1BeamGasUnusedRClustersDefaultVeloRHitManager"
                                           , LiteClusterLocation = algExtractClust.OutputLocation )

        from Configurables import Tf__PatVeloRHitManager
        hitmgr = Tf__PatVeloRHitManager( "Hlt1BeamGasUnusedRCluserPatVeloRHitManager"
                                       , DefaultHitManagerName= defmgr.name()  )

        from Configurables import Tf__PatVeloRTracking
        algRTracking2 = Tf__PatVeloRTracking(   'Hlt1BeamGasRZVeloSecondPass'
                                            , HitManagerName = hitmgr.name()
                                            , OutputTracksName = "Hlt1/Track/RZVeloBeamGas"
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

        # FIXME: why does 'clone' not get prescaled right??? because the prescale _value_ not the function is cloned...
        #        hence we repeat prescale and postscale here explicitly...
        # NOTE: we remove the 'priority' from the clone to make sure it runs first...
        from HltTracking.HltReco import MinimalRZVelo
        limit =  self.getProp('ForcedInputRateLimit') 
        line_beamCrossingForcedRZReco = line_beamCrossing.clone( lineName+"ForcedRZReco"
                                                               , priority = None
                                                               , prescale = self.prescale
                                                               , ODIN = 'scale( ODIN_ALL, RATE(%s) )' % limit if limit else 'ODIN_ALL'
                                                               , algos = [ MinimalRZVelo ] + bgTrigAlgos 
                                                               , postscale = self.postscale 
                                                               )
        return line_beamCrossing, line_beamCrossingForcedRZReco
        
    def __apply_configuration__(self) : 
        self.__create_beam_empty_line__('Beam1')
        self.__create_beam_empty_line__('Beam2')
        self.__create_beam_crossing_lines__()
