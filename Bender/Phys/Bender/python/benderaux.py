#!/usr/bin/env python
# =============================================================================
# $Id: benderaux.py,v 1.15 2005-07-24 17:14:42 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.15 $
# =============================================================================
"""
Helper module  to define the auxilalry 'enums' and constants 
"""

# =============================================================================
# @file
# Helper file to define the auxillary 'enums' and constants 
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule

# load bender dictionaries (if not done yet)
gaudimodule.loaddict( 'BenderDict'    )
# global namespaces
gbl = gaudimodule.gbl

SUCCESS  = gbl.StatusCode( 1 )
FAILURE  = gbl.StatusCode( 0 )

# namespace LoKi
LoKi = gbl.LoKi
# namespace LoKi::Fits 
Fits = LoKi.Fits

FitStrategy   = gbl.LoKi.Fits.FitStrategy

FitNone       = FitStrategy (   )
FitVertex     = FitStrategy ( 0 )
FitMass       = FitStrategy ( 1 )
FitMassVertex = FitStrategy ( 2 )
FitDirection  = FitStrategy ( 3 )
FitLifeTime   = FitStrategy ( 4 )

class _MSG_Levels ( object ) :
    """
    Pseudo 'Import of MSG::Levels enum into Bender
    enum Level   {
    NIL = 0,
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    ALWAYS,
    NUM_LEVELS
    };
    """
    __slots__ = ( 'NIL'        ,
                  'VERBOSE'    ,
                  'DEBUG'      ,
                  'INFO'       ,
                  'WARNING'    ,
                  'ERROR'      ,
                  'FATAL'      ,
                  'ALWAYS'     ,
                  'NUM_LEVELS' )
    
MSG = _MSG_Levels()
MSG.NIL        = 0
MSG.VERBOSE    = 1
MSG.DEBUG      = 2
MSG.INFO       = 3
MSG.WARNING    = 4
MSG.ERROR      = 5
MSG.FATAL      = 6
MSG.ALWAYS     = 7
MSG.NUM_LEVELS = 8

# 'import' Vertex type enum 
class _VertexType(object) :
    """
    $PHYSEVENTROOT/xml/Vertex.xml:
    <enum
    desc   = "Describe how a vertex was constructed"
    name   = "VertexType"
    value  = "Unknown=0, Primary, Decay, DecayWithMass, Interaction, Brem, Pair, Kink"
    access = "PUBLIC"
    />
    """
    __slots__ = ( 'Unknown'       ,
                  'Primary'       ,
                  'Decay'         ,
                  'DecayWithMass' ,
                  'Interaction'   ,
                  'Brem'          ,
                  'Pair'          ,
                  'Kink'          )


VertexType = _VertexType()
VertexType.Unknown         = 0
VertexType.Primary         = 1
VertexType.Decay           = 2
VertexType.DecayWithMass   = 3
VertexType.Interaction     = 4
VertexType.Brem            = 5
VertexType.Pair            = 6
VertexType.Kink            = 7


fillStream             = gbl.Bender.FillStream.fillStream 

# import useful functions
vertex                 = gbl.Bender.Accessors.vertex
particle               = gbl.Bender.Accessors.particle
child                  = gbl.Bender.Child.child
select_min             = gbl.Bender.Select.select_min
select_max             = gbl.Bender.Select.select_max
SelectPrimaryVertexMin = gbl.Bender.Select.SelectPrimaryVertexMin
SelectPrimaryVertexMax = gbl.Bender.Select.SelectPrimaryVertexMax

pidFromName            = gbl.Bender.ParticleProperties.pidFromName
nameFromPID            = gbl.Bender.ParticleProperties.nameFromPID
ppFromName             = gbl.Bender.ParticleProperties.ppFromName
ppFromPID              = gbl.Bender.ParticleProperties.ppFromPID
massFromPID            = gbl.Bender.ParticleProperties.massFromPID
massFromName           = gbl.Bender.ParticleProperties.massFromName
ppService              = gbl.Bender.ParticleProperties.ppSvc

ppFromParticle         = gbl.Bender.Extract.ppFromParticle
trFromParticle         = gbl.Bender.Extract.trFromParticle
muFromParticle         = gbl.Bender.Extract.muFromParticle
richFromParticle       = gbl.Bender.Extract.richFromParticle
trgFromParticle        = gbl.Bender.Extract.trgFromParticle

particles              = gbl.Bender.Extract.particles
protoParticles         = gbl.Bender.Extract.protoParticles
tracks                 = gbl.Bender.Extract.tracks
#trgTracks             = gbl.Bender.Extract.trgTracks
origins                = gbl.Bender.Extract.origins

addMomenta             = gbl.Bender.Adder.addMomenta

DecayChain             = gbl.Bender.DecayChain

# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.13  2005/05/20 10:55:19  ibelyaev
#  prepare for v4r8
#
# Revision 1.12  2005/03/05 16:08:11  ibelyaev
#  add more functions from namespace LoKi::Extract
#
# Revision 1.11  2005/02/02 19:15:10  ibelyaev
#  add new functions
#
# =============================================================================
# The END 
# =============================================================================
