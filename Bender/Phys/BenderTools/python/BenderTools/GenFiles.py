#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderTools/GenFiles.py
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
#    dst_dump [options] file1 [ file2 [ file3 [ file4 ....'
#
#  @endcode 
#
#  @date   2011-10-24
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================

"""

This file is a part of BENDER project:

  ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
 
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''

Usage:

    dst_dump [options] file1 [ file2 [ file3 [ file4 ....'


                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@cern.ch'
__date__    = "2011-10-24"
__version__ = '$Revision$'
__all__     = ()  ## nothing to import 
__usage__   = 'dst_dump [options] file1 [ file2 [ file3 [ file4 ....'
__all__     =  ( 'dumpDst' , )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.GenFiles' )
else                      : logger = getLogger ( __name__ )
#
# =============================================================================
## define proper action for processing of (x)gen-files
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date   2013-02-19
def genAction ( extension = 'gen' ) :
    """
    Define prooer action for processing of (x)gen files 
    """
    ## 
    ext = extension.upper()
    ##
    if not ext in ( 'GEN' , 'XGEN' ) : 
        logger.warning('No action is needed for extension =%s' % ext)
        return

    ## specific post-config action for (x)GEN-files 
    def _gen_postconfig_ () :
        """
        specific post-config action for (x)GEN-files 
        """
        logger.info ( 'Start post-config action for (x)gen-files' )
        from Configurables import DataOnDemandSvc
        dod  = DataOnDemandSvc ()
        from copy import deepcopy 
        algs = deepcopy ( dod.AlgMap ) 
        bad  = set() 
        for key in algs :
            if     0 <= key.find ( 'Rec'     )                  : bad.add ( key )
            elif   0 <= key.find ( 'Raw'     )                  : bad.add ( key )
            elif   0 <= key.find ( 'DAQ'     )                  : bad.add ( key )
            elif   0 <= key.find ( 'Trigger' )                  : bad.add ( key )
            elif   0 <= key.find ( 'Phys'    )                  : bad.add ( key )
            elif   0 <= key.find ( 'Prev/'   )                  : bad.add ( key )
            elif   0 <= key.find ( 'Next/'   )                  : bad.add ( key )
            elif   0 <= key.find ( '/MC/'    ) and 'GEN' == ext : bad.add ( key )
                
        for b in bad :
            logger.debug ( 'Remove key from DataOnDemand actions %s' % key )
            del algs[b]
            
        logger.info ( 'Remove %d keys from DataOnDemand actions ' % len(bad) )        
        dod.AlgMap = algs
            
        from Configurables import EventClockSvc, CondDB 
        EventClockSvc ( EventTimeDecoder = "FakeEventTime" )
        logger.info( 'Use fake event time decoder for (x)gen-files' )
        CondDB  ( IgnoreHeartBeat = True )
        logger.info( 'Ignore Heart-beat for (x)gen-files' )
        
    ## Important: use Post Config action! 
    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction ( _gen_postconfig_ )        
    logger.info ( 'Define post-config action for (x)gen-files' )


# =============================================================================
if '__main__' == __name__ :

    print 120*'*'
    print ' Author  : ', __author__ 
    print ' Version : ', __version__ 
    print ' Date    : ', __date__ 
    
    print 120*'*'
    
# =============================================================================
# The END 
# =============================================================================

   
