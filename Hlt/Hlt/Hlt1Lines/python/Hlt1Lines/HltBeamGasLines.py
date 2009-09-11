# =============================================================================
# $Id: HltBeamGasLines.py,v 1.13 2009-09-11 13:38:56 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.13 $"
# =============================================================================

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class HltBeamGasLinesConf(HltLinesConfigurableUser) :

    # steering variables
    __slots__ = { 'L0ChannelBeam1'          : "Hadron" # assume the L0 puts special channel name
                , 'L0ChannelBeam2'          : "Hadron" # assume the L0 puts special channel name
                , 'L0ChannelBeamCrossing'   : "Hadron" # ???		
                , 'ZVertexMin'              : -1500.
                , 'ZVertexMax'              :  1500.
                , 'BGVtxExclRangeMin'       : -265.
                , 'BGVtxExclRangeMax'       :  265.
                , 'ForcedRZVeloFraction'    : 0.001
                #, 'Prescale'                : { '.*' : 0.000005 }
                , 'Prescale'                : { 'Hlt1BeamGas1' :                    0.1
		                              , 'Hlt1BeamGas2' :                    0.1
					      , 'Hlt1BeamGasCrossing' :             0.01
					      , 'Hlt1BeamGasCrossingForcedRZReco' : 0.0001
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
        ''' Configure the Hlt Line for the case of beam-gas 
            interaction in a beam1/2 - empty crossing.
	    Here we expect that nothing has been executed yet,
	    so we do our 'own' RZ Tracking first. '''

        name = { 'Beam1': 'BeamGas1', 'Beam2' : 'BeamGas2' }[ whichBeam ]
        	
	
        ## here we can add different z-ranges for the beam1 and beam2 crossings
        from Configurables import Tf__PatVeloRTracking
        algRZTracking = Tf__PatVeloRTracking( 'Hlt1%sRZTracking' % name 
                                           , OutputTracksName = "Hlt/Track/RZVeloBeamGas"
                                           , ZVertexMin  = self.getProp('ZVertexMin')
                                           , ZVertexMax  = self.getProp('ZVertexMax') )
        ## last algorithm should have name of line, plus 'Decision'
        from Configurables import BeamGasTrigVertexCut
        algVtxCut = BeamGasTrigVertexCut( 'Hlt1%sDecision' % name
                                        , RZTracksInputLocation = algRZTracking.OutputTracksName
                                        , MaxBinValueCut     = 5
                                        , HistoBinWidth      = 10
                                        , ZExclusionRangeLow = 0.
                                        , ZExclusionRangeUp  = 0.  
                                        , MinCandidates      = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
                                        )           
 
        from HltLine.HltLine import Hlt1Line as Line
        from HltLine.HltReco import DecodeVELO
        return Line( name
	           , priority = 200
                   , prescale = self.prescale
                   , ODIN  = "ODIN_BXTYP == LHCb.ODIN.%s" % whichBeam
                   , L0DU  = "L0_CHANNEL('%s')" % self.getProp('L0Channel' + whichBeam)
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
                                                       )
						       
        from Configurables import BeamGasTrigClusterCut
        algClusterCut = BeamGasTrigClusterCut( SensorsBegin = 22
                                             , SensorsEnd   = 39
                                             , FracUnusedClustersCut = 0.27 )

        from Configurables import BeamGasTrigExtractClusters
        algExtractClust = BeamGasTrigExtractClusters( OutputLocation = "Raw/Velo/UnusedRLiteClusters" )

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
                                            , ZVertexMin  = self.getProp('ZVertexMin')
                                            , ZVertexMax  = self.getProp('ZVertexMax')
                                            )

        from Configurables import BeamGasTrigVertexCut
        algVtxCut = BeamGasTrigVertexCut(   'Hlt1%sDecision' % lineName 
                                        , RZTracksInputLocation = algRTracking2.OutputTracksName
                                        , MaxBinValueCut     = 5
                                        , HistoBinWidth      = 10
                                        , ZExclusionRangeLow = -265.
                                        , ZExclusionRangeUp  =  265.  
                                        , MinCandidates      = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
                                        #, OutputSelectionName = ... #By default = name of the algorithm
                                        )

       	bgTrigAlgos = [ algClusterCut, algExtractClust, algRTracking2, algVtxCut ]
	
	from HltLine.HltLine import Hlt1Line as Line
        line_beamCrossing = Line( lineName
                		, priority = 200 
                		, prescale = self.prescale
                		, ODIN  = "ODIN_BXTYP == LHCb.ODIN.BeamCrossing"
                		, L0DU  = "L0_CHANNEL('%s')" %  self.getProp('L0ChannelBeamCrossing')
                		, algos = [ algCheckTracks ] + bgTrigAlgos
                		, postscale = self.postscale )

        line_beamCrossingForcedRZReco = line_beamCrossing.clone( lineName+"ForcedRZReco"
							       , algos = self.__performRZVelo() + bgTrigAlgos )
			    
					    		  
        return line_beamCrossing, line_beamCrossingForcedRZReco
        
    def __apply_configuration__(self) : 
        self.__create_beam_empty_line__('Beam1')
        self.__create_beam_empty_line__('Beam2')
        self.__create_beam_crossing_lines__()
	
	
