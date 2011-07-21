#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
# 
#  The attempt for coherent stripping of various ``charm''? associative production
#
#    - dimuon + dimuon 
#    - dimuon + high-pt gamma 
#    - dimuon + charm 
#    - charm  + anti-charm
#    - charm  + charm 
#
#  The performance with Reco09-Stripping13_SDSTs.py
#  +-----------------------------------------+----------------+
#  |    Stripping Line                       |  Rate, [%]     |
#  +-----------------------------------------+----------------+
#  | StrippingDiCharmForPromptCharm          | 0.0174+-0.0087 |       
#  | StrippingDiMuonAndCharmForPromptCharm   | 0.0305+-0.0115 | 
#  | StrippingDoubleDiMuonForPromptCharm     | 0.0305+-0.0115 |        
#  | StrippingDiMuonAndGammaForPromptCharm   | 0.1785+-0.0278 | 
#  +-----------------------------------------+----------------+
#  | Total                                   | 0.2568+-0.0334 | 
#  +-----------------------------------------+----------------+
#  | Unique with respect to FullDSTDiMuon    | 0.1350+-0.0242 |
#  +-----------------------------------------+----------------+
#
#  Note that only (0.1350+-0.0242)% of rate are unique 
#  with respect to FullDSTDiMion lines,
#  that corresponds to 2.6% (relative) increment of FullDSTDiMuon lines 
#
# @author Vanya BELYAEV Ivan.Belyaev@cern.ch
# @date   2011-05-26
# 
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
The attempt for coherent stripping of various ``charm''? associative production

    - dimuon + dimuon 
    - dimuon + high-pt gamma 
    - dimuon + charm 
    - charm  + anti-charm
    - charm  + charm 

  The performance with Reco09-Stripping13_SDSTs.py
  +-----------------------------------------+----------------+
  |    Stripping Line                       |  Rate, [%]     |
  +-----------------------------------------+----------------+
  | StrippingDiCharmForPromptCharm          | 0.0174+-0.0087 |       
  | StrippingDiMuonAndCharmForPromptCharm   | 0.0305+-0.0115 | 
  | StrippingDoubleDiMuonForPromptCharm     | 0.0305+-0.0115 |        
  | StrippingDiMuonAndGammaForPromptCharm   | 0.3004+-0.0361 |
  +-----------------------------------------+----------------+
  | Total                                   | 0.3787+-0.0405 |
  +-----------------------------------------+----------------+
  | Unique with respect to FullDSTDiMuon    | 0.1350+-0.0242 |
  +-----------------------------------------+----------------+

  Note that only (0.1350+-0.0242)% of rate are unique
with respect to FullDSTDiMion lines,
  that corresponds to 2.6% (relative) increment of FullDSTDiMuon lines 

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@cern.nl'
__date__    = '2011-05-26'
__version__ = '$Revision$'
# =============================================================================
__all__ = (
    'StrippingCharmAssociativeConf',
    'default_config'
    )
# =============================================================================
from Gaudi.Configuration import *

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import   Selection, MergedSelection 
from StandardParticles      import ( StdNoPIDsPions  ,
                                     StdLoosePions   ,
                                     StdLooseKaons   ,
                                     StdLooseProtons , 
                                     StdLooseMuons   ) 

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

import logging
log = logging.getLogger( 'StrippingCharmAssociative' )

from StrippingArchive.Stripping15.StrippingPromptCharm import ( _default_configuration_ ,
                                                       default_config          ) 


# =============================================================================
## @class  StrippingCharmAssociativeConf
#  Helper class required by Tom & Greig 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @date 2011-05-26
class StrippingCharmAssociativeConf(LineBuilder) :
    """
    Helper class to configure 'CharmAssociative/PromptCharm'-lines
    """
    __configuration_keys__ = tuple ( _default_configuration_.keys() ) 
    
    ## get the default configuration 
    @staticmethod
    def defaultConfiguration( key = None ) :
        """
        Get the default configurtaion
        
        >>> conf = StrippingCharmAssociativeConf.defaultConfiguration()
        
        Get the elements of default configurtaion:
        
        >>> prescale = StrippingCharmAssociativeConf.defaultConfiguration( 'DoubleDiMuon' )
        """
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        if key : return _config[ key ]
        return _config
    
    ## constructor
    def __init__( self , name , config ) :
        """
        Constructor
        """
        # check the names 
        if not name : name = 'CharmAssociative'
        # check the names 
        if 'CharmAssociative' != name :
            log.warning ( 'The non-default name is specified' ) 
            
        from copy import deepcopy
        _config = deepcopy ( _default_configuration_ )
        _config.update ( config ) 
        
        LineBuilder.__init__( self , name , _config )
        
        from StrippingSelections.StrippingPromptCharm import StrippingPromptCharmConf as PromptCharm
        
        _prompt = PromptCharm ( 'PromptCharm' , config = config )
        for line in _prompt._lines_dicharm() :
            self.registerLine(line)
            


    
# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author :  %s' % __author__
    print ' Date   :  %s' % __date__
    print ' The output locations for default configuration: '
    _conf = StrippingCharmAssociativeConf ( 'CharmAssociative' ,
                                            config = default_config  ) 
    for l in _conf.lines() :
        print ' \t ', l.outputLocation  () , l 
        print 80*'*'
        
# =============================================================================
# The END 
# =============================================================================

