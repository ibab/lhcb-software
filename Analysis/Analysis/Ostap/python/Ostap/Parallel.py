#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file Parallel.py 
#
#  Useful utilities for multiprocessing and parallel processing for Ostap
#  Actualy it is just a little bit upgraded version of original
#  GaudiMP.Parallel module developed by Pere MATO Pere.Mato@cern.ch
#
#  The original module relies on some obsolete python modules (e.g. pyssh)
#  and it has limitations related ti the pickling.
#
#  The upgraded module relies on <code>pathos</code> suite that
#  has very attratcive functionality and solve pickling issues
#  @see https://github.com/uqfoundation/pathos
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-02-23
#
#                    $Revision:$
#  Last modification $Date:$
#  by                $Author:$
# =============================================================================
"""   Useful utilities for multiprocessing and parallel processing for Ostap
Actualy it is just a little bit upgraded version of original
GaudiMP.Parallel module developed by Pere MATO Pere.Mato@cern.ch

The original module relies on some obsolete python modules (e.g. pyssh)
and it has limitations related ti the pickling.


The upgraded module relies on <code>pathos</code> suite that
has very attratcive functionality and solve pickling issues
@see https://github.com/uqfoundation/pathos
"""
# =============================================================================
__version__ = '$Revision:$'
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = '2016-02-23'
__all__     = (
    'Task'        , ## the base class for task
    'WorkManager' , ## task manager 
    )
# =============================================================================
from Ostap.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'Ostap.Paralllel')
else                      : logger = getLogger ( __name__         ) 
# =============================================================================
try:
    
    ## 
    from Ostap.ParallelPathos import Task, WorkManager 
    logger.info ('Use Task and TaskManager from Ostap.ParallelPathos')
    
except:
    
    ## 
    from GaudiMP.Parallel     import Task, WorkManager
    logger.info ('Use Task and TaskManager from GaudiMP.Parallel')

    
# =============================================================================
if '__main__' == __name__ :
    
    from Ostap.Line import line 
    logger.info ( __file__ + '\n' + line  )
    logger.info ( 80*'*' )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 


# =============================================================================
# The END 
# =============================================================================
