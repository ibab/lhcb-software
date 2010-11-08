# $Id: StreamDimuon.py,v 1.3 2010-09-08 09:05:18 gcowan Exp $
#
#   Dimuon stripping selection
#
#   all physics channels with dimuon in the final state
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Dimuon")

#
# Lines for the RD WG
# U. Egede
#

# Phillip Urquijo
from StrippingSelections.StrippingB2XMuMuSS import StrippingB2XMuMuSSConf
stream.appendLines( [ StrippingB2XMuMuSSConf().PiSS_line(),
                      StrippingB2XMuMuSSConf().PiOS_line(),
                      StrippingB2XMuMuSSConf().KSS_line(),
                      StrippingB2XMuMuSSConf().KOS_line() ] )

# Patrick Koppenburg
from StrippingSelections.StrippingBu2LLK import StrippingBu2LLKConf
from StrippingSelections.StrippingBu2LLK import defaultConfig as Bu2LLKConfig
Bu2LLKConf = StrippingBu2LLKConf( "Bu2mmK", config = Bu2LLKConfig )
stream.appendLines( [ Bu2LLKConf._mmKLine() ] )

#
# Lines from Bs2mumu WG
# Johannes Albrecht, Diego Martinez Santos
#

from StrippingSelections.StrippingBs2MuMuLines import Bs2MuMuLinesConf
Bs2MuMuConf = Bs2MuMuLinesConf('Bs2MuMu',Bs2MuMuLinesConf.config_default)
stream.appendLines( Bs2MuMuConf.lines )

from StrippingSelections.StrippingB2MuMuMuMuLines import B2MuMuMuMuLinesConf
Bs4MuConf = B2MuMuMuMuLinesConf('Bs24Mu',B2MuMuMuMuLinesConf.config_default)
stream.appendLines( Bs4MuConf.lines )

#
# Lines from BdKstarMuMu RD WG
# Tom Blake, Jeremy Dickens, Patrick Koppenburg
#

from StrippingSelections.StrippingBd2KstarMuMuTriggered_JD import StrippingBd2KstarMuMuConf_JD
from StrippingSettings.Stripping12.LineConfigDictionaries import Bd2KstarMuMuLines
from StrippingSettings.Stripping12.LineConfigDictionaries import Bd2KstarMuMuConfig

Bd2KstarMuMuConf = StrippingBd2KstarMuMuConf_JD( config = Bd2KstarMuMuConfig, activeLines = Bd2KstarMuMuLines )
stream.appendLines( Bd2KstarMuMuConf.lines() )

from StrippingSelections.StrippingBd2KstarMuMuLT import StrippingBd2KstarMuMuLTConf
from StrippingSelections.StrippingBd2KstarMuMuLT import defaultConfig as Bd2KstarMuMuLTConfig
Bd2KstarMuMuLTConf = StrippingBd2KstarMuMuLTConf( config = Bd2KstarMuMuLTConfig )
stream.appendLines( Bd2KstarMuMuLTConf.lines() )

# Liming Zhang
from StrippingSelections.StrippingBs2PhiMuMu import StrippingBs2PhiMuMuConf
stream.appendLines( [ StrippingBs2PhiMuMuConf().Bs2PhiMuMuLine(), 
                      StrippingBs2PhiMuMuConf().Bs2f0MuMuLine() ] )


# Vanya Belayev
from StrippingSelections.StrippingTriMuons import StrippingTriMuonsConf
stream.appendLines (  StrippingTriMuonsConf({}).lines() )

# Paul Schaak and Vanya Belayev
from StrippingSelections.StrippingBs2MuMuD import StrippingBs2MuMuDConf
from StrippingSelections.StrippingBs2MuMuD import defaultConfig as Bs2MuMuDConfig

Bs2MuMuDConf = StrippingBs2MuMuDConf(config = Bs2MuMuDConfig)
stream.appendLines(Bs2MuMuDConf.lines())

from StrippingSelections.StrippingBs2MuMuPhi import StrippingBs2MuMuPhiConf
from StrippingSelections.StrippingBs2MuMuPhi import defaultConfig as Bs2MuMuPhiConfig

