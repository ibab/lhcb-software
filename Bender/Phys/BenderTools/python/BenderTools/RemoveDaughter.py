#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# @file BenderTools/RemoveDaughter.py
#
#  Helper utulity for temporary removal of daughter particle form decay tree
#  @code
#
#  mass = DTF_FUN ( M , True , 'J/psi(1S)') ## calculate the mass with constrains
#  for B in container :
#
#      mass_B  = mass ( B )  ## calcualte the mass with all constraints
#
#      chic  = B   (1) ## get chi_c
#      gamma = chic(2) ## get gamma 
#
#      print 'before:', B.decay()
#      with RemoveDaughter ( chic , gamma ) :
#         print 'inside:', B.decay()
#         mass_prime = mass ( B ) ## calculate the mass after removal of photon
#      print 'after:', B.decay()
#  
#  @endcode
#  @thanks Wouter    HULSBERGEN
#  @thanks Alexander BARANOV 
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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-12
#
#                   $Revision: 169835 $
# Last modification $Date: 2014-03-09 14:25:30 +0100 (Sun, 09 Mar 2014) $
#                by $Author: ibelyaev $
# =============================================================================
"""
Helper utulity for temporary removal of daughter particle form decay tree
#
#  mass = DTF_FUN ( M , True , 'J/psi(1S)') ## calculate the mass with constrains
#  for B in container :
#
#      mass_B  = mass ( B )  ## calcualte the mass with all constraints
#      
#      chic  = B   (1) ## get chi_c
#      gamma = chic(2) ## get gamma 
#
#      print 'before:', B.decay()
#      with RemoveDaughter ( chic , gamma ) :
#         print 'inside:', B.decay()
#         mass_prime = mass ( B ) ## calculate the mass after removal of photon
#      print 'after:', B.decay()
#
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
__version__ = "$Revision: 169835 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-05-12"
# =============================================================================
__all__     = ( 'RemoveDaughter' )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# ==============================================================================
from   LoKiCore.basic  import cpp,LHCb
import LoKiPhys.Phys 
## smart reference class
_SR  = cpp.SmartRef       ( LHCb.Particle )
_SRV = cpp.SmartRefVector ( LHCb.Particle )
## add proper __nonzero__
_SR .__nonzero__ = lambda s : bool( s.target() )
# ==============================================================================
## @class RemoveDaughter 
#  Helper utulitiy for temporary removal of daughter particle form decay tree
#  @code
#
#  mass = DTF_FUN ( M , True , 'J/psi(1S)') ## calculate the mass with constrains
#  for B in container :
#
#      mass_B  = mass ( B )  ## calcualte the mass with all constraints
#      
#      chic  = B   (1) ## get chi_c
#      gamma = chic(2) ## get gamma 
#
#      print 'before:' , B.decay()
#      with RemoveDaughter ( chic , gamma ) :
#         print 'inside:' , B.decay()
#         mass_prime = mass ( B ) ## calculate the mass after removal of photon
#      print 'after:' , B.decay()
#
#  @endcode
#  @attention Only direct daughter are supported 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-05-12
#  @todo Update for ``correct'' signature of LHCb.Particle.daughters 
class RemoveDaughter ( object ) :
    """
    Helper class to substitute PID for the basic particle
    #
    #
    #  mass = DTF_FUN ( M , True , 'J/psi(1S)') ## calculate the mass with constrains
    #  for B in container :
    #
    #      mass_B  = mass ( B )  ## calcualte the mass with all constraints
    #      
    #      chic  = B   (1) ## get chi_c
    #      gamma = chic(2) ## get gamma 
    #
    #      print 'before:' , B.decay()
    #      with RemoveDaughter ( chic , gamma ) :
    #         print 'inside:' , B.decay()
    #         mass_prime = mass ( B ) ## calculate the mass after removal of photon
    #      print 'after:' , B.decay()
    #
    """
    ## constructor
    def __init__  ( self , particle , daughter ) :

        assert particle, 'RemoveDaughter: mother   particle is invalid!'
        assert daughter, 'RemoveDaughter: daughter particle is invalid!'
        
        self._particle =       particle
        self._daughter = _SR ( daughter ) 
        self._vertex   = particle.endVertex()
        
        if not self._vertex   :
            logger.warning ( 'mother vertex is invalid')
            
    def __enter__ ( self ) :
            
        if self._particle and self._daughter :
            self._daughters = self._particle.daughters()
            self._particle.removeFromDaughters       ( self._daughter  )
            assert len ( self._daughters ) == len ( self._particle.daughters() ) + 1, 'Invalid size of daughters!'
                
        if self._vertex   and self._daughter :                
            self._outgoing = _SRV( self._vertex   .outgoingParticles() )
            self._vertex.removeFromOutgoingParticles ( self._daughter  )
            #if len ( self._outgoing ) != len ( self._vertex.outgoingParticles() ) + 1 : 
            #    logger.warning ( 'Incorrect size of outgoingParticles' )
                        
        return self
        
    def __exit__  ( self, *_ ) :

        if self._particle and self._daughter :
            self._particle.clearDaughters()
            for i in self._daughters : self._particle.addToDaughters ( i )
            ## can be more efficient, after the fix in LoKiPhys.Phys
            ## self._particles.setDaughters      ( self._daughters )
                
        if self._vertex   and self._daughter :
            self._vertex.setOutgoingParticles ( self._outgoing ) 
            
        self._particle  = None
        self._daughter  = None 
        self._vertex    = None
        
        self._daughters = None 
        self._outgoing  = None 

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'

    
    ##
    ## simple check
    ## 
    p   = LHCb.Particle()
    pr1 = _SR( p )
    pr2 = _SR(   )
    
    assert     pr1, 'Invalid smart reference (1) ! ' 
    assert not pr2, 'Invalid smart reference (2) ! ' 

    try    :
        assert not pr1, 'Assertion(1) ok!'
    except AssertionError as e :
        print e
        
    try    :
        assert     pr2, 'Assertion(2) ok!'
    except AssertionError  as e :
        print e

    print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================
