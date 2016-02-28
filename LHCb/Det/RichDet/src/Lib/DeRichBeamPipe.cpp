
//=============================================================================
/** @file DeRichBeamPipe.cpp
 *
 * Implementation file for class : DeRichBeamPipe
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-11-27
 */
//=============================================================================

// local
#include "RichDet/DeRichBeamPipe.h"

//=============================================================================

const CLID CLID_DERichBeamPipe = 12050;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichBeamPipe::DeRichBeamPipe(const std::string & name) :
  DeRichBase  ( name )
{
  setMyName("DeRichBeamPipe");
}

//=============================================================================
// Destructor
//=============================================================================
DeRichBeamPipe::~DeRichBeamPipe() { delete m_localCone; }

// Retrieve Pointer to class defininition structure
const CLID& DeRichBeamPipe::classID()
{
  return CLID_DERichBeamPipe;
}

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichBeamPipe::initialize ( )
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Initialize " << name() << endmsg;
    debug() << geometry()->lvolume()->name() << endmsg;
  }

  m_solid = geometry()->lvolume()->solid();
  if ( m_solid )
  {
    if ( m_solid->typeName() == "SolidCons" ) 
    {
      const SolidCons* coneSolid = dynamic_cast<const SolidCons*>(m_solid);
      if ( coneSolid )
      {
        m_zHalfLength = coneSolid->zHalfLength();
        m_localCone = new SolidCons( "LocalRichCone",
                                     m_zHalfLength,
                                     coneSolid->outerRadiusAtMinusZ(),
                                     coneSolid->outerRadiusAtPlusZ()
                                     );
      }
      else
      {
        fatal() << "Problem casting to SolidCons" << endmsg;
        return StatusCode::FAILURE;
      }
    }
    else
    {
      fatal() << "Beam pipe solid is not a cone" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else
  {
    fatal() << "Problem loading Geometry" << endmsg;
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
}

//=========================================================================
// intersectionPoints
//=========================================================================
DeRichBeamPipe::BeamPipeIntersectionType
DeRichBeamPipe::intersectionPoints( const Gaudi::XYZPoint&  position,
                                    const Gaudi::XYZVector& direction,
                                    Gaudi::XYZPoint& entryPoint,
                                    Gaudi::XYZPoint& exitPoint ) const
{
  const auto pLocal = geometry()->toLocal(position);
  const auto vLocal = geometry()->toLocalMatrix()*direction;

  ISolid::Ticks ticks;
  const auto noTicks = m_localCone->intersectionTicks(pLocal, vLocal, ticks);

  if ( 0 == noTicks ) return NoIntersection;

  const auto entryLocal = pLocal + ticks[0] * vLocal;
  const bool frontFaceHit = ( entryLocal.z() < -m_zHalfLength + 1*Gaudi::Units::mm );
  entryPoint = geometry()->toGlobal( entryLocal );

  const auto exitLocal = pLocal + ticks[noTicks-1] * vLocal;
  const bool backFaceHit = ( exitLocal.z() > m_zHalfLength - 1*Gaudi::Units::mm );
  exitPoint  = geometry()->toGlobal( exitLocal );

  return ( frontFaceHit && backFaceHit ? FrontAndBackFace :
           frontFaceHit                ? FrontFaceAndCone :
           backFaceHit                 ? BackFaceAndCone  :
           ConeOnly );
}

//=========================================================================
//  text conversion
//=========================================================================
std::string
DeRichBeamPipe::text (const  DeRichBeamPipe::BeamPipeIntersectionType& type )
{
  switch( type ) {
  case DeRichBeamPipe::NoIntersection:   return "NoIntersection";
  case DeRichBeamPipe::FrontAndBackFace: return "FrontAndBackFace";
  case DeRichBeamPipe::FrontFaceAndCone: return "FrontFaceAndCone";
  case DeRichBeamPipe::BackFaceAndCone:  return "BackFaceAndCone";
  case DeRichBeamPipe::ConeOnly:         return "ConeOnly";
  default:                               return "SHOULD NEVER SEE THIS";
  }

}