Bs2MuMuPhiConf = StrippingBs2MuMuPhiConf(config = Bs2MuMuPhiConfig)
stream.appendLines(Bs2MuMuPhiConf.lines())

# Thomas Blake (Inclusive RD lines)
from StrippingSelections.StrippingB2DiMuonX import StrippingB2DiMuonXConf
from StrippingSelections.StrippingB2DiMuonX import defaultConfig as DiMuonXConfig
DiMuonXConf = StrippingB2DiMuonXConf( config = DiMuonXConfig )
stream.appendLines( DiMuonXConf.lines() )

#
# Lines from Charm RD
# Walter Bonivento
#

from StrippingSelections.StrippingJpsiMuMuforD0MuMu import JpsiMuMuforD0MuMuConf
D0MuMuConfJPsi =  JpsiMuMuforD0MuMuConf( config = JpsiMuMuforD0MuMuConf.config_default )
stream.appendLines( D0MuMuConfJPsi.Lines )

#
# Ulrich Kerzel (Lines using NeuroBayes NN)
#
from StrippingSelections.StrippingNeuroBayesMuMu import StrippingNeuroBayesMuMuConf
stream.appendLines ( StrippingNeuroBayesMuMuConf().lines() )

#
# Lines from Flavour WG
# Patrick Robbe, Jibo He
#
# Jpsi2MuMu lines for the full Dimuon streamed DSTs
from StrippingSelections.StrippingDiMuonNew import DiMuonConf
DiMuonStreamDiMuonConf = DiMuonConf( config = DiMuonConf.config_default )
stream.appendLines( DiMuonStreamDiMuonConf.DiMuonStreamLines )

# Nicola Mangiafave
from StrippingSelections.StrippingBu2KJpsiPiPi import StrippingBu2KJpsiPiPiConf
stream.appendLines( [ StrippingBu2KJpsiPiPiConf().loose_line(),
                      StrippingBu2KJpsiPiPiConf().X3872_line(),
                      StrippingBu2KJpsiPiPiConf().Psi2S_line()] )

# Jibo He 
from StrippingSelections.StrippingBc2JpsiMuX import StrippingBc2JpsiMuXConf
stream.appendLines( [ StrippingBc2JpsiMuXConf().MuMu() ] )

from StrippingSelections.StrippingBc2JpsiH import StrippingBc2JpsiHConf
stream.appendLines( [ StrippingBc2JpsiHConf().MuMu()  ] )

from StrippingSelections.StrippingBc2JpsiHDetached import StrippingBc2JpsiHDetachedConf
stream.appendLines( [ StrippingBc2JpsiHDetachedConf().MuMu() ] )


#
# Lines from beta_s WG
# Olivier Leroy, Wouter Hulsebergen and Greig Cowan
#

# Greig Cowan, Juan Palacios
from StrippingSelections.StrippingB2JpsiXforBeta_s import B2JpsiXLines
stream.appendLines( B2JpsiXLines )

# Fred Blanc, Neus Lopez March
from StrippingSelections.StrippingB2Psi2SX import B2Psi2SXJpsiPiPiLines
stream.appendLines( B2Psi2SXJpsiPiPiLines )
from StrippingSelections.StrippingB2Psi2SXMuMu import B2Psi2SXMuMuLines
stream.appendLines( B2Psi2SXMuMuLines )

# Yasmine Ahmis
from StrippingSelections.StrippingLambdab2JpsiLambda import StrippingLambdab2JpsiLambdaConf
stream.appendLines( [ StrippingLambdab2JpsiLambdaConf().line_LL(),
                      StrippingLambdab2JpsiLambdaConf().line_DD() ])

# Liming Zhang
from StrippingSelections.StrippingBs2Jpsif0 import StrippingBs2Jpsif0Conf
stream.appendLines( [ StrippingBs2Jpsif0Conf().nominal_line(),
                      StrippingBs2Jpsif0Conf().nominalKst_line()  ])

# Basem Khanji, Olivier Leroy
from StrippingSelections.StrippingBiasedBs2JpsiPhi import line
stream.appendLines( [ line ] )

