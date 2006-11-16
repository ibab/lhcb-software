# =============================================================================
# $Id: benderloop.py,v 1.11 2006-11-16 13:50:15 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.11 $
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
def _formula_  ( s )      : return _BL.formula  ( s )  
Loop.formula  = _formula_
## get the effective dimension of the loop (== number of components)
def _dim_      ( s )      : return _BL.dim      ( s ) 
Loop.dim      = _dim_ 
## get the estimate of number of combinations 
def _size_     ( s )      : return _BL.size     ( s ) 
Loop.size     = _size_  
## get the status of loop object
def _status_   ( s )      : return _BL.status   ( s ) 
Loop.status   = _status_ 
##  advance the loop to the next combination
def _next_     ( s )      : return _BL.next     ( s ) 
Loop.next     = _next_ 
## conversion operator to LHCb::Particle and access to the children:
def _particle_ ( s , *a ) : return _BL.particle ( s , *a ) 
Loop.particle = _particle_
Loop.child    = Loop.particle 
Loop.daughter = Loop.particle
Loop.son      = Loop.particle
## get the momentum of the particle or daughter or combinarions
def _momentum_ ( s , *a ) : return _BL.momentum ( s , *a ) 
Loop.momentum = _momentum_
## get the invariant mass of the particel or child
def _mass_     ( s , *a ) : return _BL.mass     ( s , *a ) 
Loop.mass     = _mass_   
Loop.m        = Loop.mass 
## get the effective PID
def _pid_      ( s )      : return _BL.pid      ( s ) 
Loop.pid      = _pid_ 
## get PID for the effective particle of the loop
def _pp_       ( s )      : return _BL.pp       ( s ) 
Loop.pp       = _pp_   
## get PID for the effective particle of the loop
def _pidName_  ( s )      : return _BL.pidName  ( s ) 
Loop.pidName  = _pidName_
## set the particle ID for the effective particle of the loop
def _setPID_   ( s , *a ) : return _BL.setPID   ( s , *a ) 
Loop.setPID   = _setPID_ 
## set the default IParticleCombiner tool
def _setCom_   ( s , *a ) : return _BL.setCombiner ( s , *a ) 
Loop.setCombiner = _setCom_        
## set the default IParticleReFitter tool
def _setFit_   ( s , *a ) : return _BL.setReFitter ( s , *a ) 
Loop.setReFitter = _setFit_ 
## get the associated vertex
def _pv_       ( s )      : return _BL.pv       ( s )  
Loop.pv          = _pv_ 
## set the associated vertex
def _setPV_    ( s , *a ) : return _BL.setPV    ( s , *a ) 
Loop.setPV       = _setPV_ 
## make 'effective' particle using IParticleCombiner tool
def _make_     ( s , *a ) : return _BL.make     ( s , *a ) 
Loop.make        = _make_ 
## refit the particle using IParticleReFitter tool
def _reFit_    ( s , *a ) : return _BL.reFit    ( s , *a ) 
Loop.reFit       = _reFit_   
## save the particle into LoKi storage
def _save_     ( s , *a ) : return _BL.save     ( s , *a ) 
Loop.save        = _save_    
## backup the current state of the loop
def _backup_   ( s )      : return _BL.backup   ( s )      
Loop.backup      = _backup_  
## restore the loop from the last backup state
def _restore_  ( s )      : return _BL.restore  ( s )      
Loop.restore     = _restore_   


def _iter_ ( s ) :
    loop = s
    while loop.valid() :
        yield loop
        loop.next()

Loop.__iter__ = _iter_

# =============================================================================
if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 
# =============================================================================


# =============================================================================
# The END 
# =============================================================================
