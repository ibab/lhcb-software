# =============================================================================
# $Id: bendermatch.py,v 1.4 2006-11-09 14:10:37 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.4 $
# =============================================================================
""" Auxillary module  to decorate class LoKi::MCMatch """
# =============================================================================
## @file
#  Auxillary module  to decorate class LoKi::MCMatch 
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
MCMatch = _LoKi.MCMatch

## get the Bender decorator 
_BM = _Bender.MCMatch

def _valid_(s)    : return _BM.valid(s)
def _match_(s,*a) : return _BM.match(s,*a)

MCMatch.valid = _valid_
MCMatch.match = _match_ 

if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 

