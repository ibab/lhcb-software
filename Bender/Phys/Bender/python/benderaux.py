#!/usr/bin/env python
# =============================================================================
# $Id: benderaux.py,v 1.4 2004-07-25 12:06:13 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.3  2004/07/24 14:05:00  ibelyaev
#  v3r5
#
# Revision 1.2  2004/07/23 13:49:23  ibelyaev
# *** empty log message ***
#
# Revision 1.1  2004/07/11 15:47:05  ibelyaev
#  regular incrment
#
# =============================================================================


# =============================================================================
# @file
# Helper file to define the auxilalry 'enums' and constants 
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

import PyLCGDict   
import gaudimodule as gaudi

PyLCGDict.loadDict( 'BenderDict'    )
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
