#
# Stripping selections 
# for Minimum Bias physics.
# author: Yasmine Amhis
# date : 2009-11-11
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingMiniBias 
from StrippingSelections import StrippingNoBias 
from StrippingSelections.StrippingPass import StrippingPassConf

stream = StrippingStream("MinBias")

stream.appendLines( [ StrippingMiniBias.TrackingRequirementLine, 
                      StrippingMiniBias.L0MinBiasLine , 
                      StrippingMiniBias.TrackLine , 
                      StrippingMiniBias.VertexLine , 
                      StrippingNoBias.RandomBeam1Line,
                      StrippingNoBias.RandomBeam2Line,
                      StrippingNoBias.RandomNoBeamLine,
                      StrippingNoBias.RandomBeamCrossingLine, 
                      StrippingPassConf().passLine(),
                      StrippingPassConf().physLine() ] )
