# =============================================================================
# $Id: bendermodule.py,v 1.27 2005-07-06 18:28:02 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.26  2005/06/16 17:45:31  ibelyaev
#  add the usage of 'atexit(gaudi.exit())'
#
# Revision 1.25  2005/05/20 10:55:19  ibelyaev
#  prepare for v4r8
#
# Revision 1.23  2005/03/04 19:30:43  ibelyaev
#  v4r7: add possibility to deal with AlgTools
#
# Revision 1.22  2005/02/10 18:37:01  ibelyaev
#  add new moduel bendermatrix
#
# Revision 1.21  2005/02/08 11:29:30  ibelyaev
#  add startup file
#
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
from   bendertools     import *
from   benderclhep     import *

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
import bendermath
import bendermatrix
import benderconfig
import benderrels 

import atexit
def _atexit_() :
    state = gaudi.state()
    if 1 == state or 3 == state : gaudi.exit()

atexit.register( _atexit_ )

# =============================================================================
# The END 
# =============================================================================
