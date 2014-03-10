#!/usr/bin/env ipython
# ==========================================================================================#
# $Id:$
# ==========================================================================================
## @file  SelectorWithVars.py
#
#  Generic selector to fill RooDataSet from TChain/TTree
#  - the variables can be specified generically 
#  - ther selection is equipped with the progress bar 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date   2014-03-02
#  @author Vanya     BELYAEV Ivan.Belyaev@itep.ru
#  @thanks Alexander BARANOV a.baranov@cern.ch
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$ 
# =============================================================================
"""

Generic selector to fill RooDataSet form TChain/TTree 

- the variables can be specified generically 
- ther selection is equipped with the progress bar 

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV  Ivan.Belyaev@itep.ru'
__date__    = "2014-03-02"
__version__ = '$Revision:$'
__all__     = (
    "SelectorWithVars"
    )
# =============================================================================
import ROOT
# =============================================================================
import warnings
warnings.warn (
    """AnalysisPython:
    Use 'PyPAW.Selectors' module instead of 'AnalysisPython.SelectorWithVars'""",
    DeprecationWarning ,
    stacklevel   = 3
    )
# =============================================================================
## the actual import 
from PyPAW.Selectors import *

# =============================================================================
if '__main__' == __name__ :

    print 100*'*'
    print __doc__
    print 100*'*'
    print ' Author  : ' , __author__
    print ' Date    : ' , __date__
    print ' Version : ' , __version__
    print 100*'*'
    
# =============================================================================
# The END 
# =============================================================================
