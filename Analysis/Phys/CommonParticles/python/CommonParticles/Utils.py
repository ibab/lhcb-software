#!/usr/bin/env python
# =============================================================================
# $Id: Utils.py,v 1.1 2009-01-15 14:22:15 ibelyaev Exp $ 
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
version = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    # general:
    'updateDoD'     , ## update data-on-demand service  
    'locationsDoD'  , ## print locations, known for data-on-demand service 
    'particles'     , ## locations, known for data-on-demand service
    # more specific:
    'particleMaker' , ## get the particle maker
    'trackSelector' , ## get the track selector
    'protoFilter'     ## filter for (charged) protoparticles 
    )
# =============================================================================

from Gaudi.Configuration import *
from Configurables       import DataOnDemandSvc
from Configurables       import PhysDesktop
from Configurables       import TrackSelector
from Configurables       import CombinedParticleMaker

# local storage of data-on-demand actions 
_particles = {} 

# =============================================================================
## Return the locations known for data-on-demand service 
def particles () :
    """
    Return the locations knonw for data-oon-demand service 
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

# get the particle maker 
def particleMaker ( alg , maker = CombinedParticleMaker ) :
    """
    Get the particle maker 
    """
    alg.addTool ( PhysDesktop )
    desktop = alg.PhysDesktop
    desktop.ParticleMakerType = maker.getType() 
    desktop.addTool ( maker )
    return getattr ( desktop , maker.getType() ) 

## get the track selector 
def trackSelector ( maker , tracks = TrackSelector ) :
    """
    Get the track selector for maker 
    """
    maker.addTool ( tracks )
    return getattr ( maker , tracks.getType() ) 

## get the protoparticle filter
def protoFilter ( maker , fltr , name ) :
    """
    Get the protoparticle filter 
    """
    maker.addTool ( fltr , name )
    return getattr ( maker , name )

    
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
