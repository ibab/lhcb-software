#!/usr/bin/env python 
# =============================================================================
# $Id: Bs2DsPi_data.py,v 1.1 2008-10-04 16:14:47 ibelyaev Exp $
# =============================================================================
## @file
# 
#  Helper python module to define the input data for LoKi example
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-10-04
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaebv@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $,  version $Revision: 1.1 $ "
# =============================================================================
__all__ = (
    'Files'  , ## useful for iEventSelector.open input (GaudiPython) 
    'Inputs' , ## useful for EventSelector ( Input = ... ) (Configurables)
    'LFNs'     ## the list of proper LFNs (for Ganga&Dirac?)
    ) 
# =============================================================================

Files_MCSELECTED = (
    'Bs2DsPi_1.dst' ,
    'Bs2DsPi_2.dst' ,
    'Bs2DsPi_3.dst' ,
    'Bs2DsPi_4.dst'   
    )
LFNs = []


from Bs2Jpsiphi_mm_data import checkFiles, getInputs


Files = checkFiles ( Files_MCSELECTED ) 

if not Files : Files = LFNs     
Inputs = getInputs ( Files )   

# =============================================================================
if '__main__' == __name__ :  ## just for pure testing purposes 
    print 'Files: '
    for f in Files  : print '\t' , f 
    print 'Inputs: '
    for i in Inputs : print '\t' , i 

# =============================================================================
# The END 
# =============================================================================



