#!/usr/bin/env python
# =============================================================================
## @file no_mc_decays.py
#
#  Simple script to address ``MCDecayTuple does not contain all events'' problem
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
# Usage:
#
# @code
#
#    no-mc-decays [options] -d GENERICDECAY
#                           -z DECAYINQUESTION file1 [ file2 [ file3 [ file4 ....'
#
#  @endcode 
#
#  @date   2011-10-24
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#                    $Revision: 152803 $
#  Last modification $Date: 2013-02-23 14:29:42 +0100 (Sat, 23 Feb 2013) $
#                 by $Author: ibelyaev $
# =============================================================================
"""

Simple script to address ``MCDecayTuple does not contain all events'' problem

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

    no-mc-decays [options] -d GENERICDECAY
                           -z DECAYINQUESTION file1 [ file2 [ file3 [ file4 ....'

                  $Revision: 152803 $
Last modification $Date: 2013-02-23 14:29:42 +0100 (Sat, 23 Feb 2013) $
               by $Author: ibelyaev $

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2013-09-25 "
__version__ = " $Revision: 162137 $ "
__usage__   = """
   no-mc-decays [options] -d GENERICDECAY
                          -z DECAYINQUESTION file1 [ file2 [ ....
   """
# =============================================================================

if '__main__' == __name__ :
    
    print 120*'*'
    print                __doc__ 
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__
    print 120*'*'
    
    from BenderTools.NoMCdecay import noDecays 
    noDecays  ( __usage__ , __version__ )
    
# =============================================================================
# The END 
# =============================================================================
