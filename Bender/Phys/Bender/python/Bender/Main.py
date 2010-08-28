#!/usr/bin/env python 
# =============================================================================
## The major Python module for Bender application 
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
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
This is a major Python Module for Bender application

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2004-07-11"
__version__ = ' CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.11 $' 
# =============================================================================

import os 

try:
    ## try to get the startup script from environment 
    startup = os.environ.get('PYTHONSTARTUP',None)
    ## use the default startup script
    if not startup :
        startup = os.sep + 'Bender' + os.sep + 'Startup.py'
        bp = os.environ.get('BENDERPYTHON',None)
        if bp : startup = bp + startup
        else  : startup = 'Startup.py'
    
    if os.path.exists( startup ) :
        execfile( startup )
    else :
        import Bender.Startup
        
except:
    pass

from GaudiPython.Bindings import gbl as cpp 
cpp.Gaudi.createApplicationMgr

## ============================================================================
## post action 
__Bender_Post_Action = None 
## pre action 
__Bender_Pre_Action  = None
## ============================================================================
## post-action 
def post_Action () :
    """
    Get the ``post-action''
    """
    return __Bender_Post_Action
## ============================================================================
## post-action 
def pre_Action () :
    """
    Get the ``pre-action''
    """
    return __Bender_Pre_Action
## ============================================================================
## set new Post-action 
def setPostAction ( action ) :
    """
    set new ``post-action'' for Bender
    """
    global __Bender_Post_Action
    __Bender_Post_Action = action
    
## ============================================================================
## set new Pre-action 
def setPreAction ( action ) :
    """
    set new ``pre-action'' for Bender
    """
    global __Bender_Pre_Action
    __Bender_Pre_Action = action
        
# =============================================================================
## run events 
def run ( nEvents     =   -1 ,
          postAction  = None ,
          preAction   = None ) :
    """
    Run gaudi

    >>> run(50)
    
    """
    ## apply "pre-action", if defined 
    if   preAction    : preAction ()
    elif pre_Action() :
        action = pre_Action()
        action() 
        
    ## get the application manager
    _g = appMgr() 
    st = _g.run ( nEvents )
    
    ## apply "post-action", if defined 
    if   postAction    : postAction ()
    elif post_Action() :
        action = post_Action()
        action() 
        
    return st 


# ==============================================================================
## define the input data for Bender job
def setData ( files , catalogs = [] ) :
    """
    Define the input data for Bender job:
    
    >>> files    = [ 'file1.dst' , 'file2.dst' ]
    
    >>> catalogs = ....
    
    >>> import USERSCRIPT
    
    >>> USERSCRIPT.setData ( files , catalogs )
    
    """
    from GaudiPython.Bindings import _gaudi
    
    if   type ( files    ) is str   : files    =      [ files    ]
    elif type ( files    ) is tuple : files    = list ( files    ) 
    if   type ( catalogs ) is str   : catalogs =      [ catalogs ]    
    elif type ( catalogs ) is tuple : catalogs = list ( catalogs )
    
    if not issubclass ( type ( files    ) , list ) :
        files    = [ f for f in files    ] 
    if not issubclass ( type ( catalogs ) , list ) :
        catalogs = [ c for c in catalogs ] 
    
    if not _gaudi :               ## here we deal with configurables!
        
        if files :
            
            from Bender.DataUtils import extendfile
            files = [ extendfile ( f ) for f in files ]
            
            from Gaudi.Configuration import EventSelector
            EventSelector ( Input = files )
            
        if catalogs :
            
            from Gaudi.Configuration import Gaudi__MultiFileCatalog as FileCatalog
            FileCatalog   ( Catalogs = catalogs )
            from Gaudi.Configuration import FileCatalog
            FileCatalog   ( Catalogs = catalogs )
            
    else :                        ## here we deal with the actual components
        
        if files : 
            _e = _gaudi.evtSel()
            _e.open ( files )
        if catalogs :
            _f = _gaudi.service ( 'FileCatalog' )
            _f.Catalogs = catalogs


## ditto...
setInputData = setData

## ============================================================================
## Get the application manager 
def appMgr( *varg , **kwarg ) :
    """
    Get the application manager 
    """
    from GaudiPython.Bindings import AppMgr 
    _g = AppMgr()
    if not 'LoKiSvc' in _g.ExtSvc : _g.ExtSvc += [ 'LoKiSvc']
    return _g

# =============================================================================
## define "easy"-function to browse TES
def ls  ( *args ) :
    """
    Browse the Transient Store 
    """
    _g  = appMgr()
    _es = _g.evtSvc()
    return _es.ls ( *args ) 


## massive imports of everything 
from LoKiCore.decorators          import *
from LoKiPhys.decorators          import *
from LoKiArrayFunctors.decorators import *
from LoKiTrigger.decorators       import *
from LoKiAlgo.decorators          import *
from LoKiCore.functions           import *

# =============================================================================
from Gaudi.Configuration import importOptions

# =============================================================================
## apply some last-minutes fixes
import Bender.Fixes

## @var LoKi   : define namespace LoKi 
LoKi   = cpp.LoKi
## @var Bender : define namespace Bender 
Bender = cpp.Bender
## @var LHCb   : define namespace LHCb
LHCb   = cpp.LHCb
## @var Gaudi  : define namespace Gaudi
Gaudi  = cpp.Gaudi


_SC=cpp.StatusCode
SUCCESS = _SC(_SC.SUCCESS,True)
FAILURE = _SC(_SC.FAILURE,True)

_SE = cpp.StatEntity
_iadd_old_ = _SE.__iadd__
def _iadd_new_ (s,v) :
    return _iadd_old_(s,float(v))
_SE.__iadd__ = _iadd_new_
_SE.__str__  = _SE.toString 


# =============================================================================
## Welcome message:
Bender.Welcome.instance()

# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120
    

# =============================================================================
# The END 
# =============================================================================
