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
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingB2Charged2Body.py")


importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingB2DPi.py")

# Rob's selection
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingBd2KstarMuMu.py")

# Greig's selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingBd2JpsiKstar.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingBd2JpsiKS.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingBs2JpsiPhi.py")
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingBu2JpsiK.py")

# Topological selections
importOptions( "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingTopo.py")
