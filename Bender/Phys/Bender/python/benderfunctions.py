# =============================================================================
# $Id: benderfunctions.py,v 1.4 2006-11-16 13:50:15 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.4 $
# =============================================================================
""" Auxillary module to load few major LoKi/Bender fuctions  """
# =============================================================================
## @file
#  Auxillary module to load few major LoKi/Bender fuctions 
#  Auxillary module  to decorate basic bender algorithm 
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"


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
