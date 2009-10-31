#!/usr/bin/env
# =============================================================================
## @file KaliCalo/Pi0.py
#  Example of script for iterative (pi0 -> gamma gamma) Ecal calibration
#  @author Vanya BELYAEV Ivan.Belayev@nikhef.nl
#  @date 2009-10-31
# =============================================================================
"""
Example of script for iterative ( pi0 -> gamma gamma) Ecal calibration
"""
# =============================================================================
__author__  =  "Vanya BELYAEV Ivan.Belayev@nikhef.nl"
__version__ =  "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================

import ROOT

c1 = ROOT.TCanvas('canvas','canvas',600,500)

from   GaudiPython.Bindings import gbl as cpp
import GaudiPython.HistoUtils

from   KaliCalo.Pi0FillHistos import processFile as process 
from   KaliCalo.Pi0FillHistos import cell0
from   KaliCalo.Pi0HistoFit   import fitPi0Histo

LHCb = cpp.LHCb
AIDA = cpp.AIDA

# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__

    calibr = {}
    histos = {}

    result = process ( 'KaliPi0_Tuples.root' , calibr , histos )

    histos = result[1] 
    if histos.has_key ( cell0 ) : 
        histo = histos[cell0]
        print fitPi0Histo ( histo , 0.050 , 0.220 )
        if hasattr ( histo , 'dump' ) : print histo.dump(40,20,True) 
        histo.Draw()
        
# =============================================================================
# The END 
# =============================================================================
