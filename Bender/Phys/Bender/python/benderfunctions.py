# =============================================================================
# $Id: benderfunctions.py,v 1.5 2006-11-28 18:24:17 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.5 $
# =============================================================================
## Auxillary module to load few major LoKi/Bender fuctions
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
""" Auxillary module to load few major LoKi/Bender fuctions  """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================


import gaudimodule

_gbl    = gaudimodule.gbl
_LoKi   = _gbl.LoKi
_Bender = _gbl.Bender

printDecay      = _Bender.PrintDecay.printDecay
printMCDecay    = _Bender.PrintDecay.printMCDecay
printHepMCDecay = _Bender.PrintDecay.printHepMCDecay

antiParticle    = _LoKi.Particles.antiParticle
lifeTime        = _LoKi.Particles.lifeTime
massFromName    = _LoKi.Particles.massFromName 
massFromPID     = _LoKi.Particles.massFromPID 
ppFromPID       = _LoKi.Particles.ppFromPID 
ppFromName      = _LoKi.Particles.ppFromName
nameFromPID     = _LoKi.Particles.nameFromPID 
pidFromName     = _LoKi.Particles.pidFromName


# =============================================================================
if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 
# =============================================================================


# =============================================================================
# The END 
# =============================================================================
