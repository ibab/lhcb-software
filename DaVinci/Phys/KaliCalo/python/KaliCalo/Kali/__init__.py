#!/usr/bin/env python
# ======================================================================
# @file KaliCalo/Kali/__init__.py
# Set of useful utilities & classes
#     for pi0 Ecal calibration
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @reshuffled by Albert Puig (Albert.Puig@cern.ch)
# @date 2010-03-17
# ======================================================================
"""
Set of useful utilities & classes
for ``iterative pi0'' Ecal calibration
"""
# ======================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-17 "
__version__ = " CVS tag $Name: not supported by cvs2svn $ , version $Revision$ "
# ======================================================================

__all__ = [ 'Counter' , 'VE' , 'HistosBase', 'HistoMapBase' , 'HistoMap' , 'LambdaMap' , 'pi0Mass' ]

from LoKiCore.basic import cpp as _cpp

Counter    = _cpp.StatEntity
VE         = _cpp.Gaudi.Math.ValueWithError

def _ve_str_ ( self , fmt = '( %.3g +- %.3g )' ) :
    return fmt % ( self.value() , self.error() )

def _ve_abs_ ( self ) :
    return VE ( abs( self.value() ) , self.cov2() )

VE.__str__  = _ve_str_
VE.__repr__ = _ve_str_
VE.__abs__  = _ve_abs_

# Pi0 mass
pi0Mass = 134.9766

# EOF
