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
__all__     = ( 'MergeCharm'     ,
                'DoubleDiMuon'   ,
                'DoubleCharm'    ,
                'DiMuonAndCharm' )
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
decays_2x2mu = [ 'chi_b0(1P) -> J/psi(1S) J/psi(1S)'          ] 
decays_2muHc = [ "[ chi_b1(1P) -> J/psi(1S) D0        ]cc"    ,
                 "[ chi_b1(1P) -> J/psi(1S) D+        ]cc"    ,
                 "[ chi_b1(1P) -> J/psi(1S) D_s+      ]cc"    ,
                 "[ chi_b1(1P) -> J/psi(1S) Lambda_c+ ]cc"    ]
decays_2xHc  = [ "   psi(3770) -> D0        D~0            "  ,
                 " [ psi(3770) -> D0        D-         ]cc "  ,
                 " [ psi(3770) -> D0        D_s-       ]cc "  ,
                 " [ psi(3770) -> D0        Lambda_c~- ]cc "  ,
                 #
                 "   psi(3770) -> D+        D-             "  ,
                 " [ psi(3770) -> D+        D_s-       ]cc "  ,
                 " [ psi(3770) -> D+        Lambda_c~- ]cc "  ,
                 #
                 "   psi(3770) -> D_s+      D_s-           "  ,
                 " [ psi(3770) -> D_s+      Lambda_c~- ]cc "  ,
                 #
                 "   psi(3770) -> Lambda_c+ Lambda_c~-     "  ,
                 # double charm
                 " [ psi(3770) -> D0        D0         ]cc"   ,
                 " [ psi(3770) -> D0        D+         ]cc "  ,
                 " [ psi(3770) -> D0        D_s+       ]cc "  ,
                 " [ psi(3770) -> D0        Lambda_c+  ]cc "  ,
                 #
                 " [ psi(3770) -> D+        D+         ]cc "  ,
                 " [ psi(3770) -> D+        D_s+       ]cc "  ,
                 " [ psi(3770) -> D+        Lambda_c+  ]cc "  ,
                 #
                 " [ psi(3770) -> D_s+      D_s+       ]cc "  ,
                 " [ psi(3770) -> D_s+      Lambda_c+  ]cc "  ,
                 #
                 " [ psi(3770) -> Lambda_c+ Lambda_c+  ]cc "  ]

# =============================================================================
class DoubleDiMuon  (DiCharm) :
    def __init__ ( self , name , inputs ) :
        DiCharm.__init__ ( self , name , decays_2x2mu , inputs )
        
# =============================================================================
class DiMuonAndCharm(DiCharm) :
    def __init__ ( self , name , inputs ) :
        DiCharm.__init__ ( self , name , decays_2muHc , inputs )

# =============================================================================
class DoubleCharm(DiCharm) :
    def __init__ ( self , name , inputs ) :
        DiCharm.__init__ ( self , name , decays_2xHc  , inputs )
        
# ==============================================================================
if '__main__' == __name__ :

    print 100*'*'
    print __doc__ 
    print 100*'*'
    print 'Symbols : %s ' % list( __all__ ) 
    print 'Author  : %s ' % __author__ 
    print 'Date    : %s ' % __date__ 
    print 'Verison : %s ' % __version__
    print 100*'*'
    
# =============================================================================
# The END 
# =============================================================================
