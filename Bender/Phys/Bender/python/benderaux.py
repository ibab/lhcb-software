#!/usr/bin/env python
# =============================================================================
# $Id: benderaux.py,v 1.6 2004-08-26 19:34:36 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================


# =============================================================================
# @file
# Helper file to define the auxilalry 'enums' and constants 
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import gaudimodule as gaudi

gaudi.loaddict( 'BenderDict'    )
# global namespaces
gbl = gaudi.gbl

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
    __slots__ = ( 'NIL'       ,
                  'VERBOSE'   ,
                  'DEBUG'     ,
                  'INFO'      ,
                  'WARNING'   ,
                  'ERROR'     ,
                  'FATAL'     ,
                  'ALWAYS'    ,
                  'NUM_LEVEL' )

MSG = _MSG_Levels()
MSG.NIL       = 0
MSG.VERBOSE   = 1
MSG.DEBUG     = 2
MSG.INFO      = 3
MSG.WARNING   = 4
MSG.ERROR     = 5
MSG.FATAL     = 6
MSG.ALWAYS    = 7
MSG.NUM_LEVEL = 8

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
ppFromName             = gbl.Bender.ParticleProperties.ppFromName
ppFromPID              = gbl.Bender.ParticleProperties.ppFromPID
massFromPID            = gbl.Bender.ParticleProperties.massFromPID
massFromName           = gbl.Bender.ParticleProperties.massFromName
ppService              = gbl.Bender.ParticleProperties.ppSvc

ppFromParticle         = gbl.Bender.Extract.ppFromParticle
trFromParticle         = gbl.Bender.Extract.trFromParticle
muFromParticle         = gbl.Bender.Extract.muFromParticle
richFromParticle       = gbl.Bender.Extract.richFromParticle


# =============================================================================
# The END 
# =============================================================================
