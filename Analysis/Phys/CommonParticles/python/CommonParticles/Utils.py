#!/usr/bin/env python
# =============================================================================
# $Id: Utils.py,v 1.3 2009-07-01 18:42:29 jonrob Exp $ 
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
version = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    # general:
    'updateDoD'     , ## update data-on-demand service  
    'locationsDoD'  , ## print locations, known for data-on-demand service 
    'particles'     , ## locations, known for data-on-demand service
    'trackSelector' , ## get the track selector
    'protoFilter'     ## filter for (charged) protoparticles 
    )
# =============================================================================

from Gaudi.Configuration import *
from Configurables       import DataOnDemandSvc
from Configurables       import TrackSelector

# local storage of data-on-demand actions 
_particles = {} 

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
    _parts = { hat+alg.name() : alg } 
    _particles.update ( _parts ) 
    
    dod = DataOnDemandSvc()
    dod.AlgMap.update(
        { hat + alg.name() + '/Particles' : alg.getFullName() , 
          hat + alg.name() + '/Vertices'  : alg.getFullName() }
        )
    return _parts 


## get the track selector 
def trackSelector ( alg ,
                    tracks     = TrackSelector,
                    trackTypes = ["Long"],
                    cuts       = {  "Chi2Cut"       : [0,10]
                                   ,"LikelihoodCut" : [-100,9e40]
                                   ,"CloneDistCut"  : [-1e10,9e40]
                                   ,"GhostProbCut"  : [-1,0.99]
                                   }
                    ) :
    """
    Get the track selector for maker 
    """
    alg.addTool ( tracks )

    selector = getattr ( alg , tracks.getType() )

    # selection cuts
    selector.TrackTypes = trackTypes
    
    for name,cut in cuts.iteritems() :
        selector.setProp("Min"+name,cut[0])
        selector.setProp("Max"+name,cut[1])

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
