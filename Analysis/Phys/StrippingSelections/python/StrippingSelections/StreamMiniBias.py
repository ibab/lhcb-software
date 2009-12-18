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

stream = StrippingStream("MiniBias")

stream.appendLines( [ StrippingMiniBias.TrackLine,
		      StrippingMiniBias.VertexLine, 
                      StrippingMiniBias.L0MinBiasLine , 
                      StrippingNoBias.RandomBeam1Line,
                      StrippingNoBias.RandomBeam2Line,
                      StrippingNoBias.RandomNoBeamLine,
                      StrippingNoBias.RandomBeamCrossingLine  ])


