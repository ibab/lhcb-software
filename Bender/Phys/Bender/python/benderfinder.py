# =============================================================================
# $Id: benderfinder.py,v 1.4 2006-11-28 18:24:17 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.4 $
# =============================================================================
""" Auxillary module  to decorate class LoKi::MCFinder """
# =============================================================================
## Auxillary module  to decorate class LoKi::MCFinder
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"

import os
import gaudimodule

_gbl    = gaudimodule.gbl
_LoKi   = _gbl.LoKi
_Bender = _gbl.Bender

## get the object itsels 
MCFinder = _LoKi.MCFinder

## get the Bender decorator 
_BF = _Bender.MCFinder

def _valid_(s)         : return _BF.valid      (s)
def _findDecays_(s,*a) : return _BF.findDecays (s,*a)

MCFinder.valid      = _valid_
MCFinder.findDecays = _findDecays_ 
MCFinder.findDecay  = MCFinder.findDecays
MCFinder.find       = MCFinder.findDecays 
MCFinder.decays     = MCFinder.findDecays 
MCFinder.decay      = MCFinder.findDecays 

if '__main__' == __name__ :
    print "dir(%s) : %s" % ( __name__ , dir() ) 
