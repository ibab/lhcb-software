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
#  $Id: DefaultVeloRawBufferDecoders.py,v 1.1 2008-10-08 13:33:31 erodrigu Exp $                             #
#  CVS tag $Name: not supported by cvs2svn $,version $Revision: 1.1 $                                       #
#                                                                            #
##############################################################################
"""

from Configurables import DecodeVeloRawBuffer

# =============================================================================
# Instance of DecodeVeloRawBuffer for decoding into VeloLiteClusters
# =============================================================================
def DefaultDecoderToVeloLiteClusters(
    Name                     = 'DecodeVeloRawBufferToVeloLiteClusters',
    RawEventLocation         = 'DAQ/RawEvent',
    VeloClusterLocation      ='Raw/Velo/Clusters',
    DecodeToVeloClusters     = False,
    DecodeToVeloLiteClusters = True
    ):
    
    decoder = DecodeVeloRawBuffer( Name )
    
    decoder.RawEventLocation         = RawEventLocation
    decoder.VeloClusterLocation      = VeloClusterLocation
    decoder.DecodeToVeloClusters     = DecodeToVeloClusters
    decoder.DecodeToVeloLiteClusters = DecodeToVeloLiteClusters
    
    return decoder


# =============================================================================
# Instance of DecodeVeloRawBuffer for decoding into VeloClusters
# =============================================================================
def DefaultDecoderToVeloClusters(
    Name                     = 'DecodeVeloRawBufferToVeloClusters',
    RawEventLocation         = 'DAQ/RawEvent',
    VeloClusterLocation      ='Raw/Velo/Clusters',
    DecodeToVeloClusters     = True,
    DecodeToVeloLiteClusters = False
    ):
    
    decoder = DecodeVeloRawBuffer( Name )
    
    decoder.RawEventLocation         = RawEventLocation
    decoder.VeloClusterLocation      = VeloClusterLocation
    decoder.DecodeToVeloClusters     = DecodeToVeloClusters
    decoder.DecodeToVeloLiteClusters = DecodeToVeloLiteClusters
    
    return decoder

###############################################################################
