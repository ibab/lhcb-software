# =============================================================================
# $Id: benderloop.py,v 1.8 2006-10-11 14:45:08 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.8 $
# =============================================================================
""" Auxillary module  to decorate basic bender algorithm """
# =============================================================================
## @file
#  Auxillary module  to decorate basic bender algorithm 
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"

import gaudimodule

_gbl    = gaudimodule.gbl
_LoKi   = _gbl.LoKi
_Bender = _gbl.Bender 


## the loop object itself 
Loop    = _LoKi.Loop

## its decorator 
_BL     = _Bender.Loop

##get the effective formula of the loop, @see LoKi::LoopObj::formula
Loop.formula  = _BL.formula
## get the effective dimension of the loop (== number of components)
Loop.dim      = _BL.dim
## get the effective dimension of the loop (== number of components)
Loop.size     = _BL.size
## get the status of loop object
Loop.status   = _BL.status 
##  advance the loop to the next combination
Loop.next     = _BL.next 
## conversion operator to LHCb::Particle and access to the children:
Loop.particle = _BL.particle
Loop.daughter = Loop.child
Loop.son      = Loop.child 
## get the momentum of the particle or daughter or combinarions
Loop.momentum = _BL.momentum
## get the invariant mass of the particel or child
Loop.mass     = _BL.mass
Loop.m        = _BL.mass 
## get the effective PID
Loop.pid      = _BL.pid 
## get PID for the effective particle of the loop
Loop.pp       = _BL.pp 
## get PID for the effective particle of the loop
Loop.pidName  = _BL.pidName
## set the particle ID for the effective particle of the loop
Loop.setPID   = _BL.setPID
## set the default IParticleCombiner tool
Loop.setCombiner = _BL.setCombiner
## set the default IParticleCombiner tool
Loop.setReFitter = _BL.setReFitter
## get the associated vertex
Loop.pv          = _BL.pv
## set the associated vertex
Loop.setPV       = _BL.setPV
## make 'effective' particle using IParticleCombiner tool
Loop.make        = _BL.make
## refit the particle using IParticleReFitter tool
Loop.reFit       = _BL.reFit
## save the particle into LoKi storage
Loop.save        = _BL.save
## backup the current state of the loop
Loop.backup      = _BL.backup
## restore the loop from the last backup state
Loop.restore     = _BL.restore

print "dir(%s) : %s" % ( __name__ , dir() ) 
