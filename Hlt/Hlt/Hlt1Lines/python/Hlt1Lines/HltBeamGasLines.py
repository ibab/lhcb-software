# =============================================================================
# $Id: HltBeamGasLines.py,v 1.6 2009-07-14 19:47:30 phopchev Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.6 $"
# =============================================================================

from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from HltLine.HltLine import Hlt1Line   as Line

#from Configurables import DecodeVeloRawBuffer
#from Configurables import Tf__PatVeloRTracking
#from Configurables import BeamGasTrigCheckL0TracksBXType
#from Configurables import BeamGasTrigExtractClusters
#from Configurables import BeamGasTrigClusterCut
#from Configurables import BeamGasTrigVertexCut

class HltBeamGasLinesConf(HltLinesConfigurableUser) :
    #--------------------------------
    #
    # For the moment set up to select a single hadron Et candidate in the calorimeter
    # matching with a velo track, and with a minimum Pt

    # steering variables
    __slots__ = { 
                  'BXTypes'                 : ['BeamCrossing', 'Beam1', 'Beam2']
                , 'L0ChannelBeam1'          : "Hadron" # assume the L0 puts special channel name
                , 'L0ChannelBeam2'          : "Hadron" # assume the L0 puts special channel name
		, 'L0ChannelBeamCrossing'   : "Hadron" # ???		
		, 'BGOnlyTracksLocation'    : 'Hlt/Track/RZVeloBeamGasOnly'		
		, 'ZVertexMin'              : -1500.
		, 'ZVertexMax'              :  1500.
		, 'BGVtxExclRangeMin'       : -265.
		, 'BGVtxExclRangeMax'       :  265.			
                } 
		
    def __create_beam_empty_line__(self, whichBeam) :
        ''' Configure the Hlt Line for the case of beam-gas 
            interaction in a beam1/2 - empty crossing '''

	lineName = "emptyName"
	if whichBeam == 'Beam1' : lineName = "BeamGas1"
	if whichBeam == 'Beam2' : lineName = "BeamGas2"
	
	ODINRequirement = "ODIN_BXTYP == LHCb.ODIN." + whichBeam	

	L0DURequirement = "L0_CHANNEL('" + self.getProp('L0Channel' + whichBeam) + "')"
	algoList = []	
	
	'''
	### Can this alg be totally replaced:
	### a) BXType   b) L0Decision   c) N RZ tracks	    
	algCheckL0 = BeamGasTrigCheckL0TracksBXType(   'CheckL0' + whichBeam
                                		     , ChechBXType     = False
						     , CheckL0Decision = False 
						     , CheckTracks     = False  )

	### here we can add different z-ranges for beam1 and beam2
	algRTracking = Tf__PatVeloRTracking(   'HltRecoRZVelo_' + lineName
	                        	     , OutputTracksName = "Hlt/Track/RZVeloBeamGasOnly"
					     , ZVertexMin  = self.getProp('ZVertexMin')
					     , ZVertexMax  = self.getProp('ZVertexMax')
					     , OutputLevel = INFO  )	    						   


	### Question (for Vanya?):
	### In the "LINES" framework Do we have a cut on the N of Tracks in a certain container?
	algVtxCut = BeamGasTrigVertexCut(   'BeamGasTrigVertexCut_' + lineName
	                		  , RZTracksInputLocation = algRTracking.OutputTracksName
	                		  , MaxBinValueCut     = 5
	                		  , HistoBinWidth      = 10
	                		  , ZExclusionRangeLow = 0.
					  , ZExclusionRangeUp  = 0.  
					  , OutputLevel        = INFO
					  , MinCandidates      = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
					  #, OutputSelectionName = ... #By default = name of the algorithm
					)   


	algoList.append( algCheckL0   )
	algoList.append( algRTracking )
	algoList.append( algVtxCut    )
        '''

	return Line( lineName
	           ,prescale = self.prescale
                   ,ODIN  = ODINRequirement
		   ,L0DU  = L0DURequirement 
		   ,algos = algoList
		   ,postscale = self.postscale )
		   
    def __create_beam_crossing_line__(self) :
	''' Configure the Hlt Line for the case of beam-gas 
            interaction overlapped with a pp collision '''

	lineName = "BeamGasCrossing"
	ODINRequirement = "ODIN_BXTYP == LHCb.ODIN.BeamCrossing"
	L0DURequirement = "L0_CHANNEL('" + self.getProp('L0ChannelBeamCrossing') + "')"
	algoList = []

        '''  
	### Reconstruct RZ Tracks Manually 
	algDecodeVelo = DecodeVeloRawBuffer()  

	algRTracking1 = Tf__PatVeloRTracking(   'HltRecoRZVelo_BeamCrossing'
                                              , OutputTracksName = "Hlt/Track/RZVelo" )

	algCheckL0 = BeamGasTrigCheckL0TracksBXType(   'CheckL0BeamCrossing'
	                                	     , ChechBXType     = False 
	                                	     , CheckL0Decision = False
	                                	     , CheckTracks     = True
						     , PVRTTracksLocation = algRTracking1.OutputTracksName )

	algClusterCut = BeamGasTrigClusterCut(   SensorsBegin = 22
	                		       , SensorsEnd   = 39
					       , FracUnusedClustersCut = 0.27
					       , OutputLevel = INFO )

	algExtractClust = BeamGasTrigExtractClusters(   OutputLocation = "Raw/Velo/UnusedRLiteClusters"
                                                      , OutputLevel = INFO )

	algRTracking2 = Tf__PatVeloRTracking(   'HltRecoRZVelo_BeamCrossing_Second'
	                		      , HitManagerName = "SecondDefaultVeloRHitManager"
					      #, ToolSvc.SecondDefaultVeloRHitManager.LiteClusterLocation = "Raw/Velo/UnusedRLiteClusters"
					      , OutputTracksName = "Hlt/Track/RZVeloBeamGas"
					      , ZVertexMin  = self.getProp('ZVertexMin')
					      , ZVertexMax  = self.getProp('ZVertexMax')
					      , OutputLevel = INFO )  

	algVtxCut = BeamGasTrigVertexCut(   'BeamGasTrigVertexCut_BeamCrossing'
	                		  , RZTracksInputLocation = algRTracking2.OutputTracksName
	                		  , MaxBinValueCut     = 5
	                		  , HistoBinWidth      = 10
	                		  , ZExclusionRangeLow = -265.
					  , ZExclusionRangeUp  =  265.  
					  , OutputLevel        = INFO
					  , MinCandidates      = 1 #Should be > 0 and <= MaxBinValueCut+1 (by default = 1)
					  #, OutputSelectionName = ... #By default = name of the algorithm
					)

	algoList.append( algDecodeVelo   )		         
	algoList.append( algRTracking1   )

	algoList.append( algCheckL0      )
	algoList.append( algClusterCut   )
	algoList.append( algExtractClust )
	algoList.append( algRTracking2   )
	algoList.append( algVtxCut       )

        '''

        return Line( lineName
                   ,prescale = self.prescale
                   ,ODIN  = ODINRequirement
		   ,L0DU  = L0DURequirement 
		   ,algos = algoList
		   ,postscale = self.postscale )
		  
    
    def __apply_configuration__(self) : 
        from HltConf.HltReco import RZVelo

	linesToConfig = self.getProp('BXTypes')
	
	if linesToConfig.__contains__('Beam1'): self.__create_beam_empty_line__('Beam1')
	if linesToConfig.__contains__('Beam2'): self.__create_beam_empty_line__('Beam2')
	if linesToConfig.__contains__('BeamCrossing'): self.__create_beam_crossing_line__()
	
	
	
	
