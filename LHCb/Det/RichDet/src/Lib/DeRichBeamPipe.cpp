//=============================================================================
/** @file DeRichBeamPipe.cpp
 *
 * Implementation file for class : DeRichBeamPipe
 *
 * $Id: DeRichBeamPipe.cpp,v 1.2 2006-12-02 15:37:50 jonrob Exp $
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-11-27
 */
//=============================================================================

// Gaudi
#include "GaudiKernel/MsgStream.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidCons.h"

// local
#include "RichDet/DeRichBeamPipe.h"

// GSL
#include "gsl/gsl_math.h"
// Boost
#include "boost/lexical_cast.hpp"

//=============================================================================

const CLID& CLID_DERichBeamPipe = 12050;  // User defined

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
  const Gaudi::XYZPoint pLocal( geometry()->toLocal(position) );
  Gaudi::XYZVector vLocal( geometry()->matrix()*direction );

  ISolid::Ticks ticks;
  const unsigned int noTicks = m_localCone->intersectionTicks(pLocal, vLocal, ticks);

  if (0 == noTicks)  return NoIntersection;

  bool frontFaceHit( false );
  bool backFaceHit( false );

  Gaudi::XYZPoint entryLocal( pLocal + ticks[0] * vLocal );
  if ( entryLocal.z() < -m_zHalfLength + 1*Gaudi::Units::mm )
    frontFaceHit = true;
  entryPoint = geometry()->toGlobal( entryLocal );

  Gaudi::XYZPoint exitLocal( pLocal + ticks[noTicks-1] * vLocal );
  if ( exitLocal.z() > m_zHalfLength - 1*Gaudi::Units::mm )
    backFaceHit = true;
  exitPoint  = geometry()->toGlobal( exitLocal );

  BeamPipeIntersectionType hitType = ConeOnly;

  if ( frontFaceHit && backFaceHit ) {
    hitType = FrontAndBackFace;
  }
  else {
    if ( frontFaceHit ) {
      hitType = FrontFaceAndCone;
    }
    else if (backFaceHit ) {
      hitType = BackFaceAndCone;
    }
  }

  return hitType;
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
