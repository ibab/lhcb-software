# =============================================================================
# $Id: benderfinder.py,v 1.1 2006-10-11 14:45:07 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1 $
# =============================================================================
""" Auxillary module  to decorate class LoKi::MCFinder """
# =============================================================================
## @file
#  Auxillary module  to decorate class LoKi::MCFinder 
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
MCFinder.findDecay  = _findDecays_ 
MCFinder.find       = _findDecays_ 
MCFinder.decays     = _findDecays_ 

print "dir(%s) : %s" % ( __name__ , dir() ) 
