#!/usr/bin/env python
# =============================================================================
# $Id: Utils.py,v 1.5 2009-07-21 08:59:46 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/Utils.py
#  helper file for configuration of "standard particles"
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Helper file for configuration of 'standard particles'
"""
author  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
version = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    # general:
    'updateDoD'     , ## update data-on-demand service  
    'locationsDoD'  , ## print locations, known for data-on-demand service 
    'particles'     , ## locations, known for data-on-demand service
    'trackSelector' , ## get the track selector
    'protoFilter'   , ## filter for (charged) protoparticles
    'defaultCuts'   , ## Default PT and IP cuts
    )
# =============================================================================

from Gaudi.Configuration      import *
from Configurables            import DataOnDemandSvc
from Configurables            import TrackSelector
from Configurables            import LHCbConfigurableUser

# local storage of data-on-demand actions 
_particles = {}

# Default cuts
_defaultCuts = "(PT>250*MeV) & (MIPCHI2DV(PRIMARY) > 4.)"

# =============================================================================
#  Default Cuts
def defaultCuts():
    """
    Return string with default cuts
    """
    return _defaultCuts

# =============================================================================
# =============================================================================
## @class DefaultTrackingCuts
#  Little class to hold default track selection criteria
class DefaultTrackingCuts(LHCbConfigurableUser) :
    ## Configuration slots
    __slots__ = { "Cuts"  :  { "Chi2Cut" : [ 0, 5 ],
                               "CloneDistCut" : [5000, 9e+99 ] }, ## The cuts to apply
                  "Types" :  ["Long"]  ## The track types to include
                  }

# =============================================================================
## Return the locations known for data-on-demand service 
def particles () :
    """
    Return the locations known for data-oon-demand service 
    """
    return _particles


# =============================================================================
# Update Data-On-Demand Service 
def updateDoD ( alg , hat = 'Phys/' ) :
    """
    Update Data-On-Demand service
    """
    _parts = { hat+alg.name()+'/Particles' : alg } 
    _particles.update ( _parts ) 
    
    dod = DataOnDemandSvc()
    dod.AlgMap.update(
        { hat + alg.name() + '/Particles' : alg.getFullName() }
        )
    return _parts 
    
## get the track selector 
def trackSelector ( alg ,
                    tracks     = TrackSelector,
                    trackTypes = [],
                    cuts       = {}
                    ) :
    """
    Get the track selector for maker 
    """
    alg.addTool ( tracks )

    selector = getattr ( alg , tracks.getType() )

    # selection cuts
    # Ugly but works ...
    if 0 == len(trackTypes) :
        trackTypes = DefaultTrackingCuts().Types
    if 0 == len(cuts) :
        cuts       = DefaultTrackingCuts().Cuts
    selector.TrackTypes = trackTypes
    for name,cut in cuts.iteritems() :
        selector.setProp("Min"+name,cut[0])
        selector.setProp("Max"+name,cut[1])
        if ( "CloneDistCut"==name): selector.setProp("AcceptClones",False)
#    selector.MinPtCut = 100000000
        
    return selector

## get the protoparticle filter
def protoFilter ( alg , fltr , name ) :
    """
    Get the protoparticle filter 
    """
    alg.addTool ( fltr , name )
    return getattr ( alg , name )

    
# =============================================================================
## nice prinout for the input locations, known for Data-On-Demand Service 
def locationsDoD ( particles = _particles ) :
    """
    Nice printout of the known algorithm for 'standard particles'
    """
    line = ' +' + 100*'-' + '+ '
    result  = line 
    result += '\n | %-45.45s : %-50.50s |' % ( 'Input Location for DVAlgorithm' ,
                                               'Data-On-Demand creator' )        
    result += '\n' + line
    locs = particles.keys()
    locs.sort() 
    for l in locs :
        a = particles[l]
        if a.name() == a.getType() : a = a.name()
        else : a = a.getFullName()
        result += '\n | %-45.45s : %-50.50s |' % ( l , a )        
    result += '\n' + line 
    return result 

# =============================================================================
# The END 
# =============================================================================
