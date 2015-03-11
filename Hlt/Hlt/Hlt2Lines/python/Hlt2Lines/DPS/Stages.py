#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Set of ``stages'' for Hlt2-lines devoted to the study of
#  the associative production of various objects: e.g. J/psi&J/psi, J/psi&D, D&D, ...
#
#  The primary goal is to ``protect'' these events from prescaling 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-03-10
#  
#                    $Revision:$
#  Last modification $Date:$
#  by                $Author:$
# =============================================================================
""" Set of ``stages'' for Hlt2-lines devoted to the study of
the associative production of various objects: e.g. J/psi&J/psi, J/psi&D, D&D, ...

The primary goal is to ``protect'' these events from prescaling 
"""
# =============================================================================
__version__ = "$Revision: $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-03-10"
__all__     = ( 'MergeCharm'  ,
                'DiCharm'     )
# =============================================================================
from Hlt2Lines.Utilities.Hlt2Combiner    import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter      import Hlt2ParticleFilter
# =============================================================================
## @class MergedCharm
#  make ``merged'' container of charm 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-03-10
class MergeCharm(Hlt2ParticleFilter) :
    """
    Make ``merged'' container of charm
    """
    def __init__ ( self , name , inputs ) :
        super(MergeCharm, self).__init__( name, "ALL ", inputs )

# =============================================================================
## @class DiCharm
#  Make various combinations of charm hadrons 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-03-10
class DiCharm(Hlt2Combiner) :
    """
    Make various combinations of charm hadrons 
    """
    def __init__ ( self , name , decays , inputs ) :
        """ Instantiaate the combiner """ 
        super(DiCharm, self).__init__( name , decays , inputs ,
                                       CombinationCut = 'AALL' , 
                                       MotherCut      = 'ALL'  )

# ==============================================================================
if '__main__' == __name__ :

    from Gaudi.Configuration import log    
    log.info ( 'I am here!!!' )

    print __doc__ 
    print __all__ 
# =============================================================================
# The END 
# =============================================================================
