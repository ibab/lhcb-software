//=============================================================================
/** @file DeRichBeamPipe.cpp
 *
 * Implementation file for class : DeRichBeamPipe
 *
 * $Id: DeRichBeamPipe.cpp,v 1.5 2007-02-28 18:31:07 marcocle Exp $
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-11-27
 */
//=============================================================================

// Gaudi
#include "GaudiKernel/MsgStream.h"

// local
#include "RichDet/DeRichBeamPipe.h"

//=============================================================================

const CLID CLID_DERichBeamPipe = 12050;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichBeamPipe::DeRichBeamPipe() :
  m_localCone( NULL )
{ }

//=============================================================================
// Destructor
//=============================================================================
DeRichBeamPipe::~DeRichBeamPipe() {
  if ( m_localCone )
    delete m_localCone;
}


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
  MsgStream msg( msgSvc(), "DeRichBeamPipe" );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;
  msg << MSG::DEBUG << geometry()->lvolume()->name() << endmsg;

  m_solid = geometry()->lvolume()->solid();

  if ( m_solid->typeName() == "SolidCons" ) {
    const SolidCons* coneSolid = dynamic_cast<const SolidCons*>(m_solid);
    m_zHalfLength = coneSolid->zHalfLength();
    m_localCone = new SolidCons( "LocalRichCone",
                                 m_zHalfLength,
                                 coneSolid->outerRadiusAtMinusZ(),
                                 coneSolid->outerRadiusAtPlusZ()
                                 );
  }
  else {
    msg << MSG::FATAL << "Beam pipe solid is not a cone" << endmsg;
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
  const Gaudi::XYZPoint  pLocal( geometry()->toLocal(position) );
  const Gaudi::XYZVector vLocal( geometry()->matrix()*direction );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_localCone->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks) return NoIntersection;

  const Gaudi::XYZPoint entryLocal( pLocal + ticks[0] * vLocal );
  const bool frontFaceHit = ( entryLocal.z() < -m_zHalfLength + 1*Gaudi::Units::mm );
  entryPoint = geometry()->toGlobal( entryLocal );

  const Gaudi::XYZPoint exitLocal( pLocal + ticks[noTicks-1] * vLocal );
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
