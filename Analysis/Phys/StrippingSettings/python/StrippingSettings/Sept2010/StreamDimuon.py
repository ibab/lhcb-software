# $Id: StreamDimuon.py,v 1.1 2010-09-07 11:57:00 gcowan Exp $
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
# Lines from beta_s WG
# Olivier Leroy, Wouter Hulsebergen and Greig Cowan
#
from StrippingSelections.StrippingB2JpsiXforBeta_s import B2JpsiXLines
stream.appendLines( B2JpsiXLines )

from StrippingSelections.StrippingLambdab2JpsiLambda import StrippingLambdab2JpsiLambdaConf
stream.appendLines( [ StrippingLambdab2JpsiLambdaConf().line_LL(),
                      StrippingLambdab2JpsiLambdaConf().line_DD() ])

from StrippingSelections.StrippingBs2Jpsif0 import StrippingBs2Jpsif0Conf
stream.appendLines( [ StrippingBs2Jpsif0Conf().nominal_line(),
                      StrippingBs2Jpsif0Conf().nominalKst_line()  ])

#
# Lines from RD WG
# Ulrik Egede 
#
# Tom Blake, Jeremy Dickens 
from StrippingSelections.StrippingBd2KstarMuMuTriggered import StrippingBd2KstarMuMuConf
from StrippingSelections.StrippingBd2KstarMuMuTriggered import defaultConfig as Bd2KstarMuMuConfig
from StrippingSelections.StrippingBd2KstarMuMuTriggered import defaultLines as Bd2KstarMuMuLines
Bd2KstarMuMuConf = StrippingBd2KstarMuMuConf( config = Bd2KstarMuMuConfig, activeLines = Bd2KstarMuMuLines )
stream.appendLines( Bd2KstarMuMuConf.lines() )

# Patrick Koppenburg
from StrippingSelections.StrippingBd2KstarMuMuLT import StrippingBd2KstarMuMuLTConf
from StrippingSelections.StrippingBd2KstarMuMuLT import defaultConfig as Bd2KstarMuMuLTConfig
Bd2KstarMuMuLTConf = StrippingBd2KstarMuMuLTConf( config = Bd2KstarMuMuLTConfig )
stream.appendLines( Bd2KstarMuMuLTConf.lines() )

from StrippingSelections.StrippingBu2LLK import StrippingBu2LLKConf
from StrippingSelections.StrippingBu2LLK import defaultConfig as Bu2LLKConfig
Bu2LLKConf = StrippingBu2LLKConf( config = Bu2LLKConfig )
stream.appendLines( Bu2LLKConf.lines() )

# Liming Zhang
from StrippingSelections.StrippingBs2PhiMuMu import StrippingBs2PhiMuMuConf
stream.append( [ StrippingBs2PhiMuMuConf().Bs2PhiMuMuLine() ] )
stream.append( [ StrippingBs2PhiMuMuConf().Bs2f0MuMuLine() ] )

# Vanya Belayev
from StrippingSelections.StrippingTriMuons import Lines as ThreeMuonLines
stream.appendLines ( ThreeMuonLines )

#
# Lines from Bs2mumu WG
# Johannes Albrecht
#
from StrippingSelections.StrippingBs2MuMuLines import Bs2MuMuLinesConf
stream.appendLines( Bs2MuMuLinesConf().lines )

#
# Lines from Charm RD WG
# Walter Bonivento
#
from StrippingSelections.StrippingD02MuMu import lines as D02MuMuLines
stream.append( [ D02MuMuLines ] )

#
# Lines from Flavour WG
# Patrick Robbe, Jibo He
#
from StrippingSelections.StrippingDiMuon import StrippingDiMuonConf
stream.appendLines( [ StrippingDiMuonConf().nominal_line() ] )
stream.appendLines( [ StrippingDiMuonConf().likesign_line() ] )

# Nicola Mangiafave
from StrippingSelections.StrippingBu2KJpsiPiPi import StrippingBu2KJpsiPiPiConf
stream.appendLines( [ StrippingBu2KJpsiPiPiConf().loose_line() ] )
stream.appendLines( [ StrippingBu2KJpsiPiPiConf().X3872_line() ] )
stream.appendLines( [ StrippingBu2KJpsiPiPiConf().Psi2S_line() ] )

# Jibo He
from StrippingSelections.StrippingBc2JpsiH import StrippingBc2JpsiHConf
stream.appendLines( [ StrippingBc2JpsiHDetachedConf().MuMu() ] )

from StrippingSelections.StrippingBc2JpsiHDetached import StrippingBc2JpsiHDetachedConf
stream.appendLines( [ StrippingBc2JpsiHConf().MuMu() ] )

from StrippingSelections.StrippingBc2JpsiMuX import StrippingBc2JpsiMuXConf
stream.appendLines( [ StrippingBc2JpsiMuXConf().MuMu() ] )

