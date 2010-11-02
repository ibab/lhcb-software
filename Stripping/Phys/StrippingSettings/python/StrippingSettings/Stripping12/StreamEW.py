# $Id: StreamEW.py,v 1.3 2010-09-07 16:08:59 gcowan Exp $
#
#   Electro-weak stripping selection
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("EW")


#
# Lines from EW subgroup of flavour WG
# Tara Shears, James Keaveney
#

# James Keaveney (Z0)
from StrippingSelections import StrippingZ02MuMu
stream.appendLines( [  StrippingZ02MuMu.line,
                       StrippingZ02MuMu.lineloose,
                       StrippingZ02MuMu.lineNoPIDs ] )

from StrippingSelections import StrippingZ02ee
stream.appendLines( [ StrippingZ02ee.line ] )


# James Keaveney (W)
from StrippingSelections import StrippingWMu
stream.appendLines( [ StrippingWMu.lineNoPIDs_ps,
                      StrippingWMu.lineloose_ps,
                      StrippingWMu.line_ps,
                      StrippingWMu.lineloose,
                      StrippingWMu.line ] )

from StrippingSelections import StrippingWe
stream.appendLines( [ StrippingWe.line, StrippingWe.line_ps ] )


# James Keaveney (Drell-Yan)
from StrippingSelections import StrippingDY2MuMu
stream.appendLines( [ StrippingDY2MuMu.line1_ps,
                      StrippingDY2MuMu.line1_HLT2,
                      StrippingDY2MuMu.line2_HLT2,
                      StrippingDY2MuMu.line1_HLT2_a,
                      StrippingDY2MuMu.line2_HLT2_a,
                      StrippingDY2MuMu.line2,
                      StrippingDY2MuMu.line3,
                      StrippingDY2MuMu.line4 ] )

from StrippingSelections import StrippingDY2ee
stream.appendLines( [ StrippingDY2ee.line1,
                      StrippingDY2ee.line2,
                      StrippingDY2ee.line3,
                      StrippingDY2ee.line4 ] )
    

# Dermot Moran (DiPhotonDiMuon)
from StrippingSelections.StrippingDiPhotonDiMuon import line as DiPhotonDiMuonNoL0
from StrippingSelections.StrippingDiPhotonDiMuon import line_2 as DiPhotonDiMuonWithL0

stream.appendLines( [ DiPhotonDiMuonNoL0, DiPhotonDiMuonWithL0 ] )

#
# Lines from Exotics WG
# Clara Matteuzi
#

# Neal Gauvin, Victor Coco

from StrippingSelections.StrippingDisplVertices import line1HighMass as SingleHighMass
from StrippingSelections.StrippingDisplVertices import line1LowMass as SingleLowMass
from StrippingSelections.StrippingDisplVertices import line1 as SingleDisplVertexLine
from StrippingSelections.StrippingDisplVertices import line2 as DoubleDisplVertexLine

stream.appendLines( [ SingleDisplVertexLine,
                      DoubleDisplVertexLine, 
                      SingleHighMass,
                      SingleLowMass
                      ] )

# Marcin Kucharczyk

from StrippingSelections.StrippingHighPtJets import line1 as HighPtJetsLine
stream.appendLines( [ HighPtJetsLine ] )


from StrippingSelections import StrippingMuMuSS
stream.appendLines( [ StrippingMuMuSS.line1,
                      StrippingMuMuSS.line2,
                      StrippingMuMuSS.line3,
                      StrippingMuMuSS.line4
                      ] )
