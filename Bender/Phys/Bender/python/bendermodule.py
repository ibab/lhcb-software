# =============================================================================
# $Id: bendermodule.py,v 1.20 2005-02-02 19:15:10 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.19  2005/01/24 17:44:39  ibelyaev
#  v4r5
#
# =============================================================================
"""
This is a major Python Module for Bender application
Author:      Vanya BELYAEV Ivan.Belyaev@itep.ru
"""

# =============================================================================
# @file
#
# The major Python module for Bender application 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

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
