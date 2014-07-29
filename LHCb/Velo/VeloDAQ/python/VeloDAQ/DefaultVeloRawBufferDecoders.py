"""
##############################################################################
#                                                                            #
#  Velo/VeloDAQ helper module.                                               #
#  Contains default configurations of the DecodeVeloRawBuffer algorithm      #
#  for decoding into VeloLiteClusters or VeloClusters.                       #
#                                                                            #
#  @package Velo/VeloDAQ                                                     #
#  @author  Eduardo Rodrigues  ( e.rodrigues@physics.gla.ac.uk )             #
#  @date    06/10/2008                                                       #
#                                                                            #
#                                                                            #
##############################################################################
"""

from Configurables import DecodeVeloRawBuffer

# =============================================================================
# Instance of DecodeVeloRawBuffer for decoding into VeloLiteClusters
# =============================================================================
def DefaultDecoderToVeloLiteClusters(
    Name                     = 'DecodeVeloRawBufferToVeloLiteClusters',
    RawEventLocations        = ['Velo/RawEvent', 
                                'Other/RawEvent',
                                'DAQ/RawEvent'],
    VeloClusterLocation      ='Raw/Velo/Clusters',
    DecodeToVeloClusters     = False,
    DecodeToVeloLiteClusters = True
    ):
    
    decoder = DecodeVeloRawBuffer( Name )
    
    decoder.RawEventLocations        = RawEventLocations
    decoder.VeloClusterLocation      = VeloClusterLocation
    decoder.DecodeToVeloClusters     = DecodeToVeloClusters
    decoder.DecodeToVeloLiteClusters = DecodeToVeloLiteClusters
    
    return decoder


# =============================================================================
# Instance of DecodeVeloRawBuffer for decoding into VeloClusters
# =============================================================================
def DefaultDecoderToVeloClusters(
    Name                     = 'DecodeVeloRawBufferToVeloClusters',
    RawEventLocations        = ['Velo/RawEvent',
                                'Other/RawEvent',
                                'DAQ/RawEvent'],
    VeloClusterLocation      ='Raw/Velo/Clusters',
    DecodeToVeloClusters     = True,
    DecodeToVeloLiteClusters = False
    ):
    
    decoder = DecodeVeloRawBuffer( Name )
    
    decoder.RawEventLocations        = RawEventLocations
    decoder.VeloClusterLocation      = VeloClusterLocation
    decoder.DecodeToVeloClusters     = DecodeToVeloClusters
    decoder.DecodeToVeloLiteClusters = DecodeToVeloLiteClusters
    
    return decoder

###############################################################################
