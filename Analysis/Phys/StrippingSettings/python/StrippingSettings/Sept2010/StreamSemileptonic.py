# $Id: StreamSemileptonic.py,v 1.2 2010-09-07 13:49:53 gcowan Exp $
#
#   Semileptonic stripping selection
#
#   all physics channels with semileptonic final states
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Semileptonic")

#
# Lines from semileptonic task force
# Rob Lambert and Kim Vervink
#
from StrippingSelections.StrippingB2XuMuNu import StrippingB2XuMuNuConf
stream.appendLines( [ StrippingB2XuMuNuConf().K_line()
                    , StrippingB2XuMuNuConf().Pi_line()
                    , StrippingB2XuMuNuConf().PiSS_line()
                    , StrippingB2XuMuNuConf().Rho_line()
                    , StrippingB2XuMuNuConf().RhoWS_line()
                    , StrippingB2XuMuNuConf().KSS_line()
                    , StrippingB2XuMuNuConf().Kstar_line()
                    , StrippingB2XuMuNuConf().KstarSS_line() ] )

from StrippingSelections.StrippingB0q2DplusMuX import B0q2DplusMuXAllLinesConf 
from StrippingSelections.StrippingB0q2DplusMuX import confdict as B0q2DplusMuXAllConfDict
stream.appendLines( B0q2DplusMuXAllLinesConf(B0q2DplusMuXAllConfDict).Lines )

from StrippingSelections.StrippingB2DMuNuX import StrippingB2DMuNuXConf
stream.appendLines( [ StrippingB2DMuNuXConf().b2D0MuXLine()
                    , StrippingB2DMuNuXConf().b2DpMuXLine()
                    , StrippingB2DMuNuXConf().b2DsMuXLine()
                    , StrippingB2DMuNuXConf().b2LcMuXLine() ])

from StrippingSelections.StrippingBd2DstarMuNu import line as Bd2DstarMuNuLine
from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
stream.appendLines( [ Bd2DstarMuNuLine ] )
stream.appendLines( [ StrippingBd2DstarMuNuLooseConf().line() ] )

