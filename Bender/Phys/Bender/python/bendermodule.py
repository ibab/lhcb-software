# =============================================================================
# $Id: bendermodule.py,v 1.16 2005-01-21 09:33:06 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.15  2004/11/23 17:13:23  ibelyaev
#  v4r4
#
# Revision 1.14  2004/11/08 17:06:52  ibelyaev
#  v4r2
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
for lib in _libs_ :
    print '1) loading '+`lib`
    gaudimodule.loaddict( lib + 'EventDict' )
    print 'after loading '+`lib`

_libs_ = ( 'BenderDict'     ,
           'LoKiHybridDict' )
for lib in _libs_ :    
    print '2) loading '+`lib`
    gaudimodule.loaddict( lib               )
    print 'after loading '+`lib`


from   bendertypes   import *
from   benderaux     import *
from   bendercuts    import *
from   benderloop    import *
from   bendertuple   import *
from   bendermatch   import *
from   benderalgo    import *


AppMgr = gaudimodule.AppMgr 

gaudi  = AppMgr()

appMgr = gaudi     #
g      = gaudi     # for 'backward compatibility'
theApp = gaudi     # ATLAS style


_libs_ = ( 'LoKi'       ,
           'LoKiHlt'    ,
           'LoKiHybrid' )
for lib in _libs_ :
    if lib not in gaudi.DLLs : gaudi.DLLs += [ lib ]

import benderstr
import benderconfig


# =============================================================================
# The END 
# =============================================================================
