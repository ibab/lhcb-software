#!/usr/bin/env python
# =============================================================================
## @file KaliCalo/ReadDB.py
#  helper module to read local ``Kali'' data base 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-18
# =============================================================================
"""
helper module to read local ``Kali'' data base 
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-09-17 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
import ROOT
import shelve 
import CaloUtils.CellID, LHCbMath.Types, KaliCalo.Kali


dbase_name = 'Ecal_2k+10_Mar_18_db'

dbase = shelve.open( dbase_name , 'r')

for key in dbase : print key

print dbase_name


# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  


# =============================================================================
# The END 
# =============================================================================
