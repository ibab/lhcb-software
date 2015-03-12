#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$
# =============================================================================
## @file
#  Set of Hlt2-lines suitable for the study of associative production of
#  various objects: e.g. J/psi&J/psi, J/psi&D, D&D, ...
#
#  The primary goal is to ``protect'' these events from prescaling 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-03-10
#
#  
#                    $Revision:$
#  Last modification $Date:$
#  by                $Author:$
# =============================================================================
""" Set of Hlt2-lines suitable for the study of associative production of
various objects: e.g. J/psi&J/psi, J/psi&D, D&D, ...

The primary goal is to ``protect'' these events from prescaling 
"""
# =============================================================================
__version__ = "$Revision: $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2015-03-10"
# =============================================================================
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
# =============================================================================
## @class DPSLines
#  Set of Hlt2-lines suitable for the study of associative production of
#  various objects: e.g. J/psi&J/psi, J/psi&D, D&D, ...
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-03-10    
class DPSLines(Hlt2LinesConfigurableUser) :
    """
    Set of Hlt2-lines suitable for the study of associative production of
    various objects: e.g. J/psi&J/psi, J/psi&D, D&D, ...
    
    The primary goal is to ``protect'' these events from prescaling 
    """
    
    __slots__ = { 'Prescale'   : {} ,
                  'Postscale'  : {} ,
                  'Common'     : {} }
    
    def __apply_configuration__( self ):
        
        from Hlt2Lines.CharmHad.Lines import CharmHadLines as Charm
        from Hlt2Lines.DiMuon.Lines import DiMuonLines as DiMuon

        from Hlt2Lines.DPS.Stages import MergeCharm, DiCharm
        
        charm = Charm  ()
        dimu  = DiMuon ()
        
        ##charm_stages = charm.stages()
        jpsi  = dimu .stages ( 'JPsi'  )[0]
        psi2s = dimu .stages ( 'Psi2S' )[0]
        B     = dimu .stages ( 'B'     )[0]
        Z0    = dimu .stages ( 'Z'     )[0]

        ## all  interesting dimuons
        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
        mu2   = MergeCharm( '2mu', [ Hlt2ExternalStage( dimu, s ) for s in ( jpsi, psi2s, B, Z0 ) ] )
        
        stages = {
            '2x2mu' : [ DiCharm ('2x2Mu' , 'chi_b1(1P) -> J/psi(1S) J/psi(1S)' , [ mu2 ] ) ]
            }


        for ( linename , algos ) in self.algorithms(stages).iteritems():
            from HltLine.HltLine import Hlt2Line
            Hlt2Line( 'DPS' + linename          ,
                      prescale  = self.prescale  , 
                      algos     = algos          ,
                      postscale = self.postscale )

# =============================================================================
if '__main__' == __name__ :

    from Gaudi.Configuration import log
    
    print  __doc__
    
    
# =============================================================================
# The END 
# =============================================================================
