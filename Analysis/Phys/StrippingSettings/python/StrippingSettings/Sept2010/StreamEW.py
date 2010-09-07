# $Id: StreamEW.py,v 1.2 2010-09-07 13:49:53 gcowan Exp $
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
from StrippingSelections import StrippingWMu, StrippingZ02MuMu, StrippingDY2MuMu
from StrippingSelections import StrippingWe, StrippingZ02ee, StrippingDY2ee
from StrippingSelections import StrippingMuMuSS

stream.appendLines( [
                          StrippingWMu.lineNoPIDs_ps,
                          StrippingWMu.lineloose_ps,
                          StrippingWMu.line_ps,
                          StrippingWMu.lineloose,
                          StrippingWMu.line ,
                          StrippingDY2MuMu.line1,
                          StrippingDY2MuMu.line1_ps,
                          StrippingDY2MuMu.line2,
                          StrippingDY2MuMu.line3,
                          StrippingDY2MuMu.line4,
                          StrippingZ02MuMu.line,
                          StrippingZ02MuMu.lineloose,
                          StrippingZ02MuMu.lineNoPIDs,
                          StrippingWe.line,
                          StrippingWe.line_ps,
                          StrippingDY2ee.line1,
                          StrippingDY2ee.line2,
                          StrippingDY2ee.line3,
                          StrippingDY2ee.line4,
                          StrippingZ02ee.line,
                          StrippingMuMuSS.line1,
                          StrippingMuMuSS.line2,
                          StrippingMuMuSS.line3,
                          StrippingMuMuSS.line4
                      ] )

# DiPhotonDiMuon
from StrippingSelections import  StrippingDiPhotonDiMuon
stream.appendLines( [ StrippingDiPhotonDiMuon.line ] )

#
# Lines from Exotics WG
# Clara Matteuzi
#
# Neal Gauvin
from StrippingSelections.StrippingDisplVertices import line1 as SingleDisplVertexLine
from StrippingSelections.StrippingDisplVertices import line2 as DoubleDisplVertexLine
#stream.appendLines( [ SingleDisplVertexLine, DoubleDisplVertexLine ] )

# Marcin Kucharczyk
from StrippingSelections.StrippingHighPtJets import line1 as HighPtJetsLine
#stream.appendLines( [ HighPtJetsLine ] )

