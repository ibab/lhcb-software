# =============================================================================
# $Id: bendermodule.py,v 1.21 2005-02-08 11:29:30 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
""" This is a major Python Module for Bender application """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================
# @file
#
# The major Python module for Bender application 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

try:
    import os 
    startup = os.environ.get('PYTHONSTARTUP',None)
    if startup and os.path.exist( startup ) : execfile( startup )
except:
    pass

import gaudimodule

_libs_ = ( ''     ,
           'Phys' ,
           'Tr'   ,
           'Rich' ,
           'Muon' ,
           'Calo' ,
           'Velo' ,
           'IT'   ,
           'OT'   ,
           'L0'   ,
           'Trg'  )
for lib in _libs_ : gaudimodule.loaddict( lib + 'EventDict' )

_libs_ = ( 'BenderDict'     ,
           'LoKiHybridDict' )
for lib in _libs_ : gaudimodule.loaddict( lib               )

from   bendertypes     import *
from   benderaux       import *
from   bendercuts      import *
from   benderloop      import *
from   bendertuple     import *
from   bendermatch     import *
from   benderalgo      import *

AppMgr = gaudimodule.AppMgr 

# create Gaudi application manager (if not done yet)  
gaudi  = AppMgr()

appMgr = gaudi     #
g      = gaudi     # for 'backward compatibility'
theApp = gaudi     # ATLAS style


getClass = gaudimodule.getClass

_libs_ = ( 'LoKi'       ,
           'LoKiHlt'    ,
           'LoKiHybrid' )
for lib in _libs_ :
    if lib not in gaudi.DLLs : gaudi.DLLs += [ lib ]

import benderstr
import bendersmartrefs
import benderconfig
import bendermath

# =============================================================================
# The END 
# =============================================================================
