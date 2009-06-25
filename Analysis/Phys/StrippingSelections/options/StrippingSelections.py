#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *

#
# Import stripping selections
#

# Angelo's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingB2Charged2Body.py")

# Vava and Jacopo's B->Dh selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingB2DPi.py")

# Rob's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2KstarMuMu.py")

# Greig's selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2JpsiKstar.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBd2JpsiKS.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBs2JpsiPhi.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBu2JpsiK.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingBs2JpsiPhiPrescaledAndDetached.py")

# Topological selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingTopo.py")

# Andrew Powell's RICH calibration selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/StrippingDstarNoPID.py")
