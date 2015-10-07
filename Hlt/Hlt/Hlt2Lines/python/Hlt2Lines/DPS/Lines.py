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
    
    __slots__ = { '_stages'    : {} ,
                  'Prescale'   : {} ,
                  'Postscale'  : {} }
    
    def stages ( self , nickname = '' ) :
        if not (hasattr(self, '_stages') and self._stages):

            from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
            from Hlt2Lines.DPS.Stages import MergeCharm 
            
            from Hlt2Lines.DiMuon.Lines import DiMuonLines as DiMuon
            dimu  = DiMuon ()
            
            jpsi  = dimu .stages ( 'JPsi'  )[0]
            psi2s = dimu .stages ( 'Psi2S' )[0]
            B     = dimu .stages ( 'B'     )[0]
            Z0    = dimu .stages ( 'Z'     )[0]
            
            ## all  interesting dimuons
            mu2   = MergeCharm ( '2mu', [ Hlt2ExternalStage ( dimu, s ) for s in ( jpsi , psi2s , B , Z0 ) ] )            
            
            from Hlt2Lines.CharmHad.Lines import CharmHadLines as Charm
            charm = Charm  ()
            
            D0  = charm.stages ( 'D02KmPipTurbo'       ) [0] ## Wide mass here!!!
            D   = charm.stages ( 'DpToKmPipPipTurbo'   ) [0]
            Ds  = charm.stages ( 'DspToKmKpPipTurbo'   ) [0]
            Lc  = charm.stages ( 'LcpToPpKmPipTurbo'   ) [0]
            
            Hc  = MergeCharm ( 'MergedHc', [ Hlt2ExternalStage ( charm, s ) for s in ( D0 , D , Ds , Lc ) ] )
            
            from Hlt2Lines.DPS.Stages import DoubleDiMuon, DiMuonAndCharm, DoubleCharm 
            self._stages  = {
                '2x2mu'  : [ DoubleDiMuon   ('2x2mu'  , [ mu2      ] ) ] , 
                '2xHc'   : [ DoubleCharm    ('2xHc'   , [       Hc ] ) ] , 
                '2mu&Hc' : [ DiMuonAndCharm ('2mu&Hc' , [ mu2 , Hc ] ) ]
                }
            
        return self._stages[nickname] if nickname else self._stages
            
    def __apply_configuration__( self ):
        
        
        _stages = self.stages() 
        for ( linename , algos ) in self.algorithms( _stages ).iteritems():
            from HltLine.HltLine import Hlt2Line
            Hlt2Line( 'DPS:' + linename          ,
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
