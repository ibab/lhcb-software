# =============================================================================
# $Id: bendermatch.py,v 1.6 2006-11-28 18:24:17 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.6 $
# =============================================================================
## Auxillary module  to decorate class LoKi::MCMatch
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
#  @see Bender::MCMatch
#  @see LoKi::MCMatch
#  @see LoKi::MCMatchObj
#
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" Auxillary module  to decorate class LoKi::MCMatch """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

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

