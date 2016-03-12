#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
## @file PhysSelPython/Selections.py
#  Collection of high-level blocks for ``Seelctions''-framework
#
# Basic blocks: 
# - AutomaticData            :  selection that gets data from TES (or Datra-On-Demand)
# - MergedSelection          :  merge several selections into one ('OR'-mode)
# - SelectionSequence        :  create the selection sequence 
# - MultiSelectionSequence   :  create the sequence for several selections 
#
# Derived blocks: 
# - SimpleSelection          : create 1-step selection 
# - FilterSelection          : selection with FilterDesktop    algorithm 
# - CombineSelection         : selection with CombineParticles algorithm 
# - Combine3BodySelection    : selection with DaVinci::N3BodyDecays algorithm 
# - Combine4BodySelection    : selection with DaVinci::N4BodyDecays algorithm 
# - TupleSelection           : selection with DecayTreeTuple algorithm 
# - PrintSelection           : helper selection for debugging purposes
# - LimitSelection           : limit selection 
# - CheckPVSelection         : embed CheckPV algorithm into selectiton sequence 
# - MomentumScaling          : insert momentum scaling into selectiton sequence
#
#  @author Juan  PALACIOS 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#
#                    $Revision:$
#  Last modification $Date:$
#                 by $Author:$ 
# =============================================================================
""" Collection of high-level blocks for ``Selections''-framework:

Basic blocks: 
- AutomaticData            :  selection that gets data from TES (or Datra-On-Demand)
- MergedSelection          :  merge several selections into one ('OR'-mode)
- SelectionSequence        :  create the selection sequence 
- MultiSelectionSequence   :  create the sequence for several selections 

Derived blocks: 
- SimpleSelection          : create 1-step selection 
- FilterSelection          : selection with FilterDesktop    algorithm 
- CombineSelection         : selection with CombineParticles algorithm 
- Combine3BodySelection    : selection with DaVinci::N3BodyDecays algorithm 
- Combine4BodySelection    : selection with DaVinci::N4BodyDecays algorithm 
- TupleSelection           : selection with DecayTreeTuple algorithm 
- PrintSelection           : helper selection for debugging purposes
- LimitSelection           : limit selection 
- CheckPVSelection         : embed CheckPV algorithm into selectiton sequence 
- MomentumScaling          : insert momentum scaling into selectiton sequence 
"""
# =============================================================================
__author__  = 'Juan PALACIOS, Vanya BELYAEV'
__version__ = '$Revision:$'
__date__    = '2016-03-12'
__all__     = ( 'AutomaticData'                 ,
                'Selection'                     ,
                'MergedSelection'               ,
                'SelectionSequence'             ,
                'MultiSelectionSequence'        ,
                #
                'SimpleSelection'               , 
                'FilterSelection'               , 
                'PrintSelection'                , 
                'LimitSelection'                ,
                'CombineSelection'              ,
                'Combine3BodySelection'         ,
                'Combine4BodySelection'         ,
                'TupleSelection'                ,
                ##
                'CheckPVSelection'              ,  
                ## 
                'MomentumScaling' 
                )
# =============================================================================
## import all blocks from wrappers 
from PhysSelPython.Wrappers import *
# =============================================================================


# =============================================================================
if '__main__' == __name__ :

    print   80*'*'  
    print  __doc__ 
    print  ' Author  : %s ' %  __author__  
    print  ' Version : %s ' %  __version__  
    print  ' Date    : %s ' %  __date__   
    print  ' Symbols : %s ' %  list ( __all__ )
    print   80*'*'  
 
# =============================================================================
# The END 
# =============================================================================

