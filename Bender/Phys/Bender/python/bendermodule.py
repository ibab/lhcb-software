# =============================================================================
# $Id: bendermodule.py,v 1.30 2006-10-11 14:45:11 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
""" This is a major Python Module for Bender application """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================
## @file
#
#  The major Python module for Bender application 
#
#  @date   2004-07-11
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import os 

try:
    ## try to get the startup script from ebvironment 
    startup = os.environ.get('PYTHONSTARTUP',None)
    ## use the default startup script
    if not startup : startup = benderstartup.py 
    if os.path.exist( startup ) : execfile( startup )    
except:
    pass

import gaudimodule

import benderaux

## load all defined dictionary libraries
benderaux._loadDict_( 'HepMCRflx' )
#benderaux._loadDict_( ['BenderDict' ,'PhysEventDict' , 'MCEventDict'] )

## keep the applioation maner  
AppMgr = gaudimodule.AppMgr 

## create Gaudi application manager (if not done yet)  
gaudi  = AppMgr()

## use some shortcuts 
appMgr = gaudi     #
g      = gaudi     # for 'backward compatibility'
theApp = gaudi     # ATLAS style

## load some important DLLs for Bender/LoKi 
benderaux._loadDll_ ( [ 'LoKiCore'   ,
                        'LoKiGenMC'  , 
                        'LoKiPhysMC' ,  
                        'LoKiJets'   ] , appMgr = appMgr  ) 

## @var LoKi   : define namespace LoKi 
LoKi   = gaudimodule.gbl.LoKi
## @var Bender : define namespace Bender 
Bender = gaudimodule.gbl.Bender
## @var Bender : define namespace Bender 
LHCb   = gaudimodule.gbl.LHCb
## @var Gaudi  : define namespace Gaudi
Gaudi  = gaudimodule.gbl.Gaudi

from benderrange   import * 
from benderfuncs   import * 
from benderalgo    import *
from benderloop    import *
from bendermatch   import *
from benderfinder  import *
from benderpcuts   import * 
from bendervcuts   import * 
from bendermccuts  import * 
from bendermcvcuts import * 
from bendergcuts   import * 
from bendergvcuts  import * 


decorateFunctors ( __name__ )

if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 

# =============================================================================
# The END 
# =============================================================================
