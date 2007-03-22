# =============================================================================
# $Id: bendermodule.py,v 1.35 2007-03-22 18:50:47 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.35 $ 
# =============================================================================
## The major Python module for Bender application 
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
""" This is a major Python Module for Bender application """
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
# =============================================================================

import os 

try:
    ## try to get the startup script from ebvironment 
    startup = os.environ.get('PYTHONSTARTUP',None)
    ## use the default startup script
    if not startup :
        startup = os.sep + 'benderstartup.py'
        startup = os.environ['BENDERPYTHON'] + startup 
    if os.path.exists( startup ) : execfile( startup )    
except:
    pass

import sets 
import gaudimodule

import benderaux

## load all defined dictionary libraries
benderaux._loadDict_( [ 'HepMCRflx'      ,
                        'CLHEPRflx'      ,
                        'STLRflx'        ,
                        'MathRflx'       ,
                        'TrackEventDict' ] )

## keep the application maner  
AppMgr = gaudimodule.AppMgr 

## get the global namespace
gbl    = gaudimodule.gbl 

## create Gaudi application manager (if not done yet)  
gaudi  = AppMgr()

## use some shortcuts 
appMgr = gaudi     #
g      = gaudi     # for 'backward compatibility'
theApp = gaudi     # ATLAS style

## load some important DLLs for Bender/LoKi 
## benderaux._loadDll_ ( [ 'LoKiCore'            ,
##                         'LoKiGenMC'           , 
##                         'LoKiPhysMC'          ,  
##                         'LoKiJets'            ,
##                         'DaVinciKernel'       ,
##                         'DaVinciTools'        ,      
##                         'DaVinciMCTools'      ,      
##                         'DaVinciTransporter'  ,      
##                         'VertexFit'           ,      
##                         'ParticleMaker'       ,      
##                         'ProtoParticleFilter' ,      
##                         'DaVinciFilter'       ,      
##                         'DaVinciAssociators'  ]  , gaudi ) 

## declare LoKi-service ( needed for many algorithms and functions) 
if not "LoKiSvc" in gaudi.ExtSvc : gaudi.ExtSvc += [ "LoKiSvc" ]

## @var LoKi   : define namespace LoKi 
LoKi   = gaudimodule.gbl.LoKi
## @var Bender : define namespace Bender 
Bender = gaudimodule.gbl.Bender
## @var Bender : define namespace Bender 
LHCb   = gaudimodule.gbl.LHCb
## @var Gaudi  : define namespace Gaudi
Gaudi  = gaudimodule.gbl.Gaudi

from benderrange     import * 
from benderfuncs     import * 
from benderalgo      import *
from benderloop      import *
from bendermatch     import *
from benderfinder    import *
from benderpcuts     import * 
from bendervcuts     import * 
from bendermccuts    import * 
from bendermcvcuts   import * 
from bendergcuts     import * 
from bendergvcuts    import * 
from benderfunctions import * 

_SC=gbl.StatusCode
SUCCESS = _SC(_SC.SUCCESS)
FAILURE = _SC(_SC.FAILURE)

_SE = gbl.StatEntity
_iadd_old_ = _SE.__iadd__
def _iadd_new_ (s,v) : _iadd_old_(s,float(v))
_SE.__iadd__ = _iadd_new_

## run events 
def run (n) :
    """ run gaudi """
    return gaudi.run ( n )

## IMPORTANT, probably it is the most important line...
decorateFunctors ( __name__ )

## Welcome message:
Bender.Welcome.instance()

if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 


# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.34  2006/11/28 18:24:17  ibelyaev
#  prepare for v6r1
#
# =============================================================================
# The END 
# =============================================================================
